/*** Simulation Statistics
 * Class designed for keeping track of Simulation statistics such as the R / W hitrates,
 * total number of requests, etc...
 * Currently only a few events are being tracked. More events / statistics are considered
 * for future versions, including compulsory misses clasification, an upper theoretical limit
 * on the L2 hitrate, etc...
 ***/
#pragma once

#include <QString>

class SimulationStatistics
{
private:
    unsigned long long m_total_r_misses;
    unsigned long long m_total_w_misses;
    unsigned long long m_total_misses;
    unsigned long long m_total_r_hits;
    unsigned long long m_total_w_hits;
    unsigned long long m_total_hits;
    unsigned long long m_total_mem_req;
    double m_hitrate;
    double m_r_hitrate;
    double m_w_hitrate;
    QString m_output;

    /*Methods for preparing the output*/
    void estimateTotals();
    void estimatePercentage();

public:
    SimulationStatistics();

    void reset();

    /*Methods for incrementing statistics*/
    void incMemRequests();
    void incReadMiss();
    void incWriteMiss();
    void incReadHit();
    void incWriteHit();

    QString getOutputString();

    /*Methods for adding a variable amount of events*/
    void recordReadMisses(int r_misses);
    void recordWriteMisses(int w_misses);
    void recordReadHits(int r_hits);
    void recordWriteHits(int w_hits);
    void recordMemoryRequests(int m_req);
};

