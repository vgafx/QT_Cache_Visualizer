/*Functions for reading from input files*/

#include "fileio.h"
#include "globals.h"
#include "simulation.h"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <iomanip>
#include <map>


#define FILENAME "configuration"

//!!TODO: Add a L2 latency field in the config file and while reading the config setup

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
    config_att["autodelay"]=0;
    config_att["globallatency"]=0;
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
            } else if(attribute=="globallatency"){
                global_latency = std::stoi(value);
                config_att["globallatency"] = 1;
            } else if(attribute=="autodelay"){
                worker_delay = std::stoi(value);
                config_att["autodelay"] = 1;
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
        qDebug("Failed to open configuration file\n");
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
            } else if(att=="globallatency"){
                global_latency = std::stoi(val);
            } else if(att=="autodelay"){
                worker_delay = std::stoi(val);
            }else {
                printf("Unknown Option\n");
            }
        }
    }
    computeConfig();
    qDebug("========CONFIG CHANGED========\n");
    printGlobals();

    return true;
}


bool readTraceDataFromQstream(QTextStream &traceData, simulation *sim){

    traceData.seek(0);
    bool seenEqual = false;
    bool seenPlus = false;
    bool allowOnce = true;
    //bool encounteredProblem = false;
    //qDebug("Begun reading from QStream");

    while(!traceData.atEnd()){
        //qDebug("Started reading lines\n");
        QString line = traceData.readLine();
        if(!line.isEmpty() && line.at(0) != '#'){
            if (line.at(0) == '='){seenEqual = true;}
            if (line.at(0) == '+'){seenPlus = true;}
            //Get the launch configuration
            if (!seenEqual){
                QStringList linesplit = line.split(":");
                std::string att, val;
                att = linesplit[0].toStdString();
                val = linesplit[1].toStdString();
                printf("Read: att=%s,val=%s\n",att.c_str(),val.c_str());
                if(att == "numBlocks"){
                    sim->setNumBlocks(std::stoi(val));
                } else if (att == "numThreadsPerBlock"){
                    sim->setThreadsPerBlock(std::stoi(val));
                } else if (att == "blockDimensions"){
                    QStringList val_split = QString::fromStdString(val).split("-");
                    if (val_split.size() != 3){
                        traceData.flush();
                        qDebug("RF1\n");
                        return false;
                    } else {
                        sim->setBlocks_x(val_split[0].toInt());
                        sim->setBlocks_y(val_split[1].toInt());
                        sim->setBlocks_z(val_split[2].toInt());
                    }
                } else if (att == "threadDimensions"){
                    QStringList val_split = QString::fromStdString(val).split("-");
                    if (val_split.size() != 3){
                        traceData.flush();
                        qDebug("RF2\n");
                        return false;
                    } else {
                        sim->setThreads_x(val_split[0].toInt());
                        sim->setThreads_y(val_split[1].toInt());
                        sim->setThreads_z(val_split[2].toInt());
                    }
                }
            }//end launch config

            //Get the block scheduling info
            if (!seenPlus && seenEqual){
                if (line.isEmpty() || line.at(0) == '='){continue;}
                QStringList linesplit = line.trimmed().split(",");
                if (linesplit.size() != 3){
                    qDebug("RF3\n");
                    traceData.flush();
                    return false;
                } else {
                    QStringList entrysplit1 = linesplit[0].split(":");
                    QStringList entrysplit2 = linesplit[1].split(":");
                    QStringList entrysplit3 = linesplit[2].split(":");
                    QStringList block_vals = entrysplit1[1].split("-");
                    if (entrysplit1.size() != 2 && entrysplit2.size() != 2 && entrysplit2.size() != 2 && block_vals.size() != 3){
                        traceData.flush();
                        qDebug("RF4\n");
                        return false;
                    }
                    std::string attribute1, attribute2, attribute3;

                    attribute1 = entrysplit1[0].trimmed().toStdString();
                    attribute2 = entrysplit2[0].trimmed().toStdString();
                    attribute3 = entrysplit3[0].trimmed().toStdString();
                    block_vals = entrysplit1[1].split("-");

                    if (attribute1 == "B" && attribute2 == "SM" && attribute3 == "GT"){
                        //Replace with ds
                        int bx = block_vals[0].toInt();
                        int by = block_vals[1].toInt();
                        int bz = block_vals[2].toInt();
                        int smid = entrysplit2[1].toInt();
                        long long gtime = entrysplit3[1].toLongLong();
                        sim->addScheduleEntry(bx, by, smid, gtime);
                        //printf("bx:%d, by:%d, bz:%d, smid:%d, gtime:%llu\n",bx,by,bz,smid,gtime);
                    } else {
                        traceData.flush();
                        qDebug("RF5\n");
                        return false;
                    }
                }
            }//end block scheduling info

            //Finally get the trace input
            if (seenPlus && seenEqual){
                if(allowOnce){
                    //qDebug("Allow Once S\n");
                    sim->sortSchedulingEntries();
                    sim->configureDims();
                    sim->generateBlocks();
                    allowOnce = false;
                    //qDebug("Allow Once F\n");
                }
                if (line.isEmpty() || line.at(0) == '+'){continue;}
                QStringList linesplit = line.split(",");
                if (linesplit.size() != 7){
                    traceData.flush();
                    qDebug("RF6\n");
                    return false;
                } else {
                    //!!changed here
                    //int tx = linesplit[0].toInt();
                    //int ty = linesplit[1].toInt();
                    QStringList b_info = linesplit[0].split('-');
                    int blockx = b_info[0].toInt();
                    int blocky = b_info[1].toInt();
                    int wid = linesplit[1].toInt();
                    std::string dsn = linesplit[2].toStdString();
                    std::string s_op = linesplit[3].toStdString();
                    int operation;
                    if (s_op == "R"){
                        operation = READ;
                    } else if (s_op == "W"){
                        operation = WRITE;
                    }
                    long long ds_idx = linesplit[4].toLongLong();
                    long long address = linesplit[5].toLongLong();
                    long long cycles = linesplit[6].toLongLong();
                    //qDebug("Map\n");
                    sim->mapAccessToBlock(wid, wid, blockx, blocky, wid, dsn, operation, ds_idx, address, cycles);
                    //qDebug("Map F\n");
                }
            }
        }//end if empty/comment

    }//end of trace reading
    //qDebug("Finished reading from QStream");
    //sim->sortAllBlockAccesses();
    //sim->printBlockAccessLists();
    //sim->printBlocks();
    return true;

}



