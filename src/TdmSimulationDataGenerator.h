#ifndef TDM_SIMULATION_DATA_GENERATOR
#define TDM_SIMULATION_DATA_GENERATOR

#include <SimulationChannelDescriptor.h>
#include "AnalyzerHelpers.h"

class TdmAnalyzerSettings;

class TdmSimulationDataGenerator
{
public:
	TdmSimulationDataGenerator();
	~TdmSimulationDataGenerator();

	void Initialize( U32 simulation_sample_rate, TdmAnalyzerSettings* settings );
	U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channels );

protected: //vars
	TdmAnalyzerSettings* mSettings;
	U32 mSimulationSampleRateHz;

	SimulationChannelDescriptorGroup mSimulationChannels;
	SimulationChannelDescriptor* mClock;
	SimulationChannelDescriptor* mFrame;
	SimulationChannelDescriptor* mData;

	U32 mAudioSampleRate;
	U32 mNumberOfPaddingBits;

	ClockGenerator mClockGenerator;

	U64 mCounter;
	U32 mAudioData;

protected: //functions
	void WriteBit( BitState data, BitState frame );
	BitState GetNextDataBit();
	BitState GetNextFrameBit();

};
#endif //TDM_SIMULATION_DATA_GENERATOR
