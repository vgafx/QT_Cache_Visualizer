#include "simulationstatistics.h"
/*Default ctor. Calls reset function to init with 0*/
SimulationStatistics::SimulationStatistics()
{
    this->reset();
}

void SimulationStatistics::reset()
{
    m_total_r_misses = 0;
    m_total_w_misses = 0;
    m_total_misses = 0;
    m_total_r_hits = 0;
    m_total_w_hits = 0;
    m_total_hits = 0;
    m_total_mem_req = 0;
    m_hitrate = 0;
    m_r_hitrate = 0;
    m_w_hitrate = 0;
    m_output = "";
}

/*Increment by 1 methods*/
void SimulationStatistics::incMemRequests()
{
    m_total_mem_req++;
}

void SimulationStatistics::incReadMiss()
{
    m_total_r_misses++;
}

void SimulationStatistics::incWriteMiss()
{
    m_total_w_misses++;
}

void SimulationStatistics::incReadHit()
{
    m_total_r_hits++;
}

void SimulationStatistics::incWriteHit()
{
    m_total_w_hits++;
}

/*Increment by specified value*/
void SimulationStatistics::recordReadMisses(int r_misses)
{
    m_total_r_misses += static_cast<unsigned long long>(r_misses);
}

void SimulationStatistics::recordWriteMisses(int w_misses)
{
    m_total_w_misses += static_cast<unsigned long long>(w_misses);
}

void SimulationStatistics::recordReadHits(int r_hits)
{
    m_total_r_hits += static_cast<unsigned long long>(r_hits);
}

void SimulationStatistics::recordWriteHits(int w_hits)
{
    m_total_w_hits += static_cast<unsigned long long>(w_hits);
}

void SimulationStatistics::recordMemoryRequests(int m_req)
{
    m_total_mem_req += static_cast<unsigned long long>(m_req);
}

/*Private methods for calculating output*/
void SimulationStatistics::estimateTotals()
{
    m_total_misses = m_total_r_misses + m_total_w_misses;
    m_total_hits = m_total_r_hits + m_total_w_hits;
}

void SimulationStatistics::estimatePercentage()
{
    m_hitrate = static_cast<double>(m_total_hits) / static_cast<double>(m_total_mem_req);
    m_hitrate *= 100;
    unsigned long long total_writes = m_total_w_hits + m_total_w_misses;
    unsigned long long total_reads = m_total_r_hits + m_total_r_misses;
    m_r_hitrate = static_cast<double>(m_total_r_hits) / static_cast<double>(total_reads);
    m_w_hitrate = static_cast<double>(m_total_w_hits) / static_cast<double>(total_writes);
    m_r_hitrate *= 100;
    m_w_hitrate *= 100;
}

/*Produces the output String, ready for writting to a file*/
QString SimulationStatistics::getOutputString()
{
    this->estimateTotals();
    this->estimatePercentage();
    QString results = QString("Simulation Results\n---------------------------\nTotal Memory Requests Issued: %1\nTotal Hits: %2\n"
                              "\nTotal Misses: %3\nCache Hit-Rate: %4%\n\nRead-Write Statistics\n---------------------------\n"
                              "Read Hits: %5\nRead Misses: %6\nWrite Hits: %7\nWrite Misses: %8\nRead Hitrate: %9\nWrite Hitrate: %10")
            .arg(m_total_mem_req).arg(m_total_hits).arg(m_total_misses).arg(m_hitrate).arg(m_total_r_hits).arg(m_total_r_misses)
            .arg(m_total_w_hits).arg(m_total_w_misses).arg(m_r_hitrate).arg(m_w_hitrate);

    return results;
}



