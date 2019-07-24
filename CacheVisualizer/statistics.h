#ifndef STATISTICS_H
#define STATISTICS_H

#include <QString>



class statistics
{
public:
    statistics();


    int getR_misses_total() const;
    void setR_misses_total(int value);

    int getW_misses_total() const;
    void setW_misses_total(int value);

    int getTotal_misses() const;
    void setTotal_misses(int value);

    int getR_full_hits() const;
    void setR_full_hits(int value);

    int getR_partial_hits() const;
    void setR_partial_hits(int value);

    int getW_full_hits() const;
    void setW_full_hits(int value);

    int getW_partial_hits() const;
    void setW_partial_hits(int value);

    int getTotal_partial_hits() const;
    void setTotal_partial_hits(int value);

    int getTotal_full_hits() const;
    void setTotal_full_hits(int value);

    int getTotal_mem_requests() const;
    void setTotal_mem_requests(int value);

    void incMemRequests();
    void recordReadMiss();
    void recordReadFullHit();
    void recordReadPartialHit();
    void recordWriteMiss();
    void recordWritePartialHit();
    void recordWriteFullHit();
    void estimateTotals();
    QString getStatisticsOutput();
private:
    int r_misses_total;
    int w_misses_total;
    int total_misses;
    int r_full_hits;
    int r_partial_hits;
    int w_full_hits;
    int w_partial_hits;
    int total_partial_hits;
    int total_full_hits;
    int total_mem_requests;

    QString output;


};

#endif // STATISTICS_H
