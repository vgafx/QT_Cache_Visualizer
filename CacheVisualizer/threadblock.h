#ifndef THREADBLOCK_H
#define THREADBLOCK_H
#include <string>
#include <list>
#include <map>
#include <vector>

#include "globals.h"

//declare


class threadBlock
{
    struct mem_acc_list{
        int l_tx;
        int l_ty;
        int warp_id;
        std::string ds_name;
        int operation;
        long long ds_idx;
        long long address;
        long long cycles;

        bool operator<(const mem_acc_list& a)const{
            return cycles < a.cycles;
        }
    };


public:
    threadBlock(int threadNum, int idx, int idy, int dim, int tx, int ty, int sm, int bmask);
    std::list<mem_acc_list> instruction_stream;
    std::list<mem_acc_list>::iterator it;
    //std::map<int, warp> warp_list;
    //std::map<int, warp>::iterator w_it;

    void addAccessToLocalList(int in_tx, int in_ty, int in_wid, std::string in_dsname, int in_oper, long long in_idx, long long in_address, long long in_cycles);
    void sortAccessEntries();
    void printInstructionStream();
    void clearAllData();

    int getNumWarps() const;
    void setNumWarps(int value);

    int getNumThreadsZ() const;
    void setNumThreadsZ(int value);

    int getNumThreadsY() const;
    void setNumThreadsY(int value);

    int getNumThreadsX() const;
    void setNumThreadsX(int value);

    int getNumThreads() const;
    void setNumThreads(int value);

    int getMappedToSM() const;
    void setMappedToSM(int value);

    int getBlockId() const;
    void setBlockId(int value);

    int getBlockIdX() const;
    void setBlockIdX(int value);

    int getBlockIdY() const;
    void setBlockIdY(int value);

    int getBlockIdZ() const;
    void setBlockIdZ(int value);

    int getBlockDim() const;
    void setBlockDim(int value);

    int getConcurrentWarps() const;
    void setConcurrentWarps(int value);

    bool getRetired() const;
    void setRetired(bool value);

    bool getRunning() const;
    void setRunning(bool value);

    int generateSectorIndex(long long address);
    int generateSetIndex(long long address);

    std::list<update_line_info> getUpdateInfo();

    bool isFinished();
    long long getNextCycleVal() const;
    void setNextCycleVal(long long value);

    void updateNextCycleCounter();

    int generateLineTag(long long address);
private:
    int blockDim;
    int blockId;
    int blockIdX;
    int blockIdY;
    int blockIdZ;
    int mappedToSM;
    int numThreads;
    int numThreadsX;
    int numThreadsY;
    int numThreadsZ;
    int numWarps;
    int bit_mask;
    long long nextAddress;
    long long nextCycleVal;
    int concurrentWarps;
    bool retired;
    bool running;


};

#endif // THREADBLOCK_H
