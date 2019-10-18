/*Simple class for recording simulation statistics(cache hits/misses, etc...)*/
#include "statistics.h"

statistics::statistics()
{
    this->total_misses = 0;
    this->total_hits = 0;
    this->total_mem_requests = 0;
    this->r_misses_total = 0;
    this->r_hits_total = 0;
    this->w_misses_total = 0;
    this->w_hits_total = 0;
    this->hitrate = 0.0;
    this->output = "";
}

QString statistics::getStatisticsOutput(){
    estimateTotals();
    estimatePercentage();
    QString results = QString("Simulation Results\n---------------------------\nTotal Memory Requests Issued: %1\nTotal Hits: %2\n"
                              "\nTotal Misses: %3\nCache Hit-Rate: %4%\n\nRead-Write Statistics\n---------------------------\n"
                              "Read Hits: %5\nRead Misses: %6\nWrite Hits: %7\nWrite Misses: %8")
            .arg(this->total_mem_requests).arg(this->total_hits).arg(this->total_misses).arg(this->hitrate).arg(this->r_hits_total).arg(this->r_misses_total)
            .arg(this->w_hits_total).arg(this->w_misses_total);

    return results;
}

void statistics::reset()
{
    this->total_misses = 0;
    this->total_hits = 0;
    this->total_mem_requests = 0;
    this->r_misses_total = 0;
    this->r_hits_total = 0;
    this->w_misses_total = 0;
    this->w_hits_total = 0;
    this->hitrate = 0.0;
    this->output = "";
}

void statistics::estimateTotals(){
    this->total_misses = this->w_misses_total + this->r_misses_total;
    this->total_hits = this->r_hits_total + this->w_hits_total;
}

void statistics::recordReadMisses(int r_misses){
    this->r_misses_total+=r_misses;
}

void statistics::recordWriteMisses(int w_misses){
    this->w_misses_total+=w_misses;
}

void statistics::recordReadHits(int r_hits){
    this->r_hits_total+=r_hits;
}

void statistics::recordWriteHits(int w_hits){
    this->w_hits_total+=w_hits;
}

void statistics::recordMemoryRequests(int m_req){
    this->total_mem_requests+=m_req;
}

void statistics::estimatePercentage()
{
    this->hitrate = double(this->total_hits) / double(this->total_mem_requests);
    this->hitrate *= 100;
}

void statistics::recordWriteMiss(){
    this->w_misses_total++;
}

void statistics::recordReadMiss(){
    this->r_misses_total++;
}

void statistics::incMemRequests(){
    this->total_mem_requests++;
}

/*Default Setters & Getters*/
int statistics::getR_misses_total() const
{
    return r_misses_total;
}

void statistics::setR_misses_total(int value)
{
    r_misses_total = value;
}

int statistics::getW_misses_total() const
{
    return w_misses_total;
}

void statistics::setW_misses_total(int value)
{
    w_misses_total = value;
}

int statistics::getTotal_misses() const
{
    return total_misses;
}

void statistics::setTotal_misses(int value)
{
    total_misses = value;
}

int statistics::getTotal_mem_requests() const
{
    return total_mem_requests;
}

void statistics::setTotal_mem_requests(int value)
{
    total_mem_requests = value;
}
