#ifndef STATISTICS_H
#define STATISTICS_H

#include <QString>

class statistics
{
public:
    statistics();

    /*Default setters and getters*/
    int getR_misses_total() const;
    void setR_misses_total(int value);

    int getW_misses_total() const;
    void setW_misses_total(int value);

    int getTotal_misses() const;
    void setTotal_misses(int value);

    int getTotal_mem_requests() const;
    void setTotal_mem_requests(int value);

    /*Custom Functions*/
    void incMemRequests();
    void recordReadMiss();
    void recordReadFullHit();
    void recordReadPartialHit();
    void recordWriteMiss();
    void recordWritePartialHit();
    void recordWriteFullHit();
    void estimateTotals();
    void estimatePercentage();
    QString getStatisticsOutput();
    void reset();

    void recordReadMisses(int r_misses);
    void recordWriteMisses(int w_misses);
    void recordReadHits(int r_hits);
    void recordWriteHits(int w_hits);
    void recordMemoryRequests(int m_req);
private:
    int r_misses_total;
    int w_misses_total;
    int total_misses;
    int r_hits_total;
    int w_hits_total;
    int total_hits;
    int total_mem_requests;
    double hitrate;
    double w_hitrate;
    double r_hitrate;
    QString output;

};

#endif // STATISTICS_H
