#include "TdmAnalyzerResults.h"
#include <AnalyzerHelpers.h>
#include "TdmAnalyzer.h"
#include "TdmAnalyzerSettings.h"
#include <iostream>
#include <fstream>

TdmAnalyzerResults::TdmAnalyzerResults( TdmAnalyzer* analyzer, TdmAnalyzerSettings* settings )
:	AnalyzerResults(),
	mSettings( settings ),
	mAnalyzer( analyzer )
{
}

TdmAnalyzerResults::~TdmAnalyzerResults()
{
}

void TdmAnalyzerResults::GenerateBubbleText( U64 frame_index, Channel& channel, DisplayBase display_base )
{
	static const char* ch_names[] = { "1", "2", "3", "4", "5", "6", "7", "8" };
	ClearResultStrings();
	Frame frame = GetFrame( frame_index );

	char number_str[128];
	AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 8, number_str, 128 );

	U32 ch = TdmResultType( frame.mType );
	if( ch >= Channel1 && ch <= Channel8 )
	{
		AddResultString( ch_names[ ch - Channel1 ] );
		AddResultString( "Ch", ch_names[ ch - Channel1 ] );
		AddResultString( "Ch", ch_names[ ch - Channel1 ], ": ", number_str );
	}else if( ch == ErrorTooFewBits )
	{
		char bits_per_word[32];
		sprintf( bits_per_word, "%d", mSettings->mBitsPerWord );

		AddResultString( "Error" );
		AddResultString( "Error: too few bits, expecting ", bits_per_word );
	}
}

void TdmAnalyzerResults::GenerateExportFile( const char* file, DisplayBase display_base, U32 export_type_user_id )
{
	void* f = AnalyzerHelpers::StartFile( file, true /* is_binary */ );
	U64 num_frames = GetNumFrames();

	for( U64 i=0; i < num_frames; i++ )
	{
		Frame frame = GetFrame( i );
		U64 data = frame.mData1;
		U32 data_length = ( mSettings->mBitsPerWord + ( 8 - 1 ) ) / 8;
		U8* data_p = (U8*)&data;

		U32 ch = TdmResultType( frame.mType );
		if( ch >= Channel1 && ch <= Channel8 )
		{
			AnalyzerHelpers::AppendToFile( data_p, data_length, f );
		}else
		{
			AnalyzerHelpers::EndFile( f );
			return;
		}

		if( UpdateExportProgressAndCheckForCancel( i, num_frames ) == true )
		{
			AnalyzerHelpers::EndFile( f );
			return;
		}
	}

	UpdateExportProgressAndCheckForCancel( num_frames, num_frames );
	AnalyzerHelpers::EndFile( f );
}

void TdmAnalyzerResults::GenerateFrameTabularText( U64 frame_index, DisplayBase display_base )
{
#if 0
	Frame frame = GetFrame( frame_index );
	ClearTabularText();

	char number_str[128];
	AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 8, number_str, 128 );
	AddTabularText( number_str );
#endif
}

void TdmAnalyzerResults::GeneratePacketTabularText( U64 packet_id, DisplayBase display_base )
{
	//not supported
}

void TdmAnalyzerResults::GenerateTransactionTabularText( U64 transaction_id, DisplayBase display_base )
{
	//not supported
}
