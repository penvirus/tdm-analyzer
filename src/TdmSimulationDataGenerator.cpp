#include "TdmSimulationDataGenerator.h"
#include "TdmAnalyzerSettings.h"

#include <AnalyzerHelpers.h>

TdmSimulationDataGenerator::TdmSimulationDataGenerator()
:	mAudioSampleRate( 16000 ),
	mNumberOfPaddingBits( 0 )
{
}

TdmSimulationDataGenerator::~TdmSimulationDataGenerator()
{
}

void TdmSimulationDataGenerator::Initialize( U32 simulation_sample_rate, TdmAnalyzerSettings* settings )
{
	mSimulationSampleRateHz = simulation_sample_rate;
	mSettings = settings;

	if( mSettings->mDataValidEdge == AnalyzerEnums::NegEdge )
		mClock = mSimulationChannels.Add( mSettings->mClockChannel, mSimulationSampleRateHz, BIT_LOW );
	else
		mClock = mSimulationChannels.Add( mSettings->mClockChannel, mSimulationSampleRateHz, BIT_HIGH );

	if( mSettings->mFramePulseEdge == AnalyzerEnums::NegEdge )
		mFrame = mSimulationChannels.Add( mSettings->mFrameChannel, mSimulationSampleRateHz, BIT_LOW );
	else
		mFrame = mSimulationChannels.Add( mSettings->mFrameChannel, mSimulationSampleRateHz, BIT_HIGH );

	mData = mSimulationChannels.Add( mSettings->mDataChannel, mSimulationSampleRateHz, BIT_LOW );

	/* rising + falling */
	mClockGenerator.Init( mAudioSampleRate * 2, mSimulationSampleRateHz );

	mCounter = 0;
	mAudioData = 0;
}

U32 TdmSimulationDataGenerator::GenerateSimulationData( U64 largest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channels )
{
	U64 adjusted_largest_sample_requested = AnalyzerHelpers::AdjustSimulationTargetSample( largest_sample_requested, sample_rate, mSimulationSampleRateHz );

	while( mData->GetCurrentSampleNumber() < adjusted_largest_sample_requested )
	{
		WriteBit( GetNextDataBit(), GetNextFrameBit() );
		mCounter++;
	}

	*simulation_channels = mSimulationChannels.GetArray();
	return mSimulationChannels.GetCount();
}

void TdmSimulationDataGenerator::WriteBit( BitState data, BitState frame )
{
	mSimulationChannels.AdvanceAll( mClockGenerator.AdvanceByHalfPeriod( 1.0 ) );
	mClock->Transition();

	mFrame->TransitionIfNeeded( frame );
	mData->TransitionIfNeeded( data );

	mSimulationChannels.AdvanceAll( mClockGenerator.AdvanceByHalfPeriod( 1.0 ) );
	mClock->Transition();
}

#define BIT(n) (1ULL << n)
BitState TdmSimulationDataGenerator::GetNextDataBit()
{
	U32 word_length = ( ( mSettings->mBitsPerWord + ( 16 - 1 ) ) / 16 * 16 );
	U32 frame_size = word_length * mSettings->mNumberOfDataChannels + mNumberOfPaddingBits;
	if( mNumberOfPaddingBits != 0 )
		frame_size += mSettings->mMsbOffset;

	//skip the first 4 frames
	if( mCounter < frame_size * 4 )
		return BIT_LOW;

	U32 offset_in_frame = mCounter % frame_size;
	//paddings
	if( offset_in_frame >= word_length * mSettings->mNumberOfDataChannels + mSettings->mMsbOffset )
		return BIT_LOW;
	//offset
	if( offset_in_frame < mSettings->mMsbOffset )
		return BIT_LOW;

	U32 offset_in_word = ( offset_in_frame - mSettings->mMsbOffset ) % word_length;
	//paddings
	if( offset_in_word >= mSettings->mBitsPerWord )
		return BIT_LOW;

	BitState ret = BIT_LOW;

	if( ( mAudioData & BIT( mSettings->mBitsPerWord - 1 - offset_in_word ) ) != 0 )
		ret = BIT_HIGH;

	if( offset_in_word == 0 )
		if( ++mAudioData > 0xFFFF )
			mAudioData = 0;

	return ret;
}

BitState TdmSimulationDataGenerator::GetNextFrameBit()
{
	U32 word_length = ( ( mSettings->mBitsPerWord + ( 16 - 1 ) ) / 16 * 16 );
	U32 frame_size = word_length * mSettings->mNumberOfDataChannels + mNumberOfPaddingBits;
	if( mNumberOfPaddingBits != 0 )
		frame_size += mSettings->mMsbOffset;

	//skip the first 2 frames
	if( mCounter < frame_size * 2 )
		return mFrame->GetCurrentBitState();

	if( mCounter % frame_size == 0 )
	{
		if( mSettings->mFramePulseEdge == AnalyzerEnums::NegEdge )
			return BIT_LOW;
		else
			return BIT_HIGH;
	}else
	{
		if( mSettings->mFramePulseEdge == AnalyzerEnums::NegEdge )
			return BIT_HIGH;
		else
			return BIT_LOW;
	}
}
