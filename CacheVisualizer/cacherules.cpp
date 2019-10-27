/*Functions implementing cache functionality(checking if a mem request is a hit/miss, handling eviction, etc...)*/
#include "cacherules.h"
#include "globals.h"
#include "statistics.h"

#include <list>
#include <map>
#include <utility>
#include <QColor>
#include <QObject>
#include <random>

cacherules::cacherules()
{
}


void updateSceneFromInfo(std::list<update_line_info> up_info, statistics *stat_out){
    bool found_data_on_read = false, found_data_on_write = false, detected_empy_cline = false;
    bool read_once = true, write_once = true;
    int oldest_age = INT_MIN, oldest_tag = 0;
    int hit_tag = 0;
    std::pair <std::multimap<int,cline_info>::iterator, std::multimap<int,cline_info>::iterator> ret;

    if(up_info.empty()){
        qDebug("Update Line Passed empty to cache rules\n");
    }

    for (auto it = up_info.begin(); it != up_info.end(); it++) { //For all the update entries returned by the threadblock
        //stat_out->incMemRequests();
        if(it->oper == READ){
            read_once = true;
            ret = idx_map.equal_range(it->set_idx);
            for (auto set_it = ret.first; set_it != ret.second; set_it++) {
                if(set_it->second.cline_ptr->getTag() == it->tag && set_it->second.cline_ptr->getCycles() + global_latency <= it->cycles){ // Cache hit
                    //Check if it is a partial hit or full hit
                    if(set_it->second.cline_ptr->getSector_one_filled() == it->s0 && set_it->second.cline_ptr->getSector_two_filled() == it->s1
                            && set_it->second.cline_ptr->getSector_three_filled() == it->s2 && set_it->second.cline_ptr->getSector_four_filled() == it->s3){
                        int requests = it->s0_req;
                        requests += it->s1_req;
                        requests += it->s2_req;
                        requests += it->s3_req;
                        stat_out->recordReadHits(requests);
                        stat_out->recordMemoryRequests(requests);
                        set_it->second.cline_ptr->setAge(0);
                    } else {
                        if (it->add_low < set_it->second.cline_ptr->getAddressLow()) {set_it->second.cline_ptr->setAddressLow(it->add_low);}
                        if (it->add_high > set_it->second.cline_ptr->getAddressHigh()) {set_it->second.cline_ptr->setAddressHigh(it->add_high);}
                        if (it->idx_low < set_it->second.cline_ptr->getIdxLow()) {set_it->second.cline_ptr->setIdxLow(it->idx_low);}
                        if (it->idx_high > set_it->second.cline_ptr->getIdxHigh()) {set_it->second.cline_ptr->setIdxHigh(it->idx_high);}
                        if (!set_it->second.cline_ptr->getSector_one_filled() && it->s0){
                            set_it->second.cline_ptr->setSector_one_filled(true);
                            stat_out->recordReadMisses(it->s0_req);
                        } else {
                            stat_out->recordReadHits(it->s0_req);
                        }

                        if (!set_it->second.cline_ptr->getSector_two_filled() && it->s1){
                            set_it->second.cline_ptr->setSector_two_filled(true);
                            stat_out->recordReadMisses(it->s1_req);
                        } else {
                            stat_out->recordReadHits(it->s1_req);
                        }
                        if (!set_it->second.cline_ptr->getSector_three_filled() && it->s2){
                            set_it->second.cline_ptr->setSector_three_filled(true);
                            stat_out->recordReadMisses(it->s2_req);
                        } else {
                            stat_out->recordReadHits(it->s2_req);
                        }
                        if (!set_it->second.cline_ptr->getSector_four_filled() && it->s3){
                            set_it->second.cline_ptr->setSector_four_filled(true);
                            stat_out->recordReadMisses(it->s3_req);
                        } else {
                            stat_out->recordReadHits(it->s3_req);
                        }
                        stat_out->recordMemoryRequests(it->s0_req);
                        stat_out->recordMemoryRequests(it->s1_req);
                        stat_out->recordMemoryRequests(it->s2_req);
                        stat_out->recordMemoryRequests(it->s3_req);
                        set_it->second.cline_ptr->setDataStructure(it->name);
                        set_it->second.cline_ptr->setAge(0);
                        set_it->second.age = 0;
                        set_it->second.tag = it->tag;
                        set_it->second.cline_ptr->displayPartialHit();
                    }
                    found_data_on_read = true;
                    hit_tag = it->tag;
                }
                if(set_it->second.cline_ptr->getIs_empty()){//Check if any of the lines is empty
                    detected_empy_cline = true;
                }
                if(set_it->second.cline_ptr->getAge() > oldest_age){ //Get the oldest
                    oldest_age = set_it->second.cline_ptr->getAge();
                    oldest_tag = set_it->second.cline_ptr->getTag();
                }
            }

            if(found_data_on_read){
                for (auto h_it = ret.first; h_it != ret.second; h_it++) {
                    if(h_it->second.cline_ptr->getTag() != hit_tag){
                        h_it->second.cline_ptr->incAge();
                        h_it->second.age++;
                    }
                }
            }


            if(!found_data_on_read && detected_empy_cline){ //Data was not found, so pick the first empty cline from set and fill it
                for (auto r_it = ret.first; r_it != ret.second; r_it++) {
                    if(r_it->second.cline_ptr->getIs_empty() && read_once && r_it->second.cline_ptr->getTag() == 0){
                        int requests = it->s0_req;
                        requests += it->s1_req;
                        requests += it->s2_req;
                        requests += it->s3_req;
                        stat_out->recordReadMisses(requests);
                        stat_out->recordMemoryRequests(requests);
                        r_it->second.cline_ptr->setTag(it->tag);
                        r_it->second.cline_ptr->setAddressLow(it->add_low);
                        r_it->second.cline_ptr->setAddressHigh(it->add_high);
                        r_it->second.cline_ptr->setIdxLow(it->idx_low);
                        r_it->second.cline_ptr->setIdxHigh(it->idx_high);
                        r_it->second.cline_ptr->setDataStructure(it->name);
                        r_it->second.cline_ptr->setAge(0);
                        r_it->second.age = 0; //!!decide on if to keep in struct
                        r_it->second.tag = it->tag; //!!decide on if to keep in struct
                        r_it->second.cline_ptr->setSector_one_filled(it->s0);
                        r_it->second.cline_ptr->setSector_two_filled(it->s1);
                        r_it->second.cline_ptr->setSector_three_filled(it->s2);
                        r_it->second.cline_ptr->setSector_four_filled(it->s3);
                        r_it->second.cline_ptr->displayEviction();
                        read_once = false;
                    } else {
                        r_it->second.cline_ptr->incAge();
                        r_it->second.age++;
                    }
                }//end set loop
            } else if (!found_data_on_read && !detected_empy_cline) { //Data not found and the set is full, so we have to evict
                if(replacement_policy == 1){//LRU
                    for (auto re_it = ret.first; re_it != ret.second; re_it++) {
                        if(re_it->second.cline_ptr->getTag() == oldest_tag){
                            int requests = it->s0_req;
                            requests += it->s1_req;
                            requests += it->s2_req;
                            requests += it->s3_req;
                            stat_out->recordReadMisses(requests);
                            stat_out->recordMemoryRequests(requests);
                            re_it->second.cline_ptr->setTag(it->tag);
                            re_it->second.cline_ptr->setAddressLow(it->add_low);
                            re_it->second.cline_ptr->setAddressHigh(it->add_high);
                            re_it->second.cline_ptr->setIdxLow(it->idx_low);
                            re_it->second.cline_ptr->setIdxHigh(it->idx_high);
                            re_it->second.cline_ptr->setDataStructure(it->name);
                            re_it->second.cline_ptr->setAge(0);
                            re_it->second.age = 0;
                            re_it->second.tag = it->tag;
                            re_it->second.cline_ptr->setSector_one_filled(it->s0);
                            re_it->second.cline_ptr->setSector_two_filled(it->s1);
                            re_it->second.cline_ptr->setSector_three_filled(it->s2);
                            re_it->second.cline_ptr->setSector_four_filled(it->s3);
                            re_it->second.cline_ptr->displayEviction();
                        } else {
                            re_it->second.cline_ptr->incAge();
                            re_it->second.age++;
                        }
                    }

                } else if(replacement_policy == 2){
                    std::random_device rd;
                    std::mt19937 rng(rd());
                    std::uniform_int_distribution<int> uni(1,way_size_l2);
                    auto random_integer = uni(rng);
                    int it_counter = 0;
                    for (auto re_it = ret.first; re_it != ret.second; re_it++) {
                        it_counter++;
                        if(random_integer == it_counter){
                            int requests = it->s0_req;
                            requests += it->s1_req;
                            requests += it->s2_req;
                            requests += it->s3_req;
                            stat_out->recordReadMisses(requests);
                            stat_out->recordMemoryRequests(requests);
                            re_it->second.cline_ptr->setTag(it->tag);
                            re_it->second.cline_ptr->setAddressLow(it->add_low);
                            re_it->second.cline_ptr->setAddressHigh(it->add_high);
                            re_it->second.cline_ptr->setIdxLow(it->idx_low);
                            re_it->second.cline_ptr->setIdxHigh(it->idx_high);
                            re_it->second.cline_ptr->setDataStructure(it->name);
                            re_it->second.cline_ptr->setAge(0);
                            re_it->second.age = 0;
                            re_it->second.tag = it->tag;
                            re_it->second.cline_ptr->setSector_one_filled(it->s0);
                            re_it->second.cline_ptr->setSector_two_filled(it->s1);
                            re_it->second.cline_ptr->setSector_three_filled(it->s2);
                            re_it->second.cline_ptr->setSector_four_filled(it->s3);
                            re_it->second.cline_ptr->displayEviction();
                        } else {
                            re_it->second.cline_ptr->incAge();
                            re_it->second.age++;
                        }
                    }

                }
            }

        } else if (it->oper == WRITE){
            write_once = true;
            ret = idx_map.equal_range(it->set_idx);
            for (auto s_it = ret.first; s_it != ret.second; s_it++) {
                if(s_it->second.cline_ptr->getTag() == it->tag){ //write hit
                    if(s_it->second.cline_ptr->getSector_one_filled() == it->s0 && s_it->second.cline_ptr->getSector_two_filled() == it->s1
                            && s_it->second.cline_ptr->getSector_three_filled() == it->s2 && s_it->second.cline_ptr->getSector_four_filled() == it->s3){
                        int requests = it->s0_req;
                        requests += it->s1_req;
                        requests += it->s2_req;
                        requests += it->s3_req;
                        stat_out->recordWriteHits(requests);
                        stat_out->recordMemoryRequests(requests);
                        s_it->second.cline_ptr->setAge(0);
                        s_it->second.cline_ptr->displayFullHit();
                    } else {
                        if (it->add_low < s_it->second.cline_ptr->getAddressLow()) {s_it->second.cline_ptr->setAddressLow(it->add_low);}
                        if (it->add_high > s_it->second.cline_ptr->getAddressHigh()) {s_it->second.cline_ptr->setAddressHigh(it->add_high);}
                        if (it->idx_low < s_it->second.cline_ptr->getIdxLow()) {s_it->second.cline_ptr->setIdxLow(it->idx_low);}
                        if (it->idx_high > s_it->second.cline_ptr->getIdxHigh()) {s_it->second.cline_ptr->setIdxHigh(it->idx_high);}
                        if (!s_it->second.cline_ptr->getSector_one_filled() && it->s0){
                            s_it->second.cline_ptr->setSector_one_filled(true);
                            stat_out->recordWriteMisses(it->s0_req);
                        } else {
                            stat_out->recordWriteHits(it->s0_req);
                        }

                        if (!s_it->second.cline_ptr->getSector_two_filled() && it->s1){
                            s_it->second.cline_ptr->setSector_two_filled(true);
                            stat_out->recordWriteMisses(it->s1_req);
                        } else {
                            stat_out->recordWriteHits(it->s1_req);
                        }

                        if (!s_it->second.cline_ptr->getSector_three_filled() && it->s2){
                            s_it->second.cline_ptr->setSector_three_filled(true);
                            stat_out->recordWriteMisses(it->s2_req);
                        } else {
                            stat_out->recordWriteHits(it->s2_req);
                        }

                        if (!s_it->second.cline_ptr->getSector_four_filled() && it->s3){
                            s_it->second.cline_ptr->setSector_four_filled(true);
                            stat_out->recordWriteMisses(it->s3_req);
                        } else {
                            stat_out->recordWriteHits(it->s3_req);
                        }
                        s_it->second.cline_ptr->setDataStructure(it->name);
                        s_it->second.cline_ptr->setAge(0);
                        s_it->second.age = 0;
                        s_it->second.tag = it->tag;
                        s_it->second.cline_ptr->displayPartialHit();
                        stat_out->recordMemoryRequests(it->s0_req);
                        stat_out->recordMemoryRequests(it->s1_req);
                        stat_out->recordMemoryRequests(it->s2_req);
                        stat_out->recordMemoryRequests(it->s3_req);
                    }
                    found_data_on_write = true;
                    hit_tag = it->tag;
                }
                if(s_it->second.cline_ptr->getIs_empty()){
                    detected_empy_cline = true;
                }
                if(s_it->second.cline_ptr->getAge() > oldest_age){
                    oldest_age = s_it->second.cline_ptr->getAge();
                    oldest_tag = s_it->second.cline_ptr->getTag();
                }
            }

            if(found_data_on_write){
                for (auto hi_it = ret.first; hi_it != ret.second; hi_it++) {
                    if(hi_it->second.cline_ptr->getTag() != hit_tag){
                        hi_it->second.cline_ptr->incAge();
                        hi_it->second.age++;
                    }
                }
            }

            if(!found_data_on_write && detected_empy_cline){
                for (auto w_it = ret.first; w_it != ret.second; w_it++) {
                    if(w_it->second.cline_ptr->getIs_empty() && write_once && w_it->second.cline_ptr->getTag() == 0){
                        int requests = it->s0_req;
                        requests += it->s1_req;
                        requests += it->s2_req;
                        requests += it->s3_req;
                        stat_out->recordWriteMisses(requests);
                        stat_out->recordMemoryRequests(requests);
                        w_it->second.cline_ptr->setTag(it->tag);
                        w_it->second.cline_ptr->setAddressLow(it->add_low);
                        w_it->second.cline_ptr->setAddressHigh(it->add_high);
                        w_it->second.cline_ptr->setIdxLow(it->idx_low);
                        w_it->second.cline_ptr->setIdxHigh(it->idx_high);
                        w_it->second.cline_ptr->setDataStructure(it->name);
                        w_it->second.cline_ptr->setAge(0);
                        w_it->second.age = 0;
                        w_it->second.tag = it->tag;
                        w_it->second.cline_ptr->setSector_one_filled(it->s0);
                        w_it->second.cline_ptr->setSector_two_filled(it->s1);
                        w_it->second.cline_ptr->setSector_three_filled(it->s2);
                        w_it->second.cline_ptr->setSector_four_filled(it->s3);
                        w_it->second.cline_ptr->displayEviction();
                        write_once = false;
                    } else {
                        w_it->second.cline_ptr->incAge();
                        w_it->second.age++;
                    }
                }

            } else if (!found_data_on_write && !detected_empy_cline) {
                if (replacement_policy == 1){
                    for (auto we_it = ret.first; we_it != ret.second; we_it++) {
                        if(we_it->second.cline_ptr->getTag() == oldest_tag){
                            int requests = it->s0_req;
                            requests += it->s1_req;
                            requests += it->s2_req;
                            requests += it->s3_req;
                            stat_out->recordWriteMisses(requests);
                            stat_out->recordMemoryRequests(requests);
                            we_it->second.cline_ptr->setTag(it->tag);
                            we_it->second.cline_ptr->setAddressLow(it->add_low);
                            we_it->second.cline_ptr->setAddressHigh(it->add_high);
                            we_it->second.cline_ptr->setIdxLow(it->idx_low);
                            we_it->second.cline_ptr->setIdxHigh(it->idx_high);
                            we_it->second.cline_ptr->setDataStructure(it->name);
                            we_it->second.cline_ptr->setAge(0);
                            we_it->second.age = 0;
                            we_it->second.tag = it->tag;
                            we_it->second.cline_ptr->setSector_one_filled(it->s0);
                            we_it->second.cline_ptr->setSector_two_filled(it->s1);
                            we_it->second.cline_ptr->setSector_three_filled(it->s2);
                            we_it->second.cline_ptr->setSector_four_filled(it->s3);
                            we_it->second.cline_ptr->displayEviction();
                        } else {
                            we_it->second.cline_ptr->incAge();
                            we_it->second.age++;
                        }
                    }
                } else if(replacement_policy == 2){
                    std::random_device rd;
                    std::mt19937 rng(rd());
                    std::uniform_int_distribution<int> uni(1,way_size_l2);
                    auto random_integer = uni(rng);
                    int it_counter = 0;
                    for (auto we_it = ret.first; we_it != ret.second; we_it++) {
                        it_counter++;
                        if(it_counter == random_integer){
                            int requests = it->s0_req;
                            requests += it->s1_req;
                            requests += it->s2_req;
                            requests += it->s3_req;
                            stat_out->recordWriteMisses(requests);
                            stat_out->recordMemoryRequests(requests);
                            we_it->second.cline_ptr->setTag(it->tag);
                            we_it->second.cline_ptr->setAddressLow(it->add_low);
                            we_it->second.cline_ptr->setAddressHigh(it->add_high);
                            we_it->second.cline_ptr->setIdxLow(it->idx_low);
                            we_it->second.cline_ptr->setIdxHigh(it->idx_high);
                            we_it->second.cline_ptr->setDataStructure(it->name);
                            we_it->second.cline_ptr->setAge(0);
                            we_it->second.age = 0;
                            we_it->second.tag = it->tag;
                            we_it->second.cline_ptr->setSector_one_filled(it->s0);
                            we_it->second.cline_ptr->setSector_two_filled(it->s1);
                            we_it->second.cline_ptr->setSector_three_filled(it->s2);
                            we_it->second.cline_ptr->setSector_four_filled(it->s3);
                            we_it->second.cline_ptr->displayEviction();
                        } else {
                            we_it->second.cline_ptr->incAge();
                            we_it->second.age++;
                        }
                    }
                }//end replacement
            }
        }//end if WRITE
    }
}

void updateSceneFromInfoNonSectored(std::list<update_line_info> up_info, statistics *stat_out){
    bool found_data_on_read = false, found_data_on_write = false, detected_empy_cline = false;
    bool read_once = true, write_once = true;
    int oldest_age = INT_MIN, oldest_tag = 0;
    int hit_tag = 0;
    std::pair <std::multimap<int,cline_info>::iterator, std::multimap<int,cline_info>::iterator> ret;

    if(up_info.empty()){
        qDebug("Update Line Passed empty to cache rules\n");
    }

    for (auto it = up_info.begin(); it != up_info.end(); it++) { //For all the update entry returned by the threadblock
        if(it->oper == READ){
            read_once = true;
            ret = idx_map.equal_range(it->set_idx);
            for (auto set_it = ret.first; set_it != ret.second; set_it++) {
                if(set_it->second.cline_ptr->getTag() == it->tag && set_it->second.cline_ptr->getCycles() + global_latency <= it->cycles){ // Cache hit
                    int requests = it->s0_req;
                    requests += it->s1_req;
                    requests += it->s2_req;
                    requests += it->s3_req;
                    stat_out->recordReadHits(requests);
                    stat_out->recordMemoryRequests(requests);
                    if (it->add_low < set_it->second.cline_ptr->getAddressLow()) {set_it->second.cline_ptr->setAddressLow(it->add_low);}
                    if (it->add_high > set_it->second.cline_ptr->getAddressHigh()) {set_it->second.cline_ptr->setAddressHigh(it->add_high);}
                    if (it->idx_low < set_it->second.cline_ptr->getIdxLow()) {set_it->second.cline_ptr->setIdxLow(it->idx_low);}
                    if (it->idx_high > set_it->second.cline_ptr->getIdxHigh()) {set_it->second.cline_ptr->setIdxHigh(it->idx_high);}
                    set_it->second.cline_ptr->setDataStructure(it->name);
                    set_it->second.cline_ptr->setAge(0);
                    set_it->second.age = 0;
                    set_it->second.tag = it->tag;
                    set_it->second.cline_ptr->displayFullHit();
                    found_data_on_read = true;
                    hit_tag = it->tag;
                }
                if(set_it->second.cline_ptr->getIs_empty()){//Check if any of the lines is empty
                    detected_empy_cline = true;
                }
                if(set_it->second.cline_ptr->getAge() > oldest_age){ //Get the oldest
                    oldest_age = set_it->second.cline_ptr->getAge();
                    oldest_tag = set_it->second.cline_ptr->getTag();
                }
            }

            if(found_data_on_read){
                for (auto h_it = ret.first; h_it != ret.second; h_it++) {
                    if(h_it->second.cline_ptr->getTag() != hit_tag){
                        h_it->second.cline_ptr->incAge();
                        h_it->second.age++;
                    }
                }
            }

            if(!found_data_on_read && detected_empy_cline){ //Data was not found, so pick the first empty cline from set and fill it
                for (auto r_it = ret.first; r_it != ret.second; r_it++) {
                    if(r_it->second.cline_ptr->getIs_empty() && read_once && r_it->second.cline_ptr->getTag() == 0){
                        int requests = it->s0_req;
                        requests += it->s1_req;
                        requests += it->s2_req;
                        requests += it->s3_req;
                        stat_out->recordReadMisses(requests);
                        stat_out->recordMemoryRequests(requests);
                        r_it->second.cline_ptr->setTag(it->tag);
                        r_it->second.cline_ptr->setAddressLow(it->add_low);
                        r_it->second.cline_ptr->setAddressHigh(it->add_high);
                        r_it->second.cline_ptr->setIdxLow(it->idx_low);
                        r_it->second.cline_ptr->setIdxHigh(it->idx_high);
                        r_it->second.cline_ptr->setDataStructure(it->name);
                        r_it->second.cline_ptr->setAge(0);
                        r_it->second.age = 0;
                        r_it->second.tag = it->tag;
                        r_it->second.cline_ptr->displayEviction();
                        read_once = false;
                    } else {
                        r_it->second.cline_ptr->incAge();
                        r_it->second.age++;
                    }
                }//end set loop
            } else if (!found_data_on_read && !detected_empy_cline) { //Data not found and the set is full, so we have to evict
                if(replacement_policy == 1){//LRU
                    for (auto re_it = ret.first; re_it != ret.second; re_it++) {
                        if(re_it->second.cline_ptr->getTag() == oldest_tag){
                            int requests = it->s0_req;
                            requests += it->s1_req;
                            requests += it->s2_req;
                            requests += it->s3_req;
                            stat_out->recordReadMisses(requests);
                            stat_out->recordMemoryRequests(requests);
                            re_it->second.cline_ptr->setTag(it->tag);
                            re_it->second.cline_ptr->setAddressLow(it->add_low);
                            re_it->second.cline_ptr->setAddressHigh(it->add_high);
                            re_it->second.cline_ptr->setIdxLow(it->idx_low);
                            re_it->second.cline_ptr->setIdxHigh(it->idx_high);
                            re_it->second.cline_ptr->setDataStructure(it->name);
                            re_it->second.cline_ptr->setAge(0);
                            re_it->second.age = 0;
                            re_it->second.tag = it->tag;
                            re_it->second.cline_ptr->displayEviction();
                        } else {
                            re_it->second.cline_ptr->incAge();
                            re_it->second.age++;
                        }
                    }

                } else if(replacement_policy == 2){
                    std::random_device rd;
                    std::mt19937 rng(rd());
                    std::uniform_int_distribution<int> uni(1,way_size_l2);
                    auto random_integer = uni(rng);
                    int it_counter = 0;
                    for (auto re_it = ret.first; re_it != ret.second; re_it++) {
                        it_counter++;
                        if(random_integer == it_counter){
                            int requests = it->s0_req;
                            requests += it->s1_req;
                            requests += it->s2_req;
                            requests += it->s3_req;
                            stat_out->recordReadMisses(requests);
                            stat_out->recordMemoryRequests(requests);
                            re_it->second.cline_ptr->setTag(it->tag);
                            re_it->second.cline_ptr->setAddressLow(it->add_low);
                            re_it->second.cline_ptr->setAddressHigh(it->add_high);
                            re_it->second.cline_ptr->setIdxLow(it->idx_low);
                            re_it->second.cline_ptr->setIdxHigh(it->idx_high);
                            re_it->second.cline_ptr->setDataStructure(it->name);
                            re_it->second.cline_ptr->setAge(0);
                            re_it->second.age = 0;
                            re_it->second.tag = it->tag;
                            re_it->second.cline_ptr->displayEviction();
                        } else {
                            re_it->second.cline_ptr->incAge();
                            re_it->second.age++;
                        }
                    }
                }
            }

        } else if (it->oper == WRITE){
            write_once = true;
            ret = idx_map.equal_range(it->set_idx);
            for (auto s_it = ret.first; s_it != ret.second; s_it++) {
                if(s_it->second.cline_ptr->getTag() == it->tag){ //write hit
                    int requests = it->s0_req;
                    requests += it->s1_req;
                    requests += it->s2_req;
                    requests += it->s3_req;
                    stat_out->recordWriteHits(requests);
                    stat_out->recordMemoryRequests(requests);
                    if (it->add_low < s_it->second.cline_ptr->getAddressLow()) {s_it->second.cline_ptr->setAddressLow(it->add_low);}
                    if (it->add_high > s_it->second.cline_ptr->getAddressHigh()) {s_it->second.cline_ptr->setAddressHigh(it->add_high);}
                    if (it->idx_low < s_it->second.cline_ptr->getIdxLow()) {s_it->second.cline_ptr->setIdxLow(it->idx_low);}
                    if (it->idx_high > s_it->second.cline_ptr->getIdxHigh()) {s_it->second.cline_ptr->setIdxHigh(it->idx_high);}
                    //if (!s_it->second.cline_ptr->getSector_one_filled() && it->s0){s_it->second.cline_ptr->setSector_one_filled(true);}
                    //if (!s_it->second.cline_ptr->getSector_two_filled() && it->s1){s_it->second.cline_ptr->setSector_two_filled(true);}
                    //if (!s_it->second.cline_ptr->getSector_three_filled() && it->s2){s_it->second.cline_ptr->setSector_three_filled(true);}
                    //if (!s_it->second.cline_ptr->getSector_four_filled() && it->s3){s_it->second.cline_ptr->setSector_four_filled(true);}
                    s_it->second.cline_ptr->setSector_one_filled(true);
                    s_it->second.cline_ptr->setSector_two_filled(true);
                    s_it->second.cline_ptr->setSector_three_filled(true);
                    s_it->second.cline_ptr->setSector_four_filled(true);
                    s_it->second.cline_ptr->setDataStructure(it->name);
                    s_it->second.cline_ptr->setAge(0);
                    s_it->second.age = 0;
                    s_it->second.tag = it->tag;
                    s_it->second.cline_ptr->displayFullHit();
                    found_data_on_write = true;
                    hit_tag = it->tag;
                }
                if(s_it->second.cline_ptr->getIs_empty()){
                    detected_empy_cline = true;
                }
                if(s_it->second.cline_ptr->getAge() > oldest_age){
                    oldest_age = s_it->second.cline_ptr->getAge();
                    oldest_tag = s_it->second.cline_ptr->getTag();
                }
            }

            if(found_data_on_write){
                for (auto hi_it = ret.first; hi_it != ret.second; hi_it++) {
                    if(hi_it->second.cline_ptr->getTag() != hit_tag){
                        hi_it->second.cline_ptr->incAge();
                        hi_it->second.age++;
                    }
                }
            }

            if(!found_data_on_write && detected_empy_cline){
                for (auto w_it = ret.first; w_it != ret.second; w_it++) {
                    if(w_it->second.cline_ptr->getIs_empty() && write_once && w_it->second.cline_ptr->getTag() == 0){
                        int requests = it->s0_req;
                        requests += it->s1_req;
                        requests += it->s2_req;
                        requests += it->s3_req;
                        stat_out->recordWriteMisses(requests);
                        stat_out->recordMemoryRequests(requests);
                        w_it->second.cline_ptr->setTag(it->tag);
                        w_it->second.cline_ptr->setAddressLow(it->add_low);
                        w_it->second.cline_ptr->setAddressHigh(it->add_high);
                        w_it->second.cline_ptr->setIdxLow(it->idx_low);
                        w_it->second.cline_ptr->setIdxHigh(it->idx_high);
                        w_it->second.cline_ptr->setDataStructure(it->name);
                        w_it->second.cline_ptr->setAge(0);
                        w_it->second.age = 0;
                        w_it->second.tag = it->tag;
                        w_it->second.cline_ptr->displayEviction();
                        write_once = false;
                    } else {
                        w_it->second.cline_ptr->incAge();
                        w_it->second.age++;
                    }
                }

            } else if (!found_data_on_write && !detected_empy_cline) {
                if (replacement_policy == 1){
                    for (auto we_it = ret.first; we_it != ret.second; we_it++) {
                        if(we_it->second.cline_ptr->getTag() == oldest_tag){
                            int requests = it->s0_req;
                            requests += it->s1_req;
                            requests += it->s2_req;
                            requests += it->s3_req;
                            stat_out->recordWriteMisses(requests);
                            stat_out->recordMemoryRequests(requests);
                            we_it->second.cline_ptr->setTag(it->tag);
                            we_it->second.cline_ptr->setAddressLow(it->add_low);
                            we_it->second.cline_ptr->setAddressHigh(it->add_high);
                            we_it->second.cline_ptr->setIdxLow(it->idx_low);
                            we_it->second.cline_ptr->setIdxHigh(it->idx_high);
                            we_it->second.cline_ptr->setDataStructure(it->name);
                            we_it->second.cline_ptr->setAge(0);
                            we_it->second.age = 0;
                            we_it->second.tag = it->tag;
                            we_it->second.cline_ptr->displayEviction();
                        } else {
                            we_it->second.cline_ptr->incAge();
                            we_it->second.age++;
                        }
                    }
                } else if(replacement_policy == 2){
                    std::random_device rd;
                    std::mt19937 rng(rd());
                    std::uniform_int_distribution<int> uni(1,way_size_l2);
                    auto random_integer = uni(rng);
                    int it_counter = 0;
                    for (auto we_it = ret.first; we_it != ret.second; we_it++) {
                        it_counter++;
                        if(random_integer == it_counter){
                            int requests = it->s0_req;
                            requests += it->s1_req;
                            requests += it->s2_req;
                            requests += it->s3_req;
                            stat_out->recordWriteMisses(requests);
                            stat_out->recordMemoryRequests(requests);
                            we_it->second.cline_ptr->setTag(it->tag);
                            we_it->second.cline_ptr->setAddressLow(it->add_low);
                            we_it->second.cline_ptr->setAddressHigh(it->add_high);
                            we_it->second.cline_ptr->setIdxLow(it->idx_low);
                            we_it->second.cline_ptr->setIdxHigh(it->idx_high);
                            we_it->second.cline_ptr->setDataStructure(it->name);
                            we_it->second.cline_ptr->setAge(0);
                            we_it->second.age = 0;
                            we_it->second.tag = it->tag;
                            we_it->second.cline_ptr->displayEviction();
                        } else {
                            we_it->second.cline_ptr->incAge();
                            we_it->second.age++;
                        }
                    }
                }
            }
        }//end if WRITE
    }
}
