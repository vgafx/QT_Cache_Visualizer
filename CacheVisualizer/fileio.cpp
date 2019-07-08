#include "fileio.h"
#include "globals.h"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <iomanip>
#include <map>


#define FILENAME "configuration"

using namespace std;

std::map<std::string,int> config_att;


fileIO::fileIO()
{
}


void initConfigMap(){
    config_att["name"] = 0;
    config_att["cudacores"] = 0;
    config_att["smm"] = 0;
    config_att["warpsize"] = 0;
    config_att["concurrentwarps"] = 0;
    config_att["cacheline"] = 0;
    config_att["issectored"] = 0;
    config_att["sectorsize"] = 0;
    config_att["waysizel1"] = 0;
    config_att["waysizel2"] = 0;
    config_att["l1enabled"] = 0;
    config_att["replacementpolicy"] = 0;
    config_att["writepolicy"] = 0;
    config_att["l1loadgranularity"] = 0;
    config_att["l2loadgranularity"] = 0;
    config_att["global"] = 0;
    config_att["l2size"] = 0;
    config_att["l1size"] = 0;

}

void readConfig(){
    std::ifstream configFile;
    configFile.open(FILENAME);


    if ( configFile.is_open()){
        std::string line;
        while (getline(configFile,line)){
            if(line[0] == '#' || line.empty()){continue;}
            std::istringstream tokenizer(line);
            std::string attribute, value;
            std::getline(tokenizer, attribute, '=');
            std::getline(tokenizer, value);

            printf("Tokenizer: att=%s,val=%s\n", attribute.c_str(), value.c_str());

            if (attribute=="name"){
                name = value;
                config_att["name"] = 1;
            } else if(attribute=="cudacores"){
                cuda_cores = std::stoi(value);
                config_att["cudacores"] = 1;
            } else if(attribute=="smm"){
                num_sm = std::stoi(value);
                config_att["smm"] = 1;
            } else if(attribute=="warpsize"){
                warp_size = std::stoi(value);
                config_att["warpsize"] = 1;
            } else if(attribute=="concurrentwarps"){
                concurrent_warps = std::stoi(value);
                config_att["concurrentwarps"] = 1;
            } else if(attribute=="cacheline"){
                line_size = std::stoi(value);
                config_att["cacheline"] = 1;
            } else if(attribute=="issectored"){
                is_sectored = std::stoi(value);
                config_att["issectored"] = 1;
            } else if(attribute=="sectorsize"){
                sector_size = std::stoi(value);
                config_att["sectorsize"] = 1;
            } else if(attribute=="waysizel1"){
                way_size_l1 = std::stoi(value);
                config_att["waysizel1"] = 1;
            } else if(attribute=="waysizel2"){
                way_size_l2 = std::stoi(value);
                config_att["waysizel2"] = 1;
            } else if(attribute=="l1enabled"){
                is_l1_enabled = std::stoi(value);
                config_att["l1enabled"] = 1;
            } else if(attribute=="replacementpolicy"){
                replacement_policy = std::stoi(value);
                config_att["replacementpolicy"] = 1;
            } else if(attribute=="writepolicy"){
                write_policy = std::stoi(value);
                config_att["writepolicy"] = 1;
            } else if(attribute=="l1loadgranularity"){
                l1_load_granularity = std::stoi(value);
                config_att["l1loadgranularity"] = 1;
            } else if(attribute=="l2loadgranularity"){
                l2_load_granularity = std::stoi(value);
                config_att["l2loadgranularity"] = 1;
            } else if(attribute=="global"){
                global_size = std::stoll(value);
                config_att["global"] = 1;
            } else if(attribute=="l2size"){
                l2_size = std::stoi(value);
                config_att["l2size"] = 1;
            } else if(attribute=="l1size"){
                l1_size = std::stoi(value);
                config_att["l1size"] = 1;
            } else {
                printf("Unknown Option\n");
            }

        }

        map<std::string, int>::iterator it;
        for (it = config_att.begin(); it != config_att.end(); it++){
            if(it->second != 1){
                missingConfigAttribute = true;
            }
        }

        for (it = config_att.begin(); it != config_att.end(); it++){
            it->second = 0;
        }

        configFile.close();
    } else {
        //!!Consider using a visual exception in this case
        printf("Failed to open configuration file\n");
    }
}


bool readConfigFromQstream(QTextStream &configData){

    while(!configData.atEnd()){
        QString line = configData.readLine();
        if(!line.isEmpty() && line.at(0) != '#'){
            QStringList linesplit = line.split("=");
            std::string att;
            att = linesplit[0].toStdString();
            config_att[att] = 1;
        }
    }

    map<std::string, int>::iterator it;
    for (it = config_att.begin(); it != config_att.end(); it++){
        if(it->second != 1){
            printf("Menu:Option missing\n");
            missingConfigAttribute = true;
        }
    }

    for (it = config_att.begin(); it != config_att.end(); it++){
        it->second = 0;
    }

    if(missingConfigAttribute){missingConfigAttribute = false; return false;}

    configData.seek(0);

    while(!configData.atEnd()){
        QString line = configData.readLine();
        if(!line.isEmpty() && line.at(0) != '#'){
            QStringList linesplit = line.split("=");
            std::string att, val;
            att = linesplit[0].toStdString();
            val = linesplit[1].toStdString();
            if (att=="name"){
                name = val;
            } else if(att=="cudacores"){
                cuda_cores = std::stoi(val);
            } else if(att=="smm"){
                num_sm = std::stoi(val);
            } else if(att=="warpsize"){
                warp_size = std::stoi(val);
            } else if(att=="concurrentwarps"){
                concurrent_warps = std::stoi(val);
            } else if(att=="cacheline"){
                line_size = std::stoi(val);
            } else if(att=="issectored"){
                is_sectored = std::stoi(val);
            } else if(att=="sectorsize"){
                sector_size = std::stoi(val);
            } else if(att=="waysizel1"){
                way_size_l1 = std::stoi(val);
            } else if(att=="waysizel2"){
                way_size_l2 = std::stoi(val);
            } else if(att=="l1enabled"){
                is_l1_enabled = std::stoi(val);
            } else if(att=="replacementpolicy"){
                replacement_policy = std::stoi(val);
            } else if(att=="writepolicy"){
                write_policy = std::stoi(val);
            } else if(att=="l1loadgranularity"){
                l1_load_granularity = std::stoi(val);
            } else if(att=="l2loadgranularity"){
                l2_load_granularity = std::stoi(val);
            } else if(att=="global"){
                global_size = std::stoll(val);
            } else if(att=="l2size"){
                l2_size = std::stoi(val);
            } else if(att=="l1size"){
                l1_size = std::stoi(val);
            } else {
                printf("Unknown Option\n");
            }
        }
    }
    computeConfig();
    printf("========CONFIG CHANGED========\n");
    printGlobals();


    return true;

}


bool readTraceDataFromQstream(QTextStream &traceData){

    traceData.seek(0);

}



