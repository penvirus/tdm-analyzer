#ifndef TDM_ANALYZER_RESULTS
#define TDM_ANALYZER_RESULTS

#include <AnalyzerResults.h>

enum TdmResultType {
	Channel1 = 1,
	Channel2,
	Channel3,
	Channel4,
	Channel5,
	Channel6,
	Channel7,
	Channel8,
	ErrorTooFewBits,
};

class TdmAnalyzer;
class TdmAnalyzerSettings;

class TdmAnalyzerResults : public AnalyzerResults
{
public:
	TdmAnalyzerResults( TdmAnalyzer* analyzer, TdmAnalyzerSettings* settings );
	virtual ~TdmAnalyzerResults();

	virtual void GenerateBubbleText( U64 frame_index, Channel& channel, DisplayBase display_base );
	virtual void GenerateExportFile( const char* file, DisplayBase display_base, U32 export_type_user_id );

	virtual void GenerateFrameTabularText( U64 frame_index, DisplayBase display_base );
	virtual void GeneratePacketTabularText( U64 packet_id, DisplayBase display_base );
	virtual void GenerateTransactionTabularText( U64 transaction_id, DisplayBase display_base );

protected: //functions

protected:  //vars
	TdmAnalyzerSettings* mSettings;
	TdmAnalyzer* mAnalyzer;
};

#endif //TDM_ANALYZER_RESULTS
