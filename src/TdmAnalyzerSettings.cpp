#include "TdmAnalyzerSettings.h"
#include <AnalyzerHelpers.h>


TdmAnalyzerSettings::TdmAnalyzerSettings()
:	mClockChannel( UNDEFINED_CHANNEL ),
	mFrameChannel( UNDEFINED_CHANNEL ),
	mDataChannel( UNDEFINED_CHANNEL ),

	mFramePulseEdge( AnalyzerEnums::NegEdge ),
	mMsbOffset ( 1 ),
	mDataValidEdge( AnalyzerEnums::NegEdge ),
	mBitsPerWord( 16 ),
	mNumberOfDataChannels ( 8 )
{
	mClockChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
	mClockChannelInterface->SetTitleAndTooltip( "SCLK", "Serial Clock" );
	mClockChannelInterface->SetChannel( mClockChannel );

	mFrameChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
	mFrameChannelInterface->SetTitleAndTooltip( "FSYNC", "Frame Synchronization Pulse" );
	mFrameChannelInterface->SetChannel( mFrameChannel );

	mDataChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
	mDataChannelInterface->SetTitleAndTooltip( "SDATA", "Serial Audio Data" );
	mDataChannelInterface->SetChannel( mDataChannel );

	mFramePulseEdgeInterface.reset( new AnalyzerSettingInterfaceNumberList() );
	mFramePulseEdgeInterface->SetTitleAndTooltip( "Frame CLOCK State", "Specify if frame pulse is valid on the rising, or falling clock edge." );
	mFramePulseEdgeInterface->AddNumber( AnalyzerEnums::NegEdge, "Frame pulse is valid on falling edge", "" );
	mFramePulseEdgeInterface->AddNumber( AnalyzerEnums::PosEdge, "Frame pulse is valid on rising edge", "" );
	mFramePulseEdgeInterface->SetNumber( mFramePulseEdge );

	mMsbOffsetInterface.reset( new AnalyzerSettingInterfaceNumberList() );
	mMsbOffsetInterface->SetTitleAndTooltip( "MSB Offset", "Specify how many offsets need skip w.r.t. frame pulse" );
	mMsbOffsetInterface->AddNumber( 0, "No offset", "" );
	mMsbOffsetInterface->AddNumber( 1, "1 SCLK offset", "" );
	mMsbOffsetInterface->SetNumber( mMsbOffset );

	mDataValidEdgeInterface.reset( new AnalyzerSettingInterfaceNumberList() );
	mDataValidEdgeInterface->SetTitleAndTooltip( "Data CLOCK State", "Specify if data is valid (should be read) on the rising, or falling clock edge." );
	mDataValidEdgeInterface->AddNumber( AnalyzerEnums::NegEdge, "Data is valid on falling edge", "" );
	mDataValidEdgeInterface->AddNumber( AnalyzerEnums::PosEdge, "Data is valid on rising edge", "" );
	mDataValidEdgeInterface->SetNumber( mDataValidEdge );

	mBitsPerWordInterface.reset( new AnalyzerSettingInterfaceNumberList() );
	mBitsPerWordInterface->SetTitleAndTooltip( "Audio Bit Depth (bits/sample)", "Specify the channel block width." );
	mBitsPerWordInterface->AddNumber( 16, "16 Bits / 16 Bits", "" );
	mBitsPerWordInterface->AddNumber( 24, "24 Bits / 32 Bits", "" );
	mBitsPerWordInterface->AddNumber( 32, "32 Bits / 32 Bits", "" );
	mBitsPerWordInterface->SetNumber( mBitsPerWord );

	mNumberOfDataChannelsInterface.reset( new AnalyzerSettingInterfaceNumberList() );
	mNumberOfDataChannelsInterface->SetTitleAndTooltip( "Number of Channels", "Specify the number of channels." );
	for( U32 i=1; i<=8; i++ )
	{
		char str[128];
		sprintf( str, "%d ch", i );
		mNumberOfDataChannelsInterface->AddNumber( i, str, "" );
	}
	mNumberOfDataChannelsInterface->SetNumber( mNumberOfDataChannels );

	AddInterface( mClockChannelInterface.get() );
	AddInterface( mFrameChannelInterface.get() );
	AddInterface( mDataChannelInterface.get() );
	AddInterface( mFramePulseEdgeInterface.get() );
	AddInterface( mMsbOffsetInterface.get() );
	AddInterface( mDataValidEdgeInterface.get() );
	AddInterface( mBitsPerWordInterface.get() );
	AddInterface( mNumberOfDataChannelsInterface.get() );

	AddExportOption( 0, "Export as raw audio file" );
	AddExportExtension( 0, "raw audio", "raw" );

	ClearChannels();
	AddChannel( mClockChannel, "SCLK", false );
	AddChannel( mFrameChannel, "FSYNC", false );
	AddChannel( mDataChannel, "SDATA", false );
}

TdmAnalyzerSettings::~TdmAnalyzerSettings()
{
}

bool TdmAnalyzerSettings::SetSettingsFromInterfaces()
{
	Channel clock_channel = mClockChannelInterface->GetChannel();
	if( clock_channel == UNDEFINED_CHANNEL )
	{
		SetErrorText( "Please select a channel for SCLK signal" );
		return false;
	}

	Channel frame_channel = mFrameChannelInterface->GetChannel();
	if( frame_channel == UNDEFINED_CHANNEL )
	{
		SetErrorText( "Please select a channel for FSYNC signal" );
		return false;
	}

	Channel data_channel = mDataChannelInterface->GetChannel();
	if( data_channel == UNDEFINED_CHANNEL )
	{
		SetErrorText( "Please select a channel for SDATA signal" );
		return false;
	}

	if( ( clock_channel == frame_channel ) || ( clock_channel == data_channel ) || ( frame_channel == data_channel ) )
	{
		SetErrorText( "Please select different channels for the TDM / I2S signals" );
		return false;
	}

	mClockChannel = clock_channel;
	mFrameChannel = frame_channel;
	mDataChannel = data_channel;

	mFramePulseEdge = AnalyzerEnums::EdgeDirection( U32( mFramePulseEdgeInterface->GetNumber() ) );
	mMsbOffset = U32( mMsbOffsetInterface->GetNumber() );
	mDataValidEdge = AnalyzerEnums::EdgeDirection( U32( mDataValidEdgeInterface->GetNumber() ) );
	mBitsPerWord = U32( mBitsPerWordInterface->GetNumber() );
	mNumberOfDataChannels = U32( mNumberOfDataChannelsInterface->GetNumber() );

	ClearChannels();
	AddChannel( mClockChannel, "SCLK", true );
	AddChannel( mFrameChannel, "FSYNC", true );
	AddChannel( mDataChannel, "SDATA", true );

	return true;
}

void TdmAnalyzerSettings::UpdateInterfacesFromSettings()
{
	mClockChannelInterface->SetChannel( mClockChannel );
	mFrameChannelInterface->SetChannel( mFrameChannel );
	mDataChannelInterface->SetChannel( mDataChannel );

	mFramePulseEdgeInterface->SetNumber( mFramePulseEdge );
	mMsbOffsetInterface->SetNumber( mMsbOffset );
	mDataValidEdgeInterface->SetNumber( mDataValidEdge );
	mBitsPerWordInterface->SetNumber( mBitsPerWord );
	mNumberOfDataChannelsInterface->SetNumber( mNumberOfDataChannels );
}

void TdmAnalyzerSettings::LoadSettings( const char* settings )
{
	SimpleArchive text_archive;
	text_archive.SetString( settings );

	const char* name_string;        //the first thing in the archive is the name of the protocol analyzer that the data belongs to.
	text_archive >> &name_string;
	if( strcmp( name_string, "SaleaeTdmAnalyzer" ) != 0 )
		AnalyzerHelpers::Assert( "SaleaeTdmAnalyzer: Provided with a settings string that doesn't belong to us;" );

	text_archive >> mClockChannel;
	text_archive >> mFrameChannel;
	text_archive >> mDataChannel;

	text_archive >> *(U32*)&mFramePulseEdge;
	text_archive >> mMsbOffset;
	text_archive >> *(U32*)&mDataValidEdge;
	text_archive >> mBitsPerWord;
	text_archive >> mNumberOfDataChannels;

	ClearChannels();
	AddChannel( mClockChannel, "SCLK", true );
	AddChannel( mFrameChannel, "FSYNC", true );
	AddChannel( mDataChannel, "SDATA", true );

	UpdateInterfacesFromSettings();
}

const char* TdmAnalyzerSettings::SaveSettings()
{
	SimpleArchive text_archive;

	text_archive << "SaleaeTdmAnalyzer";

	text_archive << mClockChannel;
	text_archive << mFrameChannel;
	text_archive << mDataChannel;

	text_archive << mFramePulseEdge;
	text_archive << mMsbOffset;
	text_archive << mDataValidEdge;
	text_archive << mBitsPerWord;
	text_archive << mNumberOfDataChannels;

	return SetReturnString( text_archive.GetString() );
}
