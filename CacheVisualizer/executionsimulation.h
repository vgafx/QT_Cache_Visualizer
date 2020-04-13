/*** Execution Simulation
 * Class for representing the kernel execution simulation that the visualizer tool runs.
 * Acts as a parent object to all threadblock objects and offers functionality for controling
 * the simulation and passing on information to the thread blocks.
 ***/
#pragma once

#include <cmath>
#include <algorithm>
#include <map>
#include <QDebug>
#include <memory>

#include "block.h"
#include "settings.h"
#include "datadefinitions.h"

class ExecutionSimulation
{
private:
    int m_num_blocks;
    int m_blocks_x;
    int m_blocks_y;
    int m_blocks_z;
    int m_block_dims;
    int m_threads_per_block;
    int m_threads_x;
    int m_threads_y;
    int m_threads_z;
    int m_thread_dims;
    int m_current_active_blocks;
    int m_bit_mask;
    bool m_pause_flag;
    bool m_stop_flag;
    bool m_ready;
    bool m_is_running;


    /*Vector for holding block scheduling information*/
    std::vector<schedule_entry> m_block_schedule;
    /*Multimap for holding all the block objects required for the simulation, plus their unique keys*/
    std::multimap<std::tuple<int,int,int>, std::unique_ptr<Block>> m_blocks;

    /*Tuple & vector holding EMPTY values (defined in Settings). When no update info or block scheduling
      info is available from the corresponding data structures, the methods that return references, return
      a reference to the 'empty' object instead so that the cache can handle it accordingly.*/
    std::tuple<int,int,int> m_empty_key_ret;


    /*Keeps track of the Block that issued the most recent instruction*/
    std::tuple<int,int,int>& m_most_recent_block;

    void popVectorFront(std::vector<schedule_entry>& in_vec);

    /*Method that determines which of the thread blocks should execute the next instruction*/
    std::tuple<int, int, int>& findNextInstructionFromBlocks();

public:

    ExecutionSimulation();

    std::vector<update_cline_info<unsigned int, unsigned long long>> m_empty_update_ret;

    /*Checks if any block still has instructions to execute*/
    bool isSimulationComplete() const;

    /*Calls the coresponding Block function*/
    void sortAllBlockInstructions() const;

    void setStreamReserve() const;

    void sortSchedulingEntries();

    /*Prepares the first X Blocks for execution*/
    void prepareInitialBlocks();

    /*Creates the Blocks*/
    void generateBlocks();

    void updateBitMask();

    void configureDims();

    void cleanAll();

    /*Stores entries in m_block_schedule*/
    void addScheduleEntry(int bx, int by, int sm, long long t);

    /*Passes mem access entry to the corresponding Block*/
    void mapAccessToBlock(int in_bx, int in_by, int in_wid, char in_dsname, OperationType in_oper, unsigned int in_idx, unsigned long long in_address, unsigned long long in_time);

    /*Returns a reference to the update information generated by a Block*/
    std::vector<update_cline_info<unsigned int, unsigned long long>>& getUpdateInfoFromBlock();

    void popBlockUpdate() const;

    /*Debug Methods*/
    void printBlocks() const;
    void printSchedule() const;
    void printBlockAccessLists() const;
    void getMinMaxInstrTime() const;
    void getInstructionCounts() const;

    /*Getters & Setters*/
    int getNumBlocks() const;
    void setNumBlocks(int num_blocks);

    int getBlocksX() const;
    void setBlocksX(int blocks_x);

    int getBlocksY() const;
    void setBlocksY(int blocks_y);

    int getBlocksZ() const;
    void setBlocksZ(int blocks_z);

    int getBlockDims() const;
    void setBlockDims(int block_dims);

    int getThreadsPerBlock() const;
    void setThreadsPerBlock(int threads_per_block);

    int getThreadsX() const;
    void setThreadsX(int threads_x);

    int getThreadsY() const;
    void setThreadsY(int threads_y);

    int getThreadsZ() const;
    void setThreadsZ(int threads_z);

    int getThreadDims() const;
    void setThreadDims(int thread_dims);

    int getCurrentActiveBlocks() const;
    void setCurrentActiveBlocks(int current_active_blocks);

    int getBitMask() const;
    void setBitMask(int bit_mask);

    bool getPauseFlag() const;
    void setPauseFlag(bool pause_flag);

    bool getStopFlag() const;
    void setStopFlag(bool stop_flag);

    bool getReady() const;
    void setReady(bool ready);

    bool getIsRunning() const;
    void setIsRunning(bool is_running);
};