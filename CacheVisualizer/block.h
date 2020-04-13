/*** Block
 * Software abstaction for the concept of the threadblock.
 * Multiple instances of this object are created, based on the trace input.
 * Each block is constructed with a container that holds the memory requests
 * issued by it during the actual execution. The concept of the warp is not modeled
 * within the threadblock. Instead, this object comes with functions that re-arrange
 * memory requests as if they are issued by warps (based on the recorded warp id).
 ***/
#pragma once
#include <cmath>
#include <climits>
#include <algorithm>
#include <QDebug>
#include "settings.h"
#include "datadefinitions.h"

class Block
{
private:
    int m_block_dim;
    int m_block_id;
    int m_block_id_x;
    int m_block_id_y;
    int m_block_id_z;
    static int m_num_threads;
    static int m_num_warps;
    static unsigned long long m_bit_mask;
    static int m_tag_line_offset;
    int m_mapped_to_sm;
    unsigned long long m_next_address;
    unsigned long long m_next_time_stamp;
    bool m_is_retired;
    bool m_is_running;

    /*Holds the sorted memory access entries read by the trace file*/
    std::vector<mem_acc_entry<unsigned int, unsigned long long>> m_instruction_stream;

    /*Methods for transforming the address (hashing / mapping)*/
    inline int generateSectorIndex(unsigned long long address) const;

    inline int generateSetIndex(unsigned long long address) const;

    inline int generateClineTag(unsigned long long address) const;

    /*INternal method for setting the value for the next instruction*/
    inline void updateNextTimeCounter();

public:
    Block() = delete;

    Block(int num_threads, int bx, int by, int dims, int sm, int b_mask);

    /*Processes memory accesses are store here for consumption by the cache*/
    std::vector<std::vector<update_cline_info<unsigned int, unsigned long long>>> m_update_stream;

    bool isFinished() const;

    /*Empties the containers and attempts to reclaim the space*/
    void clearAllData();

    /*Sorts the instruction stream in reverse orded based on the comparator for mem_acc_entry structs(datadefinitions))*/
    void sortAccessEntries();

    void addAccessToLocalList(int wid, char ds_name, OperationType op, unsigned int idx, unsigned long long address, unsigned long long time);

    /*Empties the update stream*/
    void popUpdateInfo();

    /*A ref to this vector is returned when the instruction stream is found empty.*/
    std::vector<update_cline_info<unsigned int, unsigned long long>> m_empty_update;

    /*Returns a reference to the next update info*/
    std::vector<update_cline_info<unsigned int, unsigned long long>>& getUpdateInfoRef();

    void processAccessEntry();

    void setInstructionStreamReserveSize(size_t size);

    /*Debug Methods*/
    void printInstructionStream() const;
    unsigned long long getInstructionCount() const;
    unsigned long long getMinTime() const;
    unsigned long long getMaxTime() const;

    /*Default Setters & Getters*/
    int getBlockDim() const;
    void setBlockDim(int block_dim);

    int getBlockId() const;
    void setBlockId(int block_id);

    int getBlockIdX() const;
    void setBlockIdX(int block_id_x);

    int getBlockIdY() const;
    void setBlockIdY(int block_id_y);

    int getBlockIdZ() const;
    void setBlockIdZ(int block_id_z);

    static int getNumThreads();
    static void setNumThreads(int num_threads);

    static int getNumWarps();
    static void setNumWarps(int num_warps);

    static unsigned long long getBitMask();
    static void setBitMask(unsigned long long bit_mask);

    static int getTagLineOffset();
    static void setTagLineOffset(int tag_line_offset);

    int getMappedToSm() const;
    void setMappedToSm(int mapped_to_sm);

    unsigned long long getNextAddress() const;
    void setNextAddress(unsigned long long next_address);

    unsigned long long getNextTimeStamp() const;
    void setNextTimeStamp(unsigned long long next_time_stamp);

    bool getIsRetired() const;
    void setIsRetired(bool is_retired);

    bool getIsRunning() const;
    void setIsRunning(bool is_running);
};
