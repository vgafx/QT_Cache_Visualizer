#include "simulation.h"
#include "threadblock.h"
#include <algorithm>
#include <string>


simulation::simulation()
{
    this->stopFlag = false;
    this->pauseFlag = false;
    this->readyToStart = false;
    this->numBlocks = 0;
    this->threadsPerBlock = 0;
    this->threads_x = 0;
    this->threads_y = 0;
    this->threads_z = 0;
    this->blocks_x = 0;
    this->blocks_y = 0;
    this->blocks_z = 0;
}


void simulation::cleanAll(){
    block_schedule.clear();
    for (std::multimap<int,threadBlock>::iterator it=blocks.begin(); it!=blocks.end(); ++it){
        it->second.clearAllData();
    }

    blocks.clear();
    this->stopFlag = false;
    this->pauseFlag = false;
    this->readyToStart = false;
    this->numBlocks = 0;
    this->threadsPerBlock = 0;
    this->threads_x = 0;
    this->threads_y = 0;
    this->threads_z = 0;
    this->blocks_x = 0;
    this->blocks_y = 0;
    this->blocks_z = 0;
}


void simulation::addScheduleEntry(int bx, int by, int sm, long long t){
    schedule_entry entry{bx, by, sm, t};
    block_schedule.push_back(entry);
}

void simulation::sortSchedulingEntries(){
    //std::sort(block_schedule.begin(), block_schedule.end());
    block_schedule.sort();
    //for (it=block_schedule.begin(); it != block_schedule.end(); it++){
    //    printf("X:%d Y:%d SM:%d T:%llu\n", it->block_id_x, it->block_id_y, it->sm_id, it->g_time);
    //}
}

void simulation::configureDims(){
    int total_t_dims = 3, total_b_dims = 3;
    if (this->threads_z == 0){total_t_dims--;}
    if (this->threads_y == 0){total_t_dims--;}
    if (this->blocks_z == 0){total_b_dims--;}
    if (this->blocks_y == 0){total_b_dims--;}
    this->threadDims = total_t_dims;
    this->blockDims = total_b_dims;
}

void simulation::generateBlocks(){
    for (it=block_schedule.begin(); it != block_schedule.end(); it++) {
        threadBlock *blk = new threadBlock(this->threadsPerBlock, it->block_id_x, it->block_id_y, this->blockDims, this->threads_x, this->threads_y, it->sm_id);
        blocks.insert(std::pair<int,threadBlock>(it->sm_id,*blk));
    }
}

void simulation::printBlocks(){
    for (std::multimap<int,threadBlock>::iterator it=blocks.begin(); it!=blocks.end(); ++it){
        printf("BX: %d BY: %d SM: %d\n",it->second.getBlockIdX(), it->second.getBlockIdY(), it->second.getMappedToSM());
    }
}

void simulation::printBlockAccessLists(){
    for (std::multimap<int,threadBlock>::iterator it=blocks.begin(); it!=blocks.end(); ++it){
        it->second.printInstructionStream();
    }
}


void simulation::mapAccessToBlock(int in_tx, int in_ty, int in_bx, int in_by, int in_wid, std::string in_dsname, int in_oper, long long in_idx, long long in_address, long long in_cycles){
    for (std::multimap<int,threadBlock>::iterator it=blocks.begin(); it!=blocks.end(); ++it) {
        if (in_bx == it->second.getBlockIdX() && in_by == it->second.getBlockIdY()){
            it->second.addAccessToLocalList(in_tx, in_ty, in_wid, in_dsname, in_oper, in_idx, in_address, in_cycles);
        }
    }
}

void simulation::sortAllBlockAccesses(){
    for (std::multimap<int,threadBlock>::iterator it=blocks.begin(); it!=blocks.end(); ++it){
        it->second.sortAccessEntries();
    }
}


/*Setters and Getters*/
int simulation::getNumBlocks() const
{
    return numBlocks;
}

void simulation::setNumBlocks(int value)
{
    numBlocks = value;
}

int simulation::getThreadsPerBlock() const
{
    return threadsPerBlock;
}

void simulation::setThreadsPerBlock(int value)
{
    threadsPerBlock = value;
}

bool simulation::getPauseFlag() const
{
    return pauseFlag;
}

void simulation::setPauseFlag(bool value)
{
    pauseFlag = value;
}

bool simulation::getReadyToStart() const
{
    return readyToStart;
}

void simulation::setReadyToStart(bool value)
{
    readyToStart = value;
}

bool simulation::getStopFlag() const
{
    return stopFlag;
}

void simulation::setStopFlag(bool value)
{
    stopFlag = value;
}

int simulation::getBlocks_z() const
{
    return blocks_z;
}

void simulation::setBlocks_z(int value)
{
    blocks_z = value;
}

int simulation::getBlocks_y() const
{
    return blocks_y;
}

void simulation::setBlocks_y(int value)
{
    blocks_y = value;
}

int simulation::getBlocks_x() const
{
    return blocks_x;
}

void simulation::setBlocks_x(int value)
{
    blocks_x = value;
}

int simulation::getThreads_x() const
{
    return threads_x;
}

void simulation::setThreads_x(int value)
{
    threads_x = value;
}

int simulation::getThreads_y() const
{
    return threads_y;
}

void simulation::setThreads_y(int value)
{
    threads_y = value;
}

int simulation::getThreads_z() const
{
    return threads_z;
}

void simulation::setThreads_z(int value)
{
    threads_z = value;
}

int simulation::getBlockDims() const
{
    return blockDims;
}

void simulation::setBlockDims(int value)
{
    blockDims = value;
}

int simulation::getThreadDims() const
{
    return threadDims;
}

void simulation::setThreadDims(int value)
{
    threadDims = value;
}
