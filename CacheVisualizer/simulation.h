#ifndef SIMULATION_H
#define SIMULATION_H
#include <list>
#include <map>
#include "threadblock.h"


class simulation
{

    struct schedule_entry{
        int block_id_x;
        int block_id_y;
        int sm_id;
        long long g_time;

        bool operator<(const schedule_entry& a)const{
            return g_time < a.g_time;
        }
    };

public:
    simulation();
    std::list<schedule_entry> block_schedule;
    std::list<schedule_entry>::iterator it;
    std::multimap<int, threadBlock> blocks;


    void cleanAll();
    void sortSchedulingEntries();
    void addScheduleEntry(int bx, int by, int sm, long long t);
    void mapAccessToBlock(int in_tx, int in_ty, int in_bx, int in_by, int in_wid, std::string in_dsname, int in_oper, long long in_idx, long long in_address, long long in_cycles);
    void configureDims();
    void generateBlocks();
    void printBlocks();
    void sortAllBlockAccesses();
    void printBlockAccessLists();

    int getNumBlocks() const;
    void setNumBlocks(int value);

    int getThreadsPerBlock() const;
    void setThreadsPerBlock(int value);

    bool getPauseFlag() const;
    void setPauseFlag(bool value);

    bool getReadyToStart() const;
    void setReadyToStart(bool value);

    bool getStopFlag() const;
    void setStopFlag(bool value);

    int getBlocks_z() const;
    void setBlocks_z(int value);

    int getBlocks_y() const;
    void setBlocks_y(int value);

    int getBlocks_x() const;
    void setBlocks_x(int value);

    int getThreads_x() const;
    void setThreads_x(int value);

    int getThreads_y() const;
    void setThreads_y(int value);

    int getThreads_z() const;
    void setThreads_z(int value);

    int getBlockDims() const;
    void setBlockDims(int value);

    int getThreadDims() const;
    void setThreadDims(int value);

private:
    int numBlocks;
    int blocks_x;
    int blocks_y;
    int blocks_z;
    int blockDims;
    int threadsPerBlock;
    int threads_x;
    int threads_y;
    int threads_z;
    int threadDims;
    bool pauseFlag;
    bool stopFlag;
    bool readyToStart;
};

#endif // SIMULATION_H
