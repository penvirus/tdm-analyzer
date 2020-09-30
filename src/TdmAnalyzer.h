#ifndef TDM_ANALYZER_H
#define TDM_ANALYZER_H

#include <Analyzer.h>
#include "TdmAnalyzerResults.h"
#include "TdmSimulationDataGenerator.h"

class TdmAnalyzerSettings;
class ANALYZER_EXPORT TdmAnalyzer : public Analyzer2
{
public:
	TdmAnalyzer();
	virtual ~TdmAnalyzer();

	virtual void SetupResults();
	virtual void WorkerThread();

	virtual U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channels );
	virtual U32 GetMinimumSampleRateHz();

	virtual const char* GetAnalyzerName() const;
	virtual bool NeedsRerun();

protected: //functions
	void AnalyzeFrame();

	bool IsDataReadyToTrigger();
	void AdvanceToNextFrame();
	bool IsFrameReadyToTrigger();
	U32 AdvanceToNextBit();

protected: //vars
	std::auto_ptr< TdmAnalyzerSettings > mSettings;
	std::auto_ptr< TdmAnalyzerResults > mResults;

	TdmSimulationDataGenerator mSimulationDataGenerator;
	bool mSimulationInitilized;

	AnalyzerChannelData* mClock;
	AnalyzerChannelData* mFrame;
	AnalyzerChannelData* mData;

	AnalyzerResults::MarkerType mArrowMarker;
};

extern "C" ANALYZER_EXPORT const char* __cdecl GetAnalyzerName();
extern "C" ANALYZER_EXPORT Analyzer* __cdecl CreateAnalyzer( );
extern "C" ANALYZER_EXPORT void __cdecl DestroyAnalyzer( Analyzer* analyzer );

#endif //TDM_ANALYZER_H
