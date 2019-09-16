/*Class for representing the simulation that the tool runs.
  It acts as a parent object to the threadblocks*/
#include <algorithm>
#include <string>
#include <cmath>
#include "simulation.h"
#include "globals.h"
#include "threadblock.h"

/*Constructor*/
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
    this->current_active_blocks = 0;
    this->isRunning = false;
    int result = int(log2(num_sets_l2));

    switch (result) {
        case 5: this->bit_mask = MASK_5bit; break;
        case 6: this->bit_mask = MASK_6bit; break;
        case 7: this->bit_mask = MASK_7bit; break;
        case 8: this->bit_mask = MASK_8bit; break;
        case 9: this->bit_mask = MASK_9bit; break;
        case 10: this->bit_mask = MASK_10bit; break;
        case 11: this->bit_mask = MASK_11bit; break;
        case 12: this->bit_mask = MASK_12bit; break;
        case 13: this->bit_mask = MASK_13bit; break;
        case 14: this->bit_mask = MASK_14bit; break;
        case 15: this->bit_mask = MASK_15bit; break;
        case 16: this->bit_mask = MASK_16bit; break;
        default: qDebug("Error in bitmask resolution!\n"); break;
    }
}


void simulation::cleanAll(){
    block_schedule.clear();
    for (auto it=blocks.begin(); it!=blocks.end(); ++it){
        it->second.clearAllData();
    }

    blocks.clear();
    this->stopFlag = false;
    this->pauseFlag = false;
    this->readyToStart = false;
    this->isRunning = false;
    this->numBlocks = 0;
    this->threadsPerBlock = 0;
    this->threads_x = 0;
    this->threads_y = 0;
    this->threads_z = 0;
    this->blocks_x = 0;
    this->blocks_y = 0;
    this->blocks_z = 0;
    this->current_active_blocks = 0;
    this->isRunning = false;
}

/*Runtime functions (4)*/
void simulation::prepareInitialBlocks(){
    int max_blocks = std::min(num_sm, int(block_schedule.size()));
    int first_blocks = 0;
    for (auto it = block_schedule.begin(); it != block_schedule.end(); it++) {
        if (first_blocks < max_blocks){
            blocks.find(it->sm_id)->second.setRunning(true);
            first_blocks++;
        }
    }
    for (int i = 0; i < first_blocks; i++) {
        block_schedule.pop_front();
    }
}

bool simulation::isSimulationComplete(){
    bool ret_val = true;
    for (auto it = blocks.begin(); it != blocks.end(); it++) {
        if(!it->second.isFinished()){ret_val = false;}
    }
    return ret_val;
}

void simulation::updateBitMask()
{
    int result = int(log2(num_sets_l2));

    switch (result) {
        case 5: this->bit_mask = MASK_5bit; break;
        case 6: this->bit_mask = MASK_6bit; break;
        case 7: this->bit_mask = MASK_7bit; break;
        case 8: this->bit_mask = MASK_8bit; break;
        case 9: this->bit_mask = MASK_9bit; break;
        case 10: this->bit_mask = MASK_10bit; break;
        case 11: this->bit_mask = MASK_11bit; break;
        case 12: this->bit_mask = MASK_12bit; break;
        case 13: this->bit_mask = MASK_13bit; break;
        case 14: this->bit_mask = MASK_14bit; break;
        case 15: this->bit_mask = MASK_15bit; break;
        case 16: this->bit_mask = MASK_16bit; break;
        default: qDebug("Error in bitmask resolution!\n"); break;
    }
}

/*Look at all scheduled blocks and pick the one with the lowest cycle counter to execute*/
int simulation::findNextInstructionFromBlocks(){
    long long min_cyc = LLONG_MAX;
    int id = EMPTY_RET;
    int cur_active_blocks = 0;
    for (auto it = blocks.begin(); it != blocks.end(); it++) {
        if (it->second.getRunning()){
            cur_active_blocks++;
            if (it->second.getNextCycleVal() < min_cyc){
                min_cyc = it->second.getNextCycleVal();
                id = it->second.getMappedToSM();
            }
        }
    }

    if(cur_active_blocks < num_sm){
        if(!block_schedule.empty()){
            int fresh_blocks = std::min(int(block_schedule.size()),(num_sm - current_active_blocks));
            for (int i = 0; i < fresh_blocks; i++) {
                int temp_id = block_schedule.front().sm_id;
                blocks.find(temp_id)->second.setRunning(true);
                if (blocks.find(temp_id)->second.getNextCycleVal() < min_cyc){
                    min_cyc = blocks.find(temp_id)->second.getNextCycleVal();
                    id = blocks.find(temp_id)->second.getMappedToSM();
                }
                block_schedule.pop_front();
            }
        }
    }
    return id;
}


std::list<update_line_info> simulation::getUpdateInfoFromBlock(){
    std::list<update_line_info> tmp_list;
    int block_id = this->findNextInstructionFromBlocks();
    if(block_id != EMPTY_RET){
        tmp_list = blocks.find(block_id)->second.getUpdateInfo();
    }

    return tmp_list;
}


void simulation::getMinMaxFromBlocks(){
    for (auto it = blocks.begin(); it != blocks.end(); ++it){
        qDebug("Block: %d Min: %llu Max: %llu",it->second.getBlockIdX() , it->second.getMinCyc() ,it->second.getMaxCyc());
    }
}


/*Input & Setup functions (6)*/
void simulation::mapAccessToBlock(int in_tx, int in_ty, int in_bx, int in_by, int in_wid, std::string in_dsname, int in_oper, long long in_idx, long long in_address, long long in_cycles){
    for (auto it = blocks.begin(); it != blocks.end(); ++it) {
        if (in_bx == it->second.getBlockIdX() && in_by == it->second.getBlockIdY()){
            it->second.addAccessToLocalList(in_tx, in_ty, in_wid, in_dsname, in_oper, in_idx, in_address, in_cycles);
        }
    }
}

void simulation::sortAllBlockAccesses(){
    for (auto it = blocks.begin(); it != blocks.end(); ++it){
        it->second.sortAccessEntries();
    }
    getMinMaxFromBlocks();
}

void simulation::addScheduleEntry(int bx, int by, int sm, long long t){
    schedule_entry entry{bx, by, sm, t};
    block_schedule.push_back(entry);
}

void simulation::sortSchedulingEntries(){
    block_schedule.sort();
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
    int count = 0;
    for (it = block_schedule.begin(); it != block_schedule.end(); it++) {
        threadBlock *blk = new threadBlock(this->threadsPerBlock, it->block_id_x, it->block_id_y, this->blockDims, this->threads_x, this->threads_y, it->sm_id, this->bit_mask);
        blocks.insert(std::pair<int,threadBlock>(it->sm_id,*blk));
        count++;
    }
    this->numBlocks = count;
}

/*Debug Functions*/
void simulation::printBlocks(){
    for (auto it = blocks.begin(); it != blocks.end(); ++it){
        printf("BX: %d BY: %d SM: %d\n",it->second.getBlockIdX(), it->second.getBlockIdY(), it->second.getMappedToSM());
    }
}

void simulation::printBlockAccessLists(){
    for (auto it = blocks.begin(); it != blocks.end(); ++it){
        it->second.printInstructionStream();
    }
}

void simulation::printSchedule(){
    for (auto it=block_schedule.begin(); it != block_schedule.end(); it++){
        printf("X:%d Y:%d SM:%d T:%llu\n", it->block_id_x, it->block_id_y, it->sm_id, it->g_time);
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

bool simulation::getIsRunning() const
{
    return isRunning;
}

void simulation::setIsRunning(bool value)
{
    isRunning = value;
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

int simulation::getCurrent_active_blocks() const
{
    return current_active_blocks;
}

void simulation::setCurrent_active_blocks(int value)
{
    current_active_blocks = value;
}
int simulation::getBit_mask() const
{
    return bit_mask;
}

void simulation::setBit_mask(int value)
{
    bit_mask = value;
}
