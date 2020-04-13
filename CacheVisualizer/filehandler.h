/*** File Handler
 * Class designed to handle I/O for the visualizer. It implements the functionality to
 * handle 2 types of files:
 * The configuration file
 * The memory trace file that is used as input
 *
 ***/
#pragma once

#include <map>
#include <string>
#include <string_view>
#include <fstream>
#include <sstream>
#include <iostream>
#include <QDebug>
#include <charconv> //C++17 & updated compiler

#include "scopedtimer.h"
#include "settings.h"
#include "executionsimulation.h"

class FileHandler
{
    const std::string DEF_FILENAME = "configuration";

    std::map<std::string, int> config_attributes;

    /*Private methods for checking for non-negative numbers & handling of exceptions
      when reading input.*/
    bool checkAttValValidityInt(const std::string& inValue, int lower_limit);
    bool checkAttValValidityIntRestrict(const std::string& inValue);
    bool checkAttValValidityLL(const std::string& inValue);

    /*Private methods for encapsulating the extraction process. Used by readTraceData()*/
    inline void extractLaunchConfig(std::string_view o_line, ExecutionSimulation *sim) const;

    inline void extractBlockScheduling(std::string_view o_line, int& bx, int& by, int& bz, int& sm, long long& gt) const;

    inline void extractTraceEntry(std::string_view o_line, int& bx, int& by, int& wid, char& dname, char& oper, unsigned int& idx, unsigned long long &address, unsigned long long &time) const;

public:
    FileHandler();

    /*Initializes a map that is used to check for configuration values validity*/
    void initConfigMap();

    /*Performs basic validity checking for values in the configuration file
      (i.e. non 0 values, every required attribute is present, etc...*/
    bool checkConfigValidity(const std::string& fname);

    /*Reads in the config file and updates the settings*/
    bool readConfig(const std::string& fname);

    /*Reference Qobject implementation*/
    bool readTraceDataFromQstream(QTextStream& trace_data, ExecutionSimulation *sim);

    /*Lightweight method for reading the trace data*/
    bool readTraceData(std::ifstream& in_stream, ExecutionSimulation *sim);

};
