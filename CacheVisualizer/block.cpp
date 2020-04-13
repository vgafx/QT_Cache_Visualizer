#include "block.h"

/*Definition*/
int Block::m_num_threads;
int Block::m_num_warps;
int Block::m_tag_line_offset;
unsigned long long Block::m_bit_mask;


/*Cache Mapping Functions (3)*/
int Block::generateSectorIndex(unsigned long long address) const
{
    static unsigned long long sector_mask = static_cast<unsigned long long>(Settings::getClineSize() - 1);
    static unsigned long long sector_size = static_cast<unsigned long long>(Settings::getSectorSize());
    address &= sector_mask;
    address /= sector_size;
    return static_cast<int>(address);
}

int Block::generateSetIndex(unsigned long long address) const
{
    address >>= Settings::getBlockOffsetBits();
    address &= m_bit_mask;
    return static_cast<int>(address);
}

int Block::generateClineTag(unsigned long long address) const
{
    address >>= Settings::getBlockOffsetBits();
    address >>= m_tag_line_offset;
    return static_cast<int>(address);
}


void Block::updateNextTimeCounter()
{
    if (!m_instruction_stream.empty()){
        m_next_time_stamp = m_instruction_stream.back().timestamp;
    } else {
        m_is_running = false;
        m_is_retired = true;
        m_next_time_stamp = LLONG_MAX;
    }
}
/*End of private methods*/

Block::Block(int num_threads, int bx, int by, int dims, int sm, int b_mask)
{
    m_num_threads = num_threads;
    m_num_warps = m_num_threads / Settings::getWarpSize();
    m_block_dim = dims;
    m_block_id_x = bx;
    m_block_id_y = by;
    m_mapped_to_sm = sm;
    m_bit_mask = static_cast<unsigned long long>(b_mask);
    m_next_time_stamp = 0;
    m_tag_line_offset = static_cast<int>(std::log2(Settings::getNumSetsL2()));
    m_is_retired = false;
    m_is_running = false;

    m_empty_update.emplace_back(NORET, NORET, WRITE, 'Z', 0, 0, 0, 0, 0, true, true, true, true, NORET, NORET, NORET, NORET);

}

bool Block::isFinished() const
{
    return (m_instruction_stream.empty()) ? true : false;
}


void Block::clearAllData()
{
    if (!m_instruction_stream.empty()){
        m_instruction_stream.clear();
        m_instruction_stream.shrink_to_fit();
    }

    if (!m_update_stream.empty()){
        m_update_stream.clear();
        m_update_stream.shrink_to_fit();
    }
}

void Block::sortAccessEntries()
{
    std::sort(m_instruction_stream.begin(), m_instruction_stream.end());
    this->updateNextTimeCounter();
}

void Block::addAccessToLocalList(int wid, char ds_name, OperationType op, unsigned int idx, unsigned long long address, unsigned long long time)
{
    m_instruction_stream.emplace_back(wid, ds_name, op, idx, address, time);
}

void Block::popUpdateInfo()
{
    if (!m_update_stream.empty()) { m_update_stream.pop_back(); }
}

/*Returns a reference to the next update info*/
std::vector<update_cline_info<unsigned int, unsigned long long>>& Block::getUpdateInfoRef()
{
    if (m_instruction_stream.empty()){
        std::vector<update_cline_info<unsigned int, unsigned long long>>& ref = m_empty_update;
        return ref;
    } else {
        this->processAccessEntry();
        std::vector<update_cline_info<unsigned int, unsigned long long>>& ref = m_update_stream.front();
        return ref;
    }
}


/*Transforms a series of mem accesses into update cacheline information*/
void Block::processAccessEntry()
{
    if (!m_instruction_stream.empty()){
        int wid = m_instruction_stream.back().warp_id;
        unsigned long long ts = m_instruction_stream.back().timestamp;
        char dname = m_instruction_stream.back().ds_name;
        OperationType oper = m_instruction_stream.back().op;

        std::vector<update_cline_info<unsigned int, unsigned long long>> unique_entries;
        /*Process up to warp size accesses*/
        int entries = std::min(Settings::getWarpSize(), static_cast<int>(m_instruction_stream.size()));

        for (auto it = m_instruction_stream.rbegin(); it != m_instruction_stream.rbegin() + entries; ++it) {
            /*Check if instructions are issued by the same warp*/
            if (wid == it->warp_id && ts == it->timestamp){
                unsigned long long c_address =it->address;
                unsigned int c_idx = it->ds_index;
                unsigned long long c_time = it->timestamp;
                int c_sector_id = this->generateSectorIndex(c_address);
                int c_set_id = this->generateSetIndex(c_address);
                int c_line_tag = this->generateClineTag(c_address);
                bool s0 = false, s1 = false, s2 = false, s3 = false;
                int s0_req = 0, s1_req = 0, s2_req = 0, s3_req = 0;

                switch(c_sector_id){
                    case 0: s0 = true; s0_req++; break;
                    case 1: s1 = true; s1_req++; break;
                    case 2: s2 = true; s2_req++; break;
                    case 3: s3 = true; s3_req++; break;
                }

                if (unique_entries.empty())
                {//Record the first update entry
                    unique_entries.emplace_back(c_set_id, c_line_tag, oper, dname, c_idx, c_idx, c_address, c_address,
                                                c_time, s0, s1, s2, s3, s0_req, s1_req, s2_req, s3_req);
                }
                else
                {//Check if we have already seen this set idx
                    bool exists = false;
                    for (size_t s = 0; s < unique_entries.size(); s++) {
                        if (c_set_id == unique_entries[s].set_idx && c_line_tag == unique_entries[s].tag){
                            /*Update existing entry*/
                            exists = true;
                            if (c_address < unique_entries[s].add_low) {unique_entries[s].add_low = c_address;}
                            if (c_address > unique_entries[s].add_high) {unique_entries[s].add_high = c_address;}
                            if (c_idx < unique_entries[s].idx_low) {unique_entries[s].idx_low = c_idx;}
                            if (c_idx > unique_entries[s].idx_high) {unique_entries[s].idx_high = c_idx;}
                            //Update sector filled flags
                            if (!unique_entries[s].s0 && s0) {unique_entries[s].s0 = s0;}
                            if (!unique_entries[s].s1 && s1) {unique_entries[s].s1 = s1;}
                            if (!unique_entries[s].s2 && s2) {unique_entries[s].s2 = s2;}
                            if (!unique_entries[s].s3 && s3) {unique_entries[s].s3 = s3;}

                            //Update number of requests on per-sector basis
                            if (s0){ unique_entries[s].s0_req++;}
                            if (s1){ unique_entries[s].s1_req++;}
                            if (s2){ unique_entries[s].s2_req++;}
                            if (s3){ unique_entries[s].s3_req++;}
                            break;
                        }
                    }
                    if (!exists){
                        unique_entries.emplace_back(c_set_id, c_line_tag, oper, dname, c_idx, c_idx, c_address, c_address,
                                                    c_time, s0, s1, s2, s3, s0_req, s1_req, s2_req, s3_req);
                    }
                } //Instruction
            } //Same warp
        } // end for
        /*Update info is ready, push it to the update stream*/
        if (!unique_entries.empty()){
            m_update_stream.emplace_back(unique_entries);
        }
        /*Remove the mem access info we just processed from the instruction stream*/
        for(int r = 0; r < entries; r++){
            m_instruction_stream.pop_back();
        }

    } // .empty()
    /*Update the counter for the next iteration*/
    this->updateNextTimeCounter();
}

void Block::setInstructionStreamReserveSize(size_t size)
{
    m_instruction_stream.reserve(size);
}


/*Debug*/
void Block::printInstructionStream() const
{
    qDebug("---BLOCK---: %d", m_block_id_x);
    for (auto it = m_instruction_stream.begin(); it != m_instruction_stream.end(); it++) {
        qDebug("W: %d DS: %c OP: %d IDX: %d ADD: %llu Time: %llu\n", it->warp_id, it->ds_name, it->op, it->ds_index, it->address, it->timestamp);
    }
}

unsigned long long Block::getInstructionCount() const
{
    return m_instruction_stream.size();
}

unsigned long long Block::getMinTime() const
{
    return m_instruction_stream.back().timestamp;
}

unsigned long long Block::getMaxTime() const
{
    return m_instruction_stream.front().timestamp;
}

/*Setters & Getters*/
int Block::getBlockDim() const
{
    return m_block_dim;
}

void Block::setBlockDim(int block_dim)
{
    m_block_dim = block_dim;
}

int Block::getBlockId() const
{
    return m_block_id;
}

void Block::setBlockId(int block_id)
{
    m_block_id = block_id;
}

int Block::getBlockIdX() const
{
    return m_block_id_x;
}

void Block::setBlockIdX(int block_id_x)
{
    m_block_id_x = block_id_x;
}

int Block::getBlockIdY() const
{
    return m_block_id_y;
}

void Block::setBlockIdY(int block_id_y)
{
    m_block_id_y = block_id_y;
}

int Block::getBlockIdZ() const
{
    return m_block_id_z;
}

void Block::setBlockIdZ(int block_id_z)
{
    m_block_id_z = block_id_z;
}

int Block::getNumThreads()
{
    return m_num_threads;
}

void Block::setNumThreads(int num_threads)
{
    m_num_threads = num_threads;
}

int Block::getNumWarps()
{
    return m_num_warps;
}

void Block::setNumWarps(int num_warps)
{
    m_num_warps = num_warps;
}

unsigned long long Block::getBitMask()
{
    return m_bit_mask;
}

void Block::setBitMask(unsigned long long bit_mask)
{
    m_bit_mask = bit_mask;
}

int Block::getTagLineOffset()
{
    return m_tag_line_offset;
}

void Block::setTagLineOffset(int tag_line_offset)
{
    m_tag_line_offset = tag_line_offset;
}

int Block::getMappedToSm() const
{
    return m_mapped_to_sm;
}

void Block::setMappedToSm(int mapped_to_sm)
{
    m_mapped_to_sm = mapped_to_sm;
}

unsigned long long Block::getNextAddress() const
{
    return m_next_address;
}

void Block::setNextAddress(unsigned long long next_address)
{
    m_next_address = next_address;
}

unsigned long long Block::getNextTimeStamp() const
{
    return m_next_time_stamp;
}

void Block::setNextTimeStamp(unsigned long long next_time_stamp)
{
    m_next_time_stamp = next_time_stamp;
}

bool Block::getIsRetired() const
{
    return m_is_retired;
}

void Block::setIsRetired(bool is_retired)
{
    m_is_retired = is_retired;
}

bool Block::getIsRunning() const
{
    return m_is_running;
}

void Block::setIsRunning(bool is_running)
{
    m_is_running = is_running;
}
