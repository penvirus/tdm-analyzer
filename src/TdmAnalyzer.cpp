#include "TdmAnalyzer.h"
#include "TdmAnalyzerSettings.h"
#include <AnalyzerChannelData.h>

TdmAnalyzer::TdmAnalyzer()
:	Analyzer2(),
	mSettings( new TdmAnalyzerSettings() ),
	mSimulationInitilized( false )
{
	SetAnalyzerSettings( mSettings.get() );
}

TdmAnalyzer::~TdmAnalyzer()
{
	KillThread();
}

void TdmAnalyzer::SetupResults()
{
	mResults.reset( new TdmAnalyzerResults( this, mSettings.get() ) );
	SetAnalyzerResults( mResults.get() );
	mResults->AddChannelBubblesWillAppearOn( mSettings->mDataChannel );
}

void TdmAnalyzer::WorkerThread()
{
	if( mSettings->mDataValidEdge == AnalyzerEnums::NegEdge )
		mArrowMarker = AnalyzerResults::DownArrow;
	else
		mArrowMarker = AnalyzerResults::UpArrow;

	mClock = GetAnalyzerChannelData( mSettings->mClockChannel );
	mFrame = GetAnalyzerChannelData( mSettings->mFrameChannel );
	mData = GetAnalyzerChannelData( mSettings->mDataChannel );

	AdvanceToNextFrame();

	for( ; ; )
	{
		AnalyzeFrame();
		mResults->CommitResults();
		ReportProgress( mClock->GetSampleNumber() );

		CheckIfThreadShouldExit();

		if( IsFrameReadyToTrigger() )
			AdvanceToNextFrame();
	}
}

bool TdmAnalyzer::IsFrameReadyToTrigger()
{
	return ! ( ( mSettings->mFramePulseEdge == AnalyzerEnums::PosEdge && mFrame->GetBitState() == BIT_HIGH ) ||
		   ( mSettings->mFramePulseEdge == AnalyzerEnums::NegEdge && mFrame->GetBitState() == BIT_LOW ) );
}

void TdmAnalyzer::AdvanceToNextFrame()
{
	U64 sample;

	if( ! IsFrameReadyToTrigger() )
	{
		mFrame->AdvanceToNextEdge();
	}

	mFrame->AdvanceToNextEdge();
	sample = mFrame->GetSampleNumber();
	mClock->AdvanceToAbsPosition( sample );
	mData->AdvanceToAbsPosition( sample );

	if( mSettings->mMsbOffset == 1 )
		AdvanceToNextBit();
}

bool TdmAnalyzer::IsDataReadyToTrigger()
{
	return ! ( ( mSettings->mDataValidEdge == AnalyzerEnums::PosEdge && mClock->GetBitState() == BIT_HIGH ) ||
		   ( mSettings->mDataValidEdge == AnalyzerEnums::NegEdge && mClock->GetBitState() == BIT_LOW ) );
}

U32 TdmAnalyzer::AdvanceToNextBit()
{
	U64 sample;
	U32 frame_changed;

	if( ! IsDataReadyToTrigger() )
	{
		mClock->AdvanceToNextEdge();
	}

	mClock->AdvanceToNextEdge();
	sample = mClock->GetSampleNumber();
	frame_changed = mFrame->AdvanceToAbsPosition( sample );
	mData->AdvanceToAbsPosition( sample );

	return frame_changed;
}

void TdmAnalyzer::AnalyzeFrame()
{
	U64 data;
	U32 frame_changed;
	Frame frame;
	U32 word_length = ( mSettings->mBitsPerWord + ( 16 - 1 ) ) / 16 * 16;

	for( U32 ch=1; ch<=mSettings->mNumberOfDataChannels; ch++ )
	{
		data = 0;

		for( U32 i=0; i<word_length; i++ )
		{
			frame_changed = AdvanceToNextBit();
			if( frame_changed != 0 && ! IsFrameReadyToTrigger() && ( i != word_length - 1 || ch != mSettings->mNumberOfDataChannels ) )
			{
				frame.mData1 = 0;
				frame.mType = ErrorTooFewBits;
				frame.mFlags = 0;
				mResults->AddFrame(frame);
				return;
			}

			if( i == 0 )
				frame.mStartingSampleInclusive = mData->GetSampleNumber();
			else if( i == mSettings->mBitsPerWord - 1 )
				frame.mEndingSampleInclusive = mData->GetSampleNumber();

			if( i < mSettings->mBitsPerWord )
			{
				data <<= 1;
				if( mData->GetBitState() == BIT_HIGH )
					data |= 1;

				mResults->AddMarker( mClock->GetSampleNumber(), mArrowMarker, mSettings->mClockChannel );
			}
		}

		frame.mData1 = data;
		frame.mType = ch;
		frame.mFlags = 0;
		mResults->AddFrame(frame);
	}
}

bool TdmAnalyzer::NeedsRerun()
{
	return false;
}

U32 TdmAnalyzer::GenerateSimulationData( U64 minimum_sample_index, U32 device_sample_rate, SimulationChannelDescriptor** simulation_channels )
{
	if( mSimulationInitilized == false )
	{
		mSimulationDataGenerator.Initialize( GetSimulationSampleRate(), mSettings.get() );
		mSimulationInitilized = true;
	}

	return mSimulationDataGenerator.GenerateSimulationData( minimum_sample_index, device_sample_rate, simulation_channels );
}

U32 TdmAnalyzer::GetMinimumSampleRateHz()
{
	return 25000;
}

const char* TdmAnalyzer::GetAnalyzerName() const
{
	return "TDM / I2S";
}

const char* GetAnalyzerName()
{
	return "TDM / I2S";
}

Analyzer* CreateAnalyzer()
{
	return new TdmAnalyzer();
}

void DestroyAnalyzer( Analyzer* analyzer )
{
	delete analyzer;
}
