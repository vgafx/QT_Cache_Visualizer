#include "cache.h"

/*Generates a random number from uniform distribution for Random eviction policy*/
void Cache::setMemLatency(unsigned long long mem_latency)
{
    m_mem_latency = mem_latency;
}

int Cache::getRandomNumber()
{
    std::mt19937 rng(m_rd());
    std::uniform_int_distribution<int> uni(1,Settings::getWaySizeL2());
    return uni(rng);
}


/*Wrapper Method to process all update information in steps*/
void Cache::processRead(std::vector<update_cline_info<unsigned int, unsigned long long> > &up_info)
{
    /*Flags for manipulating the state*/
    bool found_data_on_read = false, detected_empy_cline = false;
    bool read_once = true;
    int oldest_age = INT_MIN, oldest_tag = 0;
    int hit_tag = 0;

    /*Iterator pair to point to all cachelines within the set*/
    std::pair<std::multimap<int,Cacheline*>::iterator, std::multimap<int,Cacheline*>::iterator> ret;

    for (auto up_it = up_info.begin(); up_it != up_info.end(); up_it++) {
        std::vector<update_cline_info<unsigned int, unsigned long long>>::iterator& it = up_it;

        /*Do a lookup to see if the data is already in the cache*/
        this->readLookup(found_data_on_read, detected_empy_cline, read_once, oldest_age, oldest_tag, hit_tag, ret, it);
        /*If not found, check for an empty spot in the set*/
        this->readLoadOnEmpty(found_data_on_read, detected_empy_cline, read_once, ret, it);
        /*If not empty spot, evict a line and load the data*/
        this->readLoadEviction(found_data_on_read, detected_empy_cline, oldest_tag, ret, it);

        /*Reset flags for reuse*/
        oldest_age = INT_MIN;
        found_data_on_read = false;
        detected_empy_cline = false;
        oldest_tag = 0;
        hit_tag = 0;
        read_once = true;
    }

}


/*Performs the initial lookup into the cache set. Only handles the case where the data is found in the cache, and
  sets the flags passed in by reference for the subsequent method calls.*/
void Cache::readLookup(bool &data_found, bool &empty_cline, bool &r_once, int &oldest_age, int &oldest_tag, int &hit_tag,
                       std::pair<std::multimap<int,Cacheline*>::iterator, std::multimap<int,Cacheline*>::iterator> &ret,
                       std::vector<update_cline_info<unsigned int, unsigned long long>>::iterator& up_it)
{
    /*Get the Cacheline objects that have the specified set index (update info)*/
    ret = m_idx_map.equal_range(up_it->set_idx);
    r_once = true;
    for (auto set_it = ret.first; set_it != ret.second; set_it++) {
        /*Check if data is there based on tag. Use global to L2 latency to filter concurrent requests*/
        if(set_it->second->getTag() == up_it->tag && set_it->second->getTimeAccessed() + m_mem_latency <= up_it->timestamp){
            /*Given sectored cachelines, check for full or partial hit*/
            if(set_it->second->getS0Filled() == up_it->s0 && set_it->second->getS1Filled() == up_it->s1 &&
               set_it->second->getS2Filled() == up_it->s2 && set_it->second->getS3Filled() == up_it->s3){

                int requests = up_it->s0_req + up_it->s1_req + up_it->s2_req + up_it->s3_req;
                m_stat.recordReadHits(requests);
                m_stat.recordMemoryRequests(requests);
                set_it->second->setAge(0);
                set_it->second->displayFullHit();
            } else {
                /*Partial hit*/
                if (up_it->add_low < set_it->second->getAddressLow()) { set_it->second->setAddressLow(up_it->add_low); }
                if (up_it->add_high > set_it->second->getAddressHigh()) { set_it->second->setAddressHigh(up_it->add_high); }
                if (up_it->idx_low < set_it->second->getIdxLow()) { set_it->second->setIdxLow(up_it->idx_low); }
                if (up_it->idx_high > set_it->second->getIdxHigh()) { set_it->second->setIdxHigh(up_it->idx_high); }
                if (!set_it->second->getS0Filled() && up_it->s0){
                    set_it->second->setS0Filled(true);
                    m_stat.recordReadMisses(up_it->s0_req);
                } else {
                    m_stat.recordReadHits(up_it->s0_req);
                }

                if (!set_it->second->getS1Filled() && up_it->s1){
                    set_it->second->setS1Filled(true);
                    m_stat.recordReadMisses(up_it->s1_req);
                } else {
                    m_stat.recordReadHits(up_it->s1_req);
                }

                if (!set_it->second->getS2Filled() && up_it->s2){
                    set_it->second->setS2Filled(true);
                    m_stat.recordReadMisses(up_it->s2_req);
                } else {
                    m_stat.recordReadHits(up_it->s2_req);
                }

                if (!set_it->second->getS3Filled() && up_it->s3){
                    set_it->second->setS3Filled(true);
                    m_stat.recordReadMisses(up_it->s3_req);
                } else {
                    m_stat.recordReadHits(up_it->s3_req);
                }

                int mem_req = up_it->s0_req + up_it->s1_req + up_it->s2_req + up_it->s3_req;
                m_stat.recordMemoryRequests(mem_req);
                set_it->second->setDataStructure(up_it->name);
                set_it->second->setAge(0);
                set_it->second->displayPartialHit();

            }//end partial / full
            data_found = true;
            hit_tag = up_it->tag;
        }
        /*While looking at the set, also check if there is an empty spot for a cacheline*/
        if (set_it->second->getIsEmpty()){
            empty_cline = true;
        }
        /*Moreover, check for the oldest cacheline in the set*/
        if (set_it->second->getAge() > oldest_age){
            oldest_age = set_it->second->getAge();
            oldest_tag = set_it->second->getTag();
        }

    }

    /*Finally, if the data is found, make all other cachelines older by 1*/
    if (data_found){
        for (auto set_it = ret.first; set_it != ret.second; set_it++) {
            if (set_it->second->getTag() != hit_tag){
                set_it->second->incAge();
            }
        }
    }

}


/*If the data was not found on the initial lookup & an empty cacheline was detected,
  this method inserts the data into that empty spot.*/
void Cache::readLoadOnEmpty(bool &data_found, bool &empty_cline, bool &r_once,
                            std::pair<std::multimap<int,Cacheline*>::iterator, std::multimap<int,Cacheline*>::iterator> &ret,
                            std::vector<update_cline_info<unsigned int, unsigned long long>>::iterator &up_it)
{
    if (!data_found && empty_cline){
        for (auto set_it = ret.first; set_it != ret.second; set_it++) {
            if (set_it->second->getIsEmpty() && r_once && set_it->second->getTag() == 0){
                /*Update the cacheline*/
                set_it->second->setTag(up_it->tag);
                set_it->second->setAddressLow(up_it->add_low);
                set_it->second->setAddressHigh(up_it->add_high);
                set_it->second->setIdxLow(up_it->idx_low);
                set_it->second->setIdxHigh(up_it->idx_high);
                set_it->second->setDataStructure(up_it->name);
                set_it->second->setAge(0);
                set_it->second->setS0Filled(up_it->s0);
                set_it->second->setS1Filled(up_it->s1);
                set_it->second->setS2Filled(up_it->s2);
                set_it->second->setS3Filled(up_it->s3);
                set_it->second->displayEviction();
                r_once = false;
                /*Update statistics*/
                int requests = up_it->s0_req + up_it->s1_req + up_it->s2_req + up_it->s3_req;
                m_stat.recordMemoryRequests(requests);
                m_stat.recordReadMisses(requests);
            } else {
                set_it->second->incAge();
            }
        }
    }

}

/*If the data  was not found on the initial lookup & the cache is already full,
  this method uses the specified evection policy to remove a cacheline and load the data*/
void Cache::readLoadEviction(bool &data_found, bool &empty_cline, int &oldest_tag,
                             std::pair<std::multimap<int,Cacheline*>::iterator, std::multimap<int,Cacheline*>::iterator> &ret,
                             std::vector<update_cline_info<unsigned int, unsigned long long>>::iterator &up_it)
{
    if (!data_found && !empty_cline){
        if (m_rep_policy == LRU){
            for (auto set_it = ret.first; set_it != ret.second; set_it++) {
                /*Evict the oldest*/
                if (set_it->second->getTag() == oldest_tag){
                    set_it->second->setTag(up_it->tag);
                    set_it->second->setAddressLow(up_it->add_low);
                    set_it->second->setAddressHigh(up_it->add_high);
                    set_it->second->setIdxLow(up_it->idx_low);
                    set_it->second->setIdxHigh(up_it->idx_high);
                    set_it->second->setDataStructure(up_it->name);
                    set_it->second->setAge(0);
                    set_it->second->setS0Filled(up_it->s0);
                    set_it->second->setS1Filled(up_it->s1);
                    set_it->second->setS2Filled(up_it->s2);
                    set_it->second->setS3Filled(up_it->s3);
                    set_it->second->displayEviction();
                    /*Update statistics*/
                    int requests = up_it->s0_req + up_it->s1_req + up_it->s2_req + up_it->s3_req;
                    m_stat.recordMemoryRequests(requests);
                    m_stat.recordReadMisses(requests);
                } else {
                    set_it->second->incAge();
                }
            }
        } else if (m_rep_policy == RANDOM){
            int random_id = this->getRandomNumber();
            int counter = 0;

            for (auto set_it = ret.first; set_it != ret.second; set_it++) {
                counter++;
                if (random_id == counter){
                    set_it->second->setTag(up_it->tag);
                    set_it->second->setAddressLow(up_it->add_low);
                    set_it->second->setAddressHigh(up_it->add_high);
                    set_it->second->setIdxLow(up_it->idx_low);
                    set_it->second->setIdxHigh(up_it->idx_high);
                    set_it->second->setDataStructure(up_it->name);
                    set_it->second->setAge(0);
                    set_it->second->setS0Filled(up_it->s0);
                    set_it->second->setS1Filled(up_it->s1);
                    set_it->second->setS2Filled(up_it->s2);
                    set_it->second->setS3Filled(up_it->s3);
                    set_it->second->displayEviction();
                    /*Update statistics*/
                    int requests = up_it->s0_req + up_it->s1_req + up_it->s2_req + up_it->s3_req;
                    m_stat.recordMemoryRequests(requests);
                    m_stat.recordReadMisses(requests);
                } else {
                    set_it->second->incAge();
                }
            }
        }
    }
}


/*The following Methods (4) are the equivalent ones for processing mem writes. They are almost identical,
  and the code was duplicated so that future additions can implement different behaviours for reads / writes
  more easily.*/
void Cache::processWrite(std::vector<update_cline_info<unsigned int, unsigned long long> > &up_info)
{
    /*If Memory writes bypass the L2, we dont affect the state*/
    if (m_w_policy == BYPASS){
        return;
    }
    bool found_data_on_write = false, detected_empy_cline = false;
    bool write_once = true;
    int oldest_age = INT_MIN, oldest_tag = 0;
    int hit_tag = 0;

    /*Iterator pair to point to all cachelines within the set*/
    std::pair <std::multimap<int,Cacheline*>::iterator, std::multimap<int,Cacheline*>::iterator> ret;

    for (auto up_it = up_info.begin(); up_it != up_info.end(); up_it++) {
        std::vector<update_cline_info<unsigned int, unsigned long long>>::iterator& it = up_it;

        /*Do a lookup to see if the data is already in the cache*/
        this->writeLookup(found_data_on_write, detected_empy_cline, write_once, oldest_age, oldest_tag, hit_tag, ret, it);
        /*If not found, check for an empty spot in the set*/
        this->writeLoadOnEmpty(found_data_on_write, detected_empy_cline, write_once, ret, it);
        /*If not empty spot, evict a line and load the data*/
        this->writeLoadEviction(found_data_on_write, detected_empy_cline, oldest_tag, ret, it);

        /*Reset flags for reuse*/
        oldest_age = INT_MIN;
        found_data_on_write = false;
        detected_empy_cline = false;
        oldest_tag = 0;
        hit_tag = 0;
        write_once = true;
    }

}

/*Performs the initial lookup into the cache set. Only handles the case where the data is found in the cache, and
  sets the flags passed in by reference for the subsequent method calls.*/
void Cache::writeLookup(bool &data_found, bool &empty_cline, bool &w_once, int &oldest_age, int &oldest_tag, int &hit_tag,
                        std::pair<std::multimap<int,Cacheline*>::iterator, std::multimap<int,Cacheline*>::iterator>& ret,
                        std::vector<update_cline_info<unsigned int, unsigned long long>>::iterator &up_it)
{
    /*Get the Cacheline objects that have the specified set index (update info)*/
    ret = m_idx_map.equal_range(up_it->set_idx);
    w_once = true;
    for (auto set_it = ret.first; set_it != ret.second; set_it++) {
        /*Check if data is there based on tag. Use global to L2 latency to filter concurrent requests*/
        if(set_it->second->getTag() == up_it->tag && set_it->second->getTimeAccessed() + m_mem_latency <= up_it->timestamp){
            /*Given sectored cachelines, check for full or partial hit*/
            if(set_it->second->getS0Filled() == up_it->s0 && set_it->second->getS1Filled() == up_it->s1 &&
               set_it->second->getS2Filled() == up_it->s2 && set_it->second->getS3Filled() == up_it->s3){

                int requests = up_it->s0_req + up_it->s1_req + up_it->s2_req + up_it->s3_req;
                m_stat.recordWriteHits(requests);
                m_stat.recordMemoryRequests(requests);
                set_it->second->setAge(0);
                set_it->second->displayFullHit();
            } else {
                /*Partial hit*/
                if (up_it->add_low < set_it->second->getAddressLow()) { set_it->second->setAddressLow(up_it->add_low); }
                if (up_it->add_high > set_it->second->getAddressHigh()) { set_it->second->setAddressHigh(up_it->add_high); }
                if (up_it->idx_low < set_it->second->getIdxLow()) { set_it->second->setIdxLow(up_it->idx_low); }
                if (up_it->idx_high > set_it->second->getIdxHigh()) { set_it->second->setIdxHigh(up_it->idx_high); }
                if (!set_it->second->getS0Filled() && up_it->s0){
                    set_it->second->setS0Filled(true);
                    m_stat.recordWriteMisses(up_it->s0_req);
                } else {
                    m_stat.recordWriteHits(up_it->s0_req);
                }

                if (!set_it->second->getS1Filled() && up_it->s1){
                    set_it->second->setS1Filled(true);
                    m_stat.recordWriteMisses(up_it->s1_req);
                } else {
                    m_stat.recordWriteHits(up_it->s1_req);
                }

                if (!set_it->second->getS2Filled() && up_it->s2){
                    set_it->second->setS2Filled(true);
                    m_stat.recordWriteMisses(up_it->s2_req);
                } else {
                    m_stat.recordWriteHits(up_it->s2_req);
                }

                if (!set_it->second->getS3Filled() && up_it->s3){
                    set_it->second->setS3Filled(true);
                    m_stat.recordWriteMisses(up_it->s3_req);
                } else {
                    m_stat.recordWriteHits(up_it->s3_req);
                }

                int mem_req = up_it->s0_req + up_it->s1_req + up_it->s2_req + up_it->s3_req;
                m_stat.recordMemoryRequests(mem_req);
                set_it->second->setDataStructure(up_it->name);
                set_it->second->setAge(0);
                set_it->second->displayPartialHit();

            }//end partial / full
            data_found = true;
            hit_tag = up_it->tag;
        }
        /*While looking at the set, also check if there is an empty spot for a cacheline*/
        if (set_it->second->getIsEmpty()){
            empty_cline = true;
        }
        /*Moreover, check for the oldest cacheline in the set*/
        if (set_it->second->getAge() > oldest_age){
            oldest_age = set_it->second->getAge();
            oldest_tag = set_it->second->getTag();
        }

    }

    /*Finally, if the data is found, make all other cachelines older by 1*/
    if (data_found){
        for (auto set_it = ret.first; set_it != ret.second; set_it++) {
            if (set_it->second->getTag() != hit_tag){
                set_it->second->incAge();
            }
        }
    }

}

/*If the data was not found on the initial lookup & an empty cacheline was detected,
  this method inserts the data into that empty spot.*/
void Cache::writeLoadOnEmpty(bool &data_found, bool &empty_cline, bool &w_once,
                             std::pair<std::multimap<int,Cacheline*>::iterator, std::multimap<int,Cacheline*>::iterator>& ret,
                             std::vector<update_cline_info<unsigned int, unsigned long long>>::iterator &up_it)
{
    if (!data_found && empty_cline){
        for (auto set_it = ret.first; set_it != ret.second; set_it++) {
            if (set_it->second->getIsEmpty() && w_once && set_it->second->getTag() == 0){
                /*Update the cacheline*/
                set_it->second->setTag(up_it->tag);
                set_it->second->setAddressLow(up_it->add_low);
                set_it->second->setAddressHigh(up_it->add_high);
                set_it->second->setIdxLow(up_it->idx_low);
                set_it->second->setIdxHigh(up_it->idx_high);
                set_it->second->setDataStructure(up_it->name);
                set_it->second->setAge(0);
                set_it->second->setS0Filled(up_it->s0);
                set_it->second->setS1Filled(up_it->s1);
                set_it->second->setS2Filled(up_it->s2);
                set_it->second->setS3Filled(up_it->s3);
                set_it->second->displayEviction();
                w_once = false;
                /*Update statistics*/
                int requests = up_it->s0_req + up_it->s1_req + up_it->s2_req + up_it->s3_req;
                m_stat.recordMemoryRequests(requests);
                m_stat.recordWriteMisses(requests);
            } else {
                set_it->second->incAge();
            }
        }
    }

}

/*If the data  was not found on the initial lookup & the cache is already full,
  this method uses the specified evection policy to remove a cacheline and load the data*/
void Cache::writeLoadEviction(bool &data_found, bool &empty_cline, int &oldest_tag,
                              std::pair<std::multimap<int,Cacheline*>::iterator, std::multimap<int,Cacheline*>::iterator>& ret,
                              std::vector<update_cline_info<unsigned int, unsigned long long>>::iterator &up_it)
{
    if (!data_found && !empty_cline){
        if (m_rep_policy == LRU){
            for (auto set_it = ret.first; set_it != ret.second; set_it++) {
                /*Evict the oldest*/
                if (set_it->second->getTag() == oldest_tag){
                    set_it->second->setTag(up_it->tag);
                    set_it->second->setAddressLow(up_it->add_low);
                    set_it->second->setAddressHigh(up_it->add_high);
                    set_it->second->setIdxLow(up_it->idx_low);
                    set_it->second->setIdxHigh(up_it->idx_high);
                    set_it->second->setDataStructure(up_it->name);
                    set_it->second->setAge(0);
                    set_it->second->setS0Filled(up_it->s0);
                    set_it->second->setS1Filled(up_it->s1);
                    set_it->second->setS2Filled(up_it->s2);
                    set_it->second->setS3Filled(up_it->s3);
                    set_it->second->displayEviction();
                    /*Update statistics*/
                    int requests = up_it->s0_req + up_it->s1_req + up_it->s2_req + up_it->s3_req;
                    m_stat.recordMemoryRequests(requests);
                    m_stat.recordWriteMisses(requests);
                } else {
                    set_it->second->incAge();
                }
            }
        } else if (m_rep_policy == RANDOM){
            int random_id = this->getRandomNumber();
            int counter = 0;

            for (auto set_it = ret.first; set_it != ret.second; set_it) {
                counter++;
                if (random_id == counter){
                    set_it->second->setTag(up_it->tag);
                    set_it->second->setAddressLow(up_it->add_low);
                    set_it->second->setAddressHigh(up_it->add_high);
                    set_it->second->setIdxLow(up_it->idx_low);
                    set_it->second->setIdxHigh(up_it->idx_high);
                    set_it->second->setDataStructure(up_it->name);
                    set_it->second->setAge(0);
                    set_it->second->setS0Filled(up_it->s0);
                    set_it->second->setS1Filled(up_it->s1);
                    set_it->second->setS2Filled(up_it->s2);
                    set_it->second->setS3Filled(up_it->s3);
                    set_it->second->displayEviction();
                    /*Update statistics*/
                    int requests = up_it->s0_req + up_it->s1_req + up_it->s2_req + up_it->s3_req;
                    m_stat.recordMemoryRequests(requests);
                    m_stat.recordWriteMisses(requests);
                } else {
                    set_it->second->incAge();
                }
            }
        }
    }
}


/*Constructors (2)*/
Cache::Cache(const std::string &in_name, bool in_sec, ReplacementPolicy in_rep, MemWritePolicy in_mem_w, SimulationStatistics* stat)
    : m_name(in_name), m_is_sectored(in_sec), m_rep_policy(in_rep), m_w_policy(in_mem_w), m_stat(*stat)
{
    m_mem_latency = static_cast<unsigned long long>(Settings::getGlobalToCacheLatency());
}


Cache::Cache(const std::string &in_name, int in_size, int in_way, int in_sets, bool in_sec, ReplacementPolicy in_rep, MemWritePolicy in_mem_w, SimulationStatistics* stat)
    : m_name(in_name), m_size_bytes(in_size), m_way_size(in_way), m_num_sets(in_sets), m_is_sectored(in_sec), m_rep_policy(in_rep), m_w_policy(in_mem_w), m_stat(*stat)
{
    m_mem_latency = static_cast<unsigned long long>(Settings::getGlobalToCacheLatency());
}



/*Accepts a reference from a Bock container and decides what to do with it based on its state / Operation*/
void Cache::processUpdate(std::vector<update_cline_info<unsigned int, unsigned long long> > &up_info)
{
    OperationType operation;
    if(up_info.empty()){
        qDebug("Update Info Reference passed empty to Cache object...\n");
        return;
    } else {
        if(up_info.front().set_idx == NORET && up_info.front().tag == NORET && up_info.front().name == 'Z'){
            qDebug("Received NORET from Block\n");
            return;
        }
        operation = up_info.front().oper;
    }

    if (operation == READ){
        this->processRead(up_info);
    } else if (operation == WRITE) {
        this->processWrite(up_info);
    }

}

void Cache::addCacheline(int set_index, Cacheline *cline)
{
    m_idx_map.emplace(set_index, cline);
}

void Cache::resetState()
{
    for (auto it = m_idx_map.begin(); it != m_idx_map.end(); ++it) {
        it->second->setAge(0);
        it->second->setTag(0);
        it->second->setIdxLow(0);
        it->second->setIdxHigh(0);
        it->second->setAddressLow(0);
        it->second->setAddressHigh(0);
        it->second->flipAllSectors(false);
        it->second->setBlockOffset(0);
        it->second->setIsEmpty(true);
        it->second->setTimeAccessed(0);
        it->second->setDataStructure('D');
        it->second->resetColor();
    }
}

void Cache::cleanAll()
{
    /*Since the QGraphicsScene takes ownership of the cacheline objects
      this method should not attempt to call delete on them, given that
      scene->clear() is called by the visualizer on clear.*/
    if(!m_idx_map.empty()) { m_idx_map.clear();}
}

void Cache::resetAllMembers(const std::string &in_name, int in_size, int in_way, int in_sets, bool in_sec, ReplacementPolicy in_rep, MemWritePolicy in_mem_w, SimulationStatistics *stat)
{
    m_name = in_name;
    m_size_bytes = in_size;
    m_way_size = in_way;
    m_num_sets = in_sets;
    m_is_sectored = in_sec;
    m_rep_policy = in_rep;
    m_w_policy = in_mem_w;
    m_mem_latency = static_cast<unsigned long long>(Settings::getGlobalToCacheLatency());
    m_stat = *stat;
}

/*Setters & Getters*/
std::string Cache::getName() const
{
    return m_name;
}

void Cache::setName(const std::string &name)
{
    m_name = name;
}

int Cache::getSizeBytes() const
{
    return m_size_bytes;
}

void Cache::setSizeBytes(int size_bytes)
{
    m_size_bytes = size_bytes;
}

int Cache::getWaySize() const
{
    return m_way_size;
}

void Cache::setWaySize(int way_size)
{
    m_way_size = way_size;
}

int Cache::getNumSets() const
{
    return m_num_sets;
}

void Cache::setNumSets(int num_sets)
{
    m_num_sets = num_sets;
}

bool Cache::getIsSectored() const
{
    return m_is_sectored;
}

void Cache::setIsSectored(bool is_sectored)
{
    m_is_sectored = is_sectored;
}

ReplacementPolicy Cache::getRepPolicy() const
{
    return m_rep_policy;
}

void Cache::setRepPolicy(const ReplacementPolicy &rep_policy)
{
    m_rep_policy = rep_policy;
}

MemWritePolicy Cache::getWPolicy() const
{
    return m_w_policy;
}

void Cache::setWPolicy(const MemWritePolicy &w_policy)
{
    m_w_policy = w_policy;
}

unsigned long long Cache::getMemLatency() const
{
    return m_mem_latency;
}
