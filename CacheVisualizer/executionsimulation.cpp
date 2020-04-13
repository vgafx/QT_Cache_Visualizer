#include "executionsimulation.h"

#include <iostream>

/*Private Methods*/
void ExecutionSimulation::popVectorFront(std::vector<schedule_entry> &in_vec)
{
    if (in_vec.size() > 0){
        in_vec.erase(in_vec.begin());
    }
}


std::tuple<int, int, int>& ExecutionSimulation::findNextInstructionFromBlocks()
{
    unsigned long long min_time = LLONG_MAX;
    int cur_active_blocks = 0;
    std::tuple<int, int, int>& ret_ref = m_empty_key_ret;

    for (auto it = m_blocks.begin(); it != m_blocks.end(); it++) {
        if(it->second->getIsRunning()){
            cur_active_blocks++;
            if (it->second->getNextTimeStamp()< min_time){
                min_time = it->second->getNextTimeStamp();
                ret_ref = it->first;
            }
        }
    }

    if(cur_active_blocks < Settings::getNumSm()){
        if(!m_block_schedule.empty()){
            qDebug("findNextInstructionFromBlocks() - Block Schedule not empty! Size: %ld\n",m_block_schedule.size());
            int fresh_blocks = std::min(static_cast<int>(m_block_schedule.size()), Settings::getNumSm() - cur_active_blocks);
            for (int i = 0; i < fresh_blocks; ++i) {
                std::tuple<int, int, int> f_key{m_block_schedule.front().block_id_x, m_block_schedule.front().block_id_y, m_block_schedule.front().sm_id};
                m_blocks.find(f_key)->second->setIsRunning(true); //!! check for calling twice
                if (m_blocks.find(f_key)->second->getNextTimeStamp() < min_time){
                    min_time = m_blocks.find(f_key)->second->getNextTimeStamp();
                    ret_ref = m_blocks.find(f_key)->first;
                }
                popVectorFront(m_block_schedule);
            }
        }
    }
    return ret_ref;
}


/*Public Methods*/
ExecutionSimulation::ExecutionSimulation()
    : m_empty_key_ret({EMPTY, EMPTY, EMPTY}), m_most_recent_block(m_empty_key_ret)
{
    m_num_blocks = 0;
    m_blocks_x = 0;
    m_blocks_y = 0;
    m_blocks_z = 0;
    m_block_dims = 0;
    m_threads_per_block = 0;
    m_threads_x = 0;
    m_threads_y = 0;
    m_threads_z = 0;
    m_thread_dims = 0;
    m_current_active_blocks = 0;
    m_pause_flag = false;
    m_stop_flag = false;
    m_ready = false;
    m_is_running = false;

    //m_empty_key_ret = {EMPTY, EMPTY, EMPTY};

    m_empty_update_ret.emplace_back(NORET, NORET, WRITE, 'Z', 0, 0, 0, 0, 0, false, false, false, false, NORET, NORET, NORET, NORET);

    //m_most_recent_block = m_empty_key_ret;

    this->updateBitMask();
}


bool ExecutionSimulation::isSimulationComplete() const
{
    bool ret_val = true;
    for (auto it = m_blocks.begin(); it != m_blocks.end(); it++){
        if(!it->second->isFinished()){ret_val = false; break;}
    }
    return ret_val;
}

void ExecutionSimulation::sortAllBlockInstructions() const
{
    for (auto it = m_blocks.begin(); it != m_blocks.end(); it++) {
        it->second->sortAccessEntries();
    }
    this->getMinMaxInstrTime();
}

void ExecutionSimulation::setStreamReserve() const
{
    for (auto it = m_blocks.begin(); it != m_blocks.end(); it++) {
        //Settings::getReserveSize();
        it->second->setInstructionStreamReserveSize(0);
    }
}

/*Sorts the block schedule according to timestamps*/
void ExecutionSimulation::sortSchedulingEntries()
{
    std::sort(m_block_schedule.begin(), m_block_schedule.end());
}

void ExecutionSimulation::prepareInitialBlocks()
{
    int max_blocks = std::min(Settings::getNumSm(), static_cast<int>(m_block_schedule.size()));
    int first_blocks = 0;
    for (auto it = m_block_schedule.begin(); it != m_block_schedule.end(); it++) {
        if (first_blocks < max_blocks){
            std::tuple<int,int,int> unique_key(it->block_id_x, it->block_id_y, it->sm_id);
            auto it = m_blocks.find(unique_key);
            if (it == m_blocks.end()){
                qDebug("DBG: Missmatch between block schedule and block object\n");
            } else {
                m_blocks.find(unique_key)->second->setIsRunning(true);
                first_blocks++;
            }
        }
    }

    for (int i = 0; i < first_blocks; i++) {
        this->popVectorFront(m_block_schedule);
    }
}

void ExecutionSimulation::generateBlocks()
{
    int count = 0;
    for (auto it = m_block_schedule.begin(); it != m_block_schedule.end(); it++) {
        m_blocks.emplace(std::make_pair(std::make_tuple(it->block_id_x, it->block_id_y, it->sm_id),
                                        std::make_unique<Block>(m_threads_per_block, it->block_id_x, it->block_id_y, m_block_dims,it->sm_id, m_bit_mask)));
        count++;
    }
    m_num_blocks = count;
}

/*Updates the bitmask used by the hashing function based on current settings*/
void ExecutionSimulation::updateBitMask()
{
    int result = static_cast<int>(std::log2(Settings::getNumSetsL2()));

    switch (result) {
        case 5: m_bit_mask = MASK_5_BIT; break;
        case 6: m_bit_mask = MASK_6_BIT; break;
        case 7: m_bit_mask = MASK_7_BIT; break;
        case 8: m_bit_mask = MASK_8_BIT; break;
        case 9: m_bit_mask = MASK_9_BIT; break;
        case 10: m_bit_mask = MASK_10_BIT; break;
        case 11: m_bit_mask = MASK_11_BIT; break;
        case 12: m_bit_mask = MASK_12_BIT; break;
        case 13: m_bit_mask = MASK_13_BIT; break;
        case 14: m_bit_mask = MASK_14_BIT; break;
        case 15: m_bit_mask = MASK_15_BIT; break;
        case 16: m_bit_mask = MASK_16_BIT; break;
        case 17: m_bit_mask = MASK_17_BIT; break;
        case 18: m_bit_mask = MASK_18_BIT; break;
        case 19: m_bit_mask = MASK_19_BIT; break;
        case 20: m_bit_mask = MASK_20_BIT; break;
        default:
            m_bit_mask = MASK_7_BIT;
            qDebug("Simulation: Could not resolve m_bit_mask\n");
            break;
    }
}

void ExecutionSimulation::configureDims()
{
    int total_t_dims = 3, total_b_dims = 3;
    if (m_threads_z == 0){total_t_dims--;}
    if (m_threads_y == 0){total_t_dims--;}
    if (m_blocks_z == 0){total_b_dims--;}
    if (m_blocks_y == 0){total_b_dims--;}
    m_thread_dims = total_t_dims;
    m_block_dims = total_b_dims;
}


/*Clears all stored data from simulation and threadblock objects*/
void ExecutionSimulation::cleanAll()
{
    if(!m_block_schedule.empty()){
        m_block_schedule.clear();
        m_block_schedule.shrink_to_fit();
    }

    if(!m_blocks.empty()){
        for (auto it = m_blocks.begin(); it != m_blocks.end(); it++){
            it->second->clearAllData();
        }
        /*The unique_ptrs should clean up when clear is called on the container*/
        m_blocks.clear();
    }

    m_most_recent_block = m_empty_key_ret;

    m_num_blocks = 0;
    m_blocks_x = 0;
    m_blocks_y = 0;
    m_blocks_z = 0;
    m_block_dims = 0;
    m_threads_per_block = 0;
    m_threads_x = 0;
    m_threads_y = 0;
    m_threads_z = 0;
    m_thread_dims = 0;
    m_current_active_blocks = 0;
    m_pause_flag = false;
    m_stop_flag = false;
    m_ready = false;
    m_is_running = false;
}

/*Add block scheduling entry inplace*/
void ExecutionSimulation::addScheduleEntry(int bx, int by, int sm, long long t)
{
    m_block_schedule.emplace_back(bx,by,sm,t);
}

/*Passes read trace entry to the corresponding block*/
void ExecutionSimulation::mapAccessToBlock(int in_bx, int in_by, int in_wid, char in_dsname, OperationType in_oper, unsigned int in_idx, unsigned long long in_address, unsigned long long in_time)
{
    for (auto it = m_blocks.begin(); it != m_blocks.end(); it++) {
        if (it->second->getBlockIdX() == in_bx && it->second->getBlockIdY() == in_by){
            it->second->addAccessToLocalList(in_wid, in_dsname, in_oper, in_idx, in_address, in_time);
            break;
        }
    }
}

std::vector<update_cline_info<unsigned int, unsigned long long>>& ExecutionSimulation::getUpdateInfoFromBlock()
{
    /*Get the id/sm of the block that executes the next instruction*/
    std::tuple<int,int,int>& key_ref = this->findNextInstructionFromBlocks();
    m_most_recent_block = key_ref;
    /*Find the corresponding block and retrieve the update information*/
    if(std::get<0>(key_ref) != EMPTY && std::get<1>(key_ref) != EMPTY && std::get<2>(key_ref) != EMPTY){
        auto& ret_ref = m_blocks.find(key_ref)->second->getUpdateInfoRef();
        if(ret_ref.front().set_idx == m_empty_update_ret.front().set_idx && ret_ref.front().tag == m_empty_update_ret.front().tag){
            qDebug("Firing NO_RET\n");
            ret_ref = m_empty_update_ret;
            return ret_ref;
        } else {
            return ret_ref;
        }

    } else {
        auto& ret_ref = m_empty_update_ret;
        qDebug("Returning EMPTY\n");
        return ret_ref;
    }
}

void ExecutionSimulation::popBlockUpdate() const
{
    if (std::get<0>(m_most_recent_block) != EMPTY && std::get<1>(m_most_recent_block) != EMPTY && std::get<2>(m_most_recent_block) != EMPTY){
        m_blocks.find(m_most_recent_block)->second->popUpdateInfo();
    } else {
        //qDebug("popBlockUpdate() EMPTY ref\n");
    }
}


/*---Debug methods (5)---*/
void ExecutionSimulation::printBlocks() const
{
    qDebug("----------ThreadBlocks----------\n");
    for (auto it = m_blocks.begin(); it != m_blocks.end(); ++it){
        qDebug("BX: %d BY: %d SM: %d\n",it->second->getBlockIdX(), it->second->getBlockIdY(), it->second->getMappedToSm());
    }
}


void ExecutionSimulation::printSchedule() const
{
    qDebug("----------ThreadBlock Schedule----------\n");
    for (auto it = m_block_schedule.begin(); it != m_block_schedule.end(); it++) {
        qDebug("X:%d Y:%d SM:%d T:%llu\n", it->block_id_x, it->block_id_y, it->sm_id, it->g_time);
    }
}

void ExecutionSimulation::printBlockAccessLists() const
{
    for (auto it = m_blocks.begin(); it != m_blocks.end(); ++it){
        qDebug("+ThreadBlock X:%d Y:%d SM:%d Instruction Stream ==\n", std::get<0>(it->first), std::get<1>(it->first), std::get<2>(it->first));
        it->second->printInstructionStream();
    }
}

void ExecutionSimulation::getMinMaxInstrTime() const
{
    qDebug("Getting Min and Max timestamp for all instructions, from all blocks\n");
    for (auto it = m_blocks.begin(); it != m_blocks.end(); ++it){
        qDebug("BlockX: %d BlockY: %d Min: %llu Max: %llu",it->second->getBlockIdX() ,it->second->getBlockIdY(), it->second->getMinTime() ,it->second->getMaxTime());
    }
}

void ExecutionSimulation::getInstructionCounts() const
{
    unsigned long long count = 0;
    for (auto it = m_blocks.begin(); it != m_blocks.end(); ++it){
        qDebug("BlockX: %d BlockY: %d  Instructions: %llu ",it->second->getBlockIdX() , it->second->getBlockIdY() ,it->second->getInstructionCount());
        if(it->second->getInstructionCount()>0){
            count+= it->second->getInstructionCount();
        }
    }
    qDebug("Total instructions %llu\n", count);
}
/*---End Debug methods (5)---*/


/*Public Setters & Getters*/
int ExecutionSimulation::getNumBlocks() const
{
    return m_num_blocks;
}

void ExecutionSimulation::setNumBlocks(int num_blocks)
{
    m_num_blocks = num_blocks;
    /*Also reserve the space*/
    m_block_schedule.reserve(static_cast<unsigned long>(num_blocks));
}

int ExecutionSimulation::getBlocksX() const
{
    return m_blocks_x;
}

void ExecutionSimulation::setBlocksX(int blocks_x)
{
    m_blocks_x = blocks_x;
}

int ExecutionSimulation::getBlocksY() const
{
    return m_blocks_y;
}

void ExecutionSimulation::setBlocksY(int blocks_y)
{
    m_blocks_y = blocks_y;
}

int ExecutionSimulation::getBlocksZ() const
{
    return m_blocks_z;
}

void ExecutionSimulation::setBlocksZ(int blocks_z)
{
    m_blocks_z = blocks_z;
}

int ExecutionSimulation::getBlockDims() const
{
    return m_block_dims;
}

void ExecutionSimulation::setBlockDims(int block_dims)
{
    m_block_dims = block_dims;
}

int ExecutionSimulation::getThreadsPerBlock() const
{
    return m_threads_per_block;
}

void ExecutionSimulation::setThreadsPerBlock(int threads_per_block)
{
    m_threads_per_block = threads_per_block;
}

int ExecutionSimulation::getThreadsX() const
{
    return m_threads_x;
}

void ExecutionSimulation::setThreadsX(int threads_x)
{
    m_threads_x = threads_x;
}

int ExecutionSimulation::getThreadsY() const
{
    return m_threads_y;
}

void ExecutionSimulation::setThreadsY(int threads_y)
{
    m_threads_y = threads_y;
}

int ExecutionSimulation::getThreadsZ() const
{
    return m_threads_z;
}

void ExecutionSimulation::setThreadsZ(int threads_z)
{
    m_threads_z = threads_z;
}

int ExecutionSimulation::getThreadDims() const
{
    return m_thread_dims;
}

void ExecutionSimulation::setThreadDims(int thread_dims)
{
    m_thread_dims = thread_dims;
}

int ExecutionSimulation::getCurrentActiveBlocks() const
{
    return m_current_active_blocks;
}

void ExecutionSimulation::setCurrentActiveBlocks(int current_active_blocks)
{
    m_current_active_blocks = current_active_blocks;
}

int ExecutionSimulation::getBitMask() const
{
    return m_bit_mask;
}

void ExecutionSimulation::setBitMask(int bit_mask)
{
    m_bit_mask = bit_mask;
}

bool ExecutionSimulation::getPauseFlag() const
{
    return m_pause_flag;
}

void ExecutionSimulation::setPauseFlag(bool pause_flag)
{
    m_pause_flag = pause_flag;
}

bool ExecutionSimulation::getStopFlag() const
{
    return m_stop_flag;
}

void ExecutionSimulation::setStopFlag(bool stop_flag)
{
    m_stop_flag = stop_flag;
}

bool ExecutionSimulation::getReady() const
{
    return m_ready;
}

void ExecutionSimulation::setReady(bool ready)
{
    m_ready = ready;
}

bool ExecutionSimulation::getIsRunning() const
{
    return m_is_running;
}

void ExecutionSimulation::setIsRunning(bool is_running)
{
    m_is_running = is_running;
}

