#include "threadblock.h"
#include "globals.h"

//define
threadBlock::threadBlock(int threadNum, int idx, int idy, int dim, int tx, int ty, int sm)
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
}

void threadBlock::configureDims(){

}

void threadBlock::formWarps(){

}


void threadBlock::addAccessToLocalList(int in_tx, int in_ty, int in_wid, std::string in_dsname, int in_oper, long long in_idx, long long in_address, long long in_cycles){
    mem_acc_list access_entry{in_tx, in_ty, in_wid, in_dsname, in_oper, in_idx, in_address, in_cycles};
    instruction_stream.push_back(access_entry);
}

void threadBlock::sortAccessEntries(){
    instruction_stream.sort();
}

void threadBlock::printInstructionStream(){
    printf("-----BLOCK----: %d", this->blockIdX);
    for (it = instruction_stream.begin(); it != instruction_stream.end(); it++) {
        printf("TX: %d TY: %d W: %d DS: %s OP: %d IDX: %llu ADD: %llu CYC: %llu\n", it->l_tx, it->l_ty, it->warp_id, it->ds_name.c_str(), it->operation, it->ds_idx, it->address, it->cycles);
    }
}

void threadBlock::clearAllData(){
    //this->running = false;
    instruction_stream.clear();
}

/*Setters and Getters*/
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


