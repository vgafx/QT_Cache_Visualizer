#include "fileio.h"
#include "globals.h"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <iomanip>

#define FILENAME "configuration"

using namespace std;

fileIO::fileIO()
{
}

void readConfig(){
    std::ifstream configFile;
    configFile.open(FILENAME);

    if ( configFile.is_open()){
        std::string line;
        while (getline(configFile,line)){
            std::istringstream tokenizer(line);
            std::string attribute, value;
            std::getline(tokenizer, attribute, '=');
            std::getline(tokenizer, value);

            printf("Tokenizer: att=%s,val=%sEND\n", attribute.c_str(), value.c_str());

            if (attribute=="name"){
                name = value;
            } else if(attribute=="cudacores"){
                cuda_cores = std::stoi(value);
            } else if(attribute=="smm"){
                num_sm = std::stoi(value);
            } else if(attribute=="warpsize"){
                warp_size = std::stoi(value);
            } else if(attribute=="concurrentwarps"){
                concurrent_warps = std::stoi(value);
            } else if(attribute=="cacheline"){
                line_size = std::stoi(value);
            } else if(attribute=="issectored"){
                is_sectored = std::stoi(value);
            } else if(attribute=="sectorsize"){
                sector_size = std::stoi(value);
            } else if(attribute=="waysizel1"){
                way_size_l1 = std::stoi(value);
            } else if(attribute=="waysizel2"){
                way_size_l2 = std::stoi(value);
            } else if(attribute=="l1enabled"){
                is_l1_enabled = std::stoi(value);
            } else if(attribute=="replacementpolicy"){
                replacement_policy = std::stoi(value);
            } else if(attribute=="writepolicy"){
                write_policy = std::stoi(value);
            } else if(attribute=="l1loadgranularity"){
                l1_load_granularity = std::stoi(value);
            } else if(attribute=="l2loadgranularity"){
                l2_load_granularity = std::stoi(value);
            } else if(attribute=="global"){
                global_size = std::stoll(value);
            } else if(attribute=="l2size"){
                l2_size = std::stoi(value);
            } else if(attribute=="l1size"){
                l1_size = std::stoi(value);
            } else {
                printf("Unknown Option\n");
            }
        }
        configFile.close();
    } else {
        //Consider using a visual exception in this case
        printf("Failed to open configuration file\n");
    }
    //return 0;
}





