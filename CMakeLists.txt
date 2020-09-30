cmake_minimum_required (VERSION 3.11)
project(tdm_analyzer)

# custom CMake Modules are located in the cmake directory.
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

include(ExternalAnalyzerSDK)

set(SOURCES 
src/TdmAnalyzer.cpp
src/TdmAnalyzer.h
src/TdmAnalyzerResults.cpp
src/TdmAnalyzerResults.h
src/TdmAnalyzerSettings.cpp
src/TdmAnalyzerSettings.h
src/TdmSimulationDataGenerator.cpp
src/TdmSimulationDataGenerator.h
)

add_analyzer_plugin(tdm_analyzer SOURCES ${SOURCES})
