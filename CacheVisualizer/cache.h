/*** Cache
 *  Class designed to be used as a parent object for the Cacheline objects.
 *  Mainly responsible for keeping track of the Cachelines, proccessing
 *  update_cline_info structs and implementing the Cache ruleset (Eviction
 *  policy, Cache lookup, etc...)
 ***/
#pragma once

#include <map>
#include <random>
#include "cacheline.h"
#include "settings.h"
#include "simulationstatistics.h"
#include "datadefinitions.h"


class Cache
{
private:
    std::string m_name;
    int m_size_bytes;
    int m_way_size;
    int m_num_sets;
    bool m_is_sectored;
    unsigned long long m_mem_latency;
    ReplacementPolicy m_rep_policy;
    MemWritePolicy m_w_policy;
    std::random_device m_rd;
    SimulationStatistics& m_stat;



    /*Multmap to keep track of the cacheline objects. The set index is used as a key*/
    std::multimap<int, Cacheline*> m_idx_map;

    inline int getRandomNumber();

    /*Wrapper Method to process update information in steps*/
    inline void processRead(std::vector<update_cline_info<unsigned int, unsigned long long> > &up_info);

    inline void readLookup(bool& data_found, bool& empty_cline, bool& r_once, int& oldest_age, int& oldest_tag, int& hit_tag,
                           std::pair<std::multimap<int,Cacheline*>::iterator, std::multimap<int,Cacheline*>::iterator>& ret,
                           std::vector<update_cline_info<unsigned int, unsigned long long>>::iterator &up_it);

    inline void readLoadOnEmpty(bool& data_found, bool& empty_cline, bool& r_once,
                                std::pair<std::multimap<int,Cacheline*>::iterator, std::multimap<int,Cacheline*>::iterator>& ret,
                                std::vector<update_cline_info<unsigned int, unsigned long long>>::iterator &up_it);

    inline void readLoadEviction(bool& data_found, bool& empty_cline, int& oldest_tag,
                                 std::pair<std::multimap<int,Cacheline*>::iterator, std::multimap<int,Cacheline*>::iterator>& ret,
                                 std::vector<update_cline_info<unsigned int, unsigned long long>>::iterator &up_it);

    /*Wrapper Method to process update information in steps*/
    inline void processWrite(std::vector<update_cline_info<unsigned int, unsigned long long> > &up_info);

    inline void writeLookup(bool& data_found, bool& empty_cline, bool& w_once, int& oldest_age, int& oldest_tag, int& hit_tag,
                           std::pair<std::multimap<int,Cacheline*>::iterator, std::multimap<int,Cacheline*>::iterator>& ret,
                           std::vector<update_cline_info<unsigned int, unsigned long long>>::iterator &up_it);

    inline void writeLoadOnEmpty(bool& data_found, bool& empty_cline, bool& w_once,
                                std::pair<std::multimap<int,Cacheline*>::iterator, std::multimap<int,Cacheline*>::iterator>& ret,
                                std::vector<update_cline_info<unsigned int, unsigned long long>>::iterator &up_it);

    inline void writeLoadEviction(bool& data_found, bool& empty_cline, int& oldest_tag,
                                 std::pair<std::multimap<int,Cacheline*>::iterator, std::multimap<int,Cacheline*>::iterator>& ret,
                                 std::vector<update_cline_info<unsigned int, unsigned long long>>::iterator &up_it);

public:
    Cache() = delete;

    Cache(const std::string& in_name, bool in_sec, ReplacementPolicy in_rep, MemWritePolicy in_mem_w, SimulationStatistics* stat);

    Cache(const std::string& in_name, int in_size, int in_way, int in_sets, bool in_sec, ReplacementPolicy in_rep, MemWritePolicy in_mem_w, SimulationStatistics* stat);

    void processUpdate(std::vector<update_cline_info<unsigned int, unsigned long long>>& up_info);

    void addCacheline(int set_index, Cacheline* cline);

    void resetState();

    void cleanAll();

    void resetAllMembers(const std::string& in_name, int in_size, int in_way, int in_sets, bool in_sec, ReplacementPolicy in_rep, MemWritePolicy in_mem_w, SimulationStatistics* stat);

    /*Getters & Setters*/
    std::string getName() const;
    void setName(const std::string &name);

    int getSizeBytes() const;
    void setSizeBytes(int size_bytes);

    int getWaySize() const;
    void setWaySize(int way_size);

    int getNumSets() const;
    void setNumSets(int num_sets);

    bool getIsSectored() const;
    void setIsSectored(bool is_sectored);

    ReplacementPolicy getRepPolicy() const;
    void setRepPolicy(const ReplacementPolicy &rep_policy);

    MemWritePolicy getWPolicy() const;
    void setWPolicy(const MemWritePolicy &w_policy);

    unsigned long long getMemLatency() const;
    void setMemLatency(unsigned long long mem_latency);
};

