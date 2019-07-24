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
    this->w_partial_hits =0;
    this->output = "";

}

QString statistics::getStatisticsOutput(){

    return this->output;
}


void statistics::estimateTotals(){
    this->total_misses = this->w_misses_total + this->r_misses_total;
    this->total_full_hits = this->w_full_hits + this->r_full_hits;
    this->total_partial_hits = this->w_partial_hits + this->r_partial_hits;
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
