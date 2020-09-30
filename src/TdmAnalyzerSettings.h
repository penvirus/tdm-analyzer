#ifndef TDM_ANALYZER_SETTINGS
#define TDM_ANALYZER_SETTINGS

#include <AnalyzerSettings.h>
#include <AnalyzerTypes.h>

class TdmAnalyzerSettings : public AnalyzerSettings
{
public:
	TdmAnalyzerSettings();
	virtual ~TdmAnalyzerSettings();

	virtual bool SetSettingsFromInterfaces();
	void UpdateInterfacesFromSettings();
	virtual void LoadSettings( const char* settings );
	virtual const char* SaveSettings();

	
	Channel mClockChannel;
	Channel mFrameChannel;
	Channel mDataChannel;

	AnalyzerEnums::EdgeDirection mFramePulseEdge;
	U32 mMsbOffset;
	AnalyzerEnums::EdgeDirection mDataValidEdge;
	U32 mBitsPerWord;
	U32 mNumberOfDataChannels;

protected:
	std::auto_ptr< AnalyzerSettingInterfaceChannel > mClockChannelInterface;
	std::auto_ptr< AnalyzerSettingInterfaceChannel > mFrameChannelInterface;
	std::auto_ptr< AnalyzerSettingInterfaceChannel > mDataChannelInterface;

	std::auto_ptr< AnalyzerSettingInterfaceNumberList > mFramePulseEdgeInterface;
	std::auto_ptr< AnalyzerSettingInterfaceNumberList > mMsbOffsetInterface;
	std::auto_ptr< AnalyzerSettingInterfaceNumberList > mDataValidEdgeInterface;
	std::auto_ptr< AnalyzerSettingInterfaceNumberList > mBitsPerWordInterface;
	std::auto_ptr< AnalyzerSettingInterfaceNumberList > mNumberOfDataChannelsInterface;
};

#endif //TDM_ANALYZER_SETTINGS
