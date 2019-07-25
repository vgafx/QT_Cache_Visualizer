/*Software abstaction for the concept of the threadblock.
  Multiple instances of this object are created, based on the trace input.
  Each threadblock is constructed with a container that holds the memory requests
  issued by it during the actual execution. The concept of the warp is not modeled
  within the threadblock. Instead, this object comes with functions that re-arrange
  memory requests as if they are issued by warps.*/

#include "threadblock.h"
#include "globals.h"

#include <climits>
#include <algorithm>
#include <cmath>


/*Constructor*/
threadBlock::threadBlock(int threadNum, int idx, int idy, int dim, int tx, int ty, int sm, int bmask)
{
    this->numThreads = threadNum;
    this->numWarps = threadNum / warp_size;
    this->blockDim = dim;
    this->blockIdX = idx;
    this->blockIdY = idy;
    this->numThreadsX = tx;
    this->numThreadsY = ty;
    this->mappedToSM = sm;
    this->concurrentWarps = concurrent_warps;
    this->retired = false;
    this->running = false;
    this->nextCycleVal =0;
    this->bit_mask = bmask;
}


/*Returns a list of mem accesses, constructed with the visual update in mind.
 *At most 'warp size' entries are returned per call(Requires sorted instruction stream)*/
std::list<update_line_info> threadBlock::getUpdateInfo(){
    std::list<update_line_info> ret_update_entries;
    //qDebug("getUpdateInfo() - Entered\n");
    if(!instruction_stream.empty()){
        //qDebug("getUpdateInfo() - Stream Not Empty\n");
        int operation = instruction_stream.front().operation;
        std::string name = instruction_stream.front().ds_name;
        int wid = instruction_stream.front().warp_id;
        long long cycles = instruction_stream.front().cycles;
        int entries = std::min(warp_size, int(instruction_stream.size()));

        std::vector<distinct_entries> uniq_entries;
        //qDebug("getUpdateInfo() - Looping for %d entries\n",entries);
        for (int i = 0; i < entries; i++) {
            if(wid == instruction_stream.front().warp_id && cycles == instruction_stream.front().cycles){
                long long cur_address = instruction_stream.front().address;
                long long cur_ds_idx = instruction_stream.front().ds_idx;
                int cur_sector_id = this->generateSectorIndex(cur_address);
                int cur_set_id = this->generateSetIndex(cur_address);
                int cur_line_tag = this->generateLineTag(cur_address);
                bool s0 = false, s1 = false, s2 = false, s3 = false;
                switch(cur_sector_id){
                    case 0: s0 = true; break;
                    case 1: s1 = true; break;
                    case 2: s2 = true; break;
                    case 3: s3 = true; break;
                }
                if(uniq_entries.empty()){//Record the first update entry
                    //qDebug("getUpdateInfo() - First Entry\n");
                    distinct_entries first_entry = {cur_set_id, cur_line_tag, cur_address, cur_address, cur_ds_idx, cur_ds_idx, s0, s1, s2, s3};
                    uniq_entries.push_back(first_entry);
                } else {//Check whether we have already seen this set idx
                    bool entry_exists = false;
                    for (size_t s = 0; s < uniq_entries.size(); s++) {
                        if (cur_set_id == uniq_entries.at(s).set_id && cur_line_tag == uniq_entries.at(s).tag){ //If yes, Update min, max, and filled vectors
                            //qDebug("getUpdateInfo() - Entry already exists\n");
                            entry_exists = true;
                            if (cur_address < uniq_entries.at(s).min_add){uniq_entries.at(s).min_add = cur_address;}
                            if (cur_address > uniq_entries.at(s).max_add){uniq_entries.at(s).max_add = cur_address;}
                            if (cur_ds_idx < uniq_entries.at(s).min_idx){uniq_entries.at(s).min_idx = cur_ds_idx;}
                            if (cur_ds_idx > uniq_entries.at(s).max_idx){uniq_entries.at(s).max_idx = cur_ds_idx;}
                            if (!uniq_entries.at(s).sec0 && s0){uniq_entries.at(s).sec0 = s0;}
                            if (!uniq_entries.at(s).sec1 && s1){uniq_entries.at(s).sec1 = s1;}
                            if (!uniq_entries.at(s).sec2 && s2){uniq_entries.at(s).sec2 = s2;}
                            if (!uniq_entries.at(s).sec3 && s3){uniq_entries.at(s).sec3 = s3;}
                        }
                    }
                    if(!entry_exists){//Encountered a new set index and tag, so create a new entry
                        distinct_entries n_entry = {cur_set_id, cur_line_tag,cur_address, cur_address, cur_ds_idx, cur_ds_idx, s0, s1, s2, s3};
                        uniq_entries.push_back(n_entry);
                    }
                }
                //qDebug("getUpdateInfo() - poping front\n");
                instruction_stream.pop_front();
            }
            //Instruction does not match the first picked instruction's wid & cycle counter
        }
        qDebug("getUpdateInfo() - Creating update_line_info\n");
        for (size_t o = 0; o < uniq_entries.size(); o++) {
            update_line_info line_info = {uniq_entries.at(o).set_id, uniq_entries.at(0).tag, operation, name, uniq_entries.at(o).min_add, uniq_entries.at(o).max_add,
                                          uniq_entries.at(o).min_idx, uniq_entries.at(o).max_idx, cycles, uniq_entries.at(o).sec0,
                                          uniq_entries.at(o).sec1, uniq_entries.at(o).sec2, uniq_entries.at(o).sec3};
            qDebug("getUpdateInfo()-E:%lu- SID: %d, TAG: %d , OP: %d, NAME: %s, MINAD: %llu, MAXAD: %llu, MINIDX: %llu, MAXIDX: %llu, CYC: %llu, S0: %d,S1: %d,S2: %d,S3: %d\n",
                   o,line_info.set_idx, line_info.tag, line_info.oper, line_info.name.c_str(),line_info.add_low, line_info.add_high, line_info.idx_low, line_info.idx_high, line_info.cycles,
                   line_info.s0, line_info.s1, line_info.s2, line_info.s3);
            ret_update_entries.push_back(line_info);
        }
    }//instruction_stream.empty

    this->updateNextCycleCounter();
    return ret_update_entries;
}


void threadBlock::updateNextCycleCounter(){
    if (!instruction_stream.empty()){ //!!handle if not empty
        this->nextCycleVal = instruction_stream.front().cycles;
    } else {
        this->running = false;
        this->retired = true;
        this->nextCycleVal = LLONG_MAX;
    }
}


/*Cache Mapping functions (3)*/
int threadBlock::generateSetIndex(long long address){
    long long temp_address = address >> BLOCK_OFFSET_BITS;
    temp_address &= this->bit_mask;
    return int(temp_address);
}

int threadBlock::generateSectorIndex(long long address){
    long long tmp_address = address & (line_size-1);
    return int(tmp_address /= sector_size);
}

int threadBlock::generateLineTag(long long address){
    //!!Needs work
    long long temp_address = address >> BLOCK_OFFSET_BITS;
    return temp_address >>= int(log2(num_sets_l2));
}


/*Misc*/
bool threadBlock::isFinished(){
    return (instruction_stream.empty()) ? true : false;
}

void threadBlock::clearAllData(){
    instruction_stream.clear();
}


/*Input reading & setup*/
void threadBlock::addAccessToLocalList(int in_tx, int in_ty, int in_wid, std::string in_dsname, int in_oper, long long in_idx, long long in_address, long long in_cycles){
    mem_acc_list access_entry{in_tx, in_ty, in_wid, in_dsname, in_oper, in_idx, in_address, in_cycles};
    instruction_stream.push_back(access_entry);
}

void threadBlock::sortAccessEntries(){
    instruction_stream.sort();
    this->updateNextCycleCounter();
}

/*Debug function*/
void threadBlock::printInstructionStream(){
    printf("-----BLOCK----: %d", this->blockIdX);
    for (it = instruction_stream.begin(); it != instruction_stream.end(); it++) {
        printf("TX: %d TY: %d W: %d DS: %s OP: %d IDX: %llu ADD: %llu CYC: %llu\n", it->l_tx, it->l_ty, it->warp_id, it->ds_name.c_str(), it->operation, it->ds_idx, it->address, it->cycles);
    }
}

/*Setters and Getters*/
long long threadBlock::getNextCycleVal() const
{
    return nextCycleVal;
}

void threadBlock::setNextCycleVal(long long value)
{
    nextCycleVal = value;
}

int threadBlock::getNumWarps() const
{
    return numWarps;
}

void threadBlock::setNumWarps(int value)
{
    numWarps = value;
}

int threadBlock::getNumThreadsZ() const
{
    return numThreadsZ;
}

void threadBlock::setNumThreadsZ(int value)
{
    numThreadsZ = value;
}

int threadBlock::getNumThreadsY() const
{
    return numThreadsY;
}

void threadBlock::setNumThreadsY(int value)
{
    numThreadsY = value;
}

int threadBlock::getNumThreadsX() const
{
    return numThreadsX;
}

void threadBlock::setNumThreadsX(int value)
{
    numThreadsX = value;
}

int threadBlock::getNumThreads() const
{
    return numThreads;
}

void threadBlock::setNumThreads(int value)
{
    numThreads = value;
}

int threadBlock::getMappedToSM() const
{
    return mappedToSM;
}

void threadBlock::setMappedToSM(int value)
{
    mappedToSM = value;
}

int threadBlock::getBlockId() const
{
    return blockId;
}

void threadBlock::setBlockId(int value)
{
    blockId = value;
}

int threadBlock::getBlockIdX() const
{
    return blockIdX;
}

void threadBlock::setBlockIdX(int value)
{
    blockIdX = value;
}

int threadBlock::getBlockIdY() const
{
    return blockIdY;
}

void threadBlock::setBlockIdY(int value)
{
    blockIdY = value;
}

int threadBlock::getBlockIdZ() const
{
    return blockIdZ;
}

void threadBlock::setBlockIdZ(int value)
{
    blockIdZ = value;
}

int threadBlock::getBlockDim() const
{
    return blockDim;
}

void threadBlock::setBlockDim(int value)
{
    blockDim = value;
}

int threadBlock::getConcurrentWarps() const
{
    return concurrentWarps;
}

void threadBlock::setConcurrentWarps(int value)
{
    concurrentWarps = value;
}

bool threadBlock::getRetired() const
{
    return retired;
}

void threadBlock::setRetired(bool value)
{
    retired = value;
}

bool threadBlock::getRunning() const
{
    return running;
}

void threadBlock::setRunning(bool value)
{
    running = value;
}


