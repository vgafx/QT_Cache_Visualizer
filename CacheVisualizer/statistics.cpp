/*Simple class for recording simulation statistics(cache hits/misses, etc...)*/
#include "statistics.h"

statistics::statistics()
{
    this->total_misses = 0;
    this->total_full_hits = 0;
    this->total_mem_requests = 0;
    this->total_partial_hits = 0;
    this->r_full_hits = 0;
    this->r_misses_total = 0;
    this->r_partial_hits = 0;
    this->w_full_hits = 0;
    this->w_misses_total = 0;
    this->w_partial_hits = 0;
    this->hitrate = 0.0;
    this->output = "";

}

QString statistics::getStatisticsOutput(){
    estimateTotals();
    estimatePercentage();
    QString results = QString("Simulation Results\n---------------------------\nTotal Memory Requests Issued(Warp-wide): %1\nTotal Full Hits: %2\nTotal Partial Hits: %3"
                              "\nTotal Misses: %4\nCache Hit-Rate: %5%\n\nRead-Write Statistics\n---------------------------\n"
                              "Read Full Hits: %6\nRead Partial Hits: %7\nRead Misses: %8\nWrite Full Hits: %9\nWrite Partial Hits: %10\nWrite Misses: %11")
            .arg(this->total_mem_requests).arg(this->total_full_hits).arg(this->total_partial_hits).arg(this->total_misses).arg(this->hitrate).arg(this->r_full_hits).arg(this->r_partial_hits)
            .arg(this->r_misses_total).arg(this->w_full_hits).arg(this->w_partial_hits).arg(this->w_misses_total);

    return results;
}

void statistics::reset()
{
    this->total_misses = 0;
    this->total_full_hits = 0;
    this->total_mem_requests = 0;
    this->total_partial_hits = 0;
    this->r_full_hits = 0;
    this->r_misses_total = 0;
    this->r_partial_hits = 0;
    this->w_full_hits = 0;
    this->w_misses_total = 0;
    this->w_partial_hits = 0;
    this->hitrate = 0.0;
    this->output = "";
}

void statistics::estimateTotals(){
    this->total_misses = this->w_misses_total + this->r_misses_total;
    this->total_full_hits = this->w_full_hits + this->r_full_hits;
    this->total_partial_hits = this->w_partial_hits + this->r_partial_hits;
}

void statistics::estimatePercentage()
{
    qDebug("Estimating tfh: %d, tph:%d, tmr:%d\n",this->total_full_hits, this->total_partial_hits, this->total_mem_requests);
    this->hitrate = (double(this->total_full_hits) + double(this->total_partial_hits)) / double(this->total_mem_requests);
    this->hitrate *= 100;
    qDebug("HR:%.2f\n", this->hitrate);
}

void statistics::recordWritePartialHit(){
    this->w_partial_hits++;
}

void statistics::recordWriteFullHit(){
    this->w_full_hits++;
}

void statistics::recordWriteMiss(){
    this->w_misses_total++;
}

void statistics::recordReadPartialHit(){
    this->r_partial_hits++;
}

void statistics::recordReadFullHit(){
    this->r_full_hits++;
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

int statistics::getR_full_hits() const
{
    return r_full_hits;
}

void statistics::setR_full_hits(int value)
{
    r_full_hits = value;
}

int statistics::getR_partial_hits() const
{
    return r_partial_hits;
}

void statistics::setR_partial_hits(int value)
{
    r_partial_hits = value;
}

int statistics::getW_full_hits() const
{
    return w_full_hits;
}

void statistics::setW_full_hits(int value)
{
    w_full_hits = value;
}

int statistics::getW_partial_hits() const
{
    return w_partial_hits;
}

void statistics::setW_partial_hits(int value)
{
    w_partial_hits = value;
}

int statistics::getTotal_partial_hits() const
{
    return total_partial_hits;
}

void statistics::setTotal_partial_hits(int value)
{
    total_partial_hits = value;
}

int statistics::getTotal_full_hits() const
{
    return total_full_hits;
}

void statistics::setTotal_full_hits(int value)
{
    total_full_hits = value;
}

int statistics::getTotal_mem_requests() const
{
    return total_mem_requests;
}

void statistics::setTotal_mem_requests(int value)
{
    total_mem_requests = value;
}
