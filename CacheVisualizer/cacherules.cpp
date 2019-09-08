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


//!!TODO:Include latency(~200ms on R/W cache hits)
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
        stat_out->incMemRequests();
        if(it->oper == READ){
            read_once = true;
            ret = idx_map.equal_range(it->set_idx);
            for (auto set_it = ret.first; set_it != ret.second; set_it++) {
                if(set_it->second.cline_ptr->getTag() == it->tag){ // Cache hit
                    //Check if it is a partial hit
                    if(set_it->second.cline_ptr->getSector_one_filled() == it->s0 && set_it->second.cline_ptr->getSector_two_filled() == it->s1
                            && set_it->second.cline_ptr->getSector_three_filled() == it->s2 && set_it->second.cline_ptr->getSector_four_filled() == it->s3){
                        //Full hit
                        stat_out->recordReadFullHit();
                        set_it->second.cline_ptr->setAge(0);
                        //qDebug("Full Read Hit\n");
                    } else {
                        //qDebug("Partial Read Hit\n");
                        stat_out->recordReadPartialHit();
                        if (it->add_low < set_it->second.cline_ptr->getAddressLow()) {set_it->second.cline_ptr->setAddressLow(it->add_low);}
                        if (it->add_high > set_it->second.cline_ptr->getAddressHigh()) {set_it->second.cline_ptr->setAddressHigh(it->add_high);}
                        if (it->idx_low < set_it->second.cline_ptr->getIdxLow()) {set_it->second.cline_ptr->setIdxLow(it->idx_low);}
                        if (it->idx_high > set_it->second.cline_ptr->getIdxHigh()) {set_it->second.cline_ptr->setIdxHigh(it->idx_high);}
                        if (!set_it->second.cline_ptr->getSector_one_filled() && it->s0){set_it->second.cline_ptr->setSector_one_filled(true);}
                        if (!set_it->second.cline_ptr->getSector_two_filled() && it->s1){set_it->second.cline_ptr->setSector_two_filled(true);}
                        if (!set_it->second.cline_ptr->getSector_three_filled() && it->s2){set_it->second.cline_ptr->setSector_three_filled(true);}
                        if (!set_it->second.cline_ptr->getSector_four_filled() && it->s3){set_it->second.cline_ptr->setSector_four_filled(true);}
                        set_it->second.cline_ptr->setDataStructure(it->name); //Maybe not required
                        set_it->second.cline_ptr->setAge(0);
                        set_it->second.age = 0; //!!decide on if to keep in struct
                        set_it->second.tag = it->tag; //!!decide on if to keep in struct
                        QColor color(Qt::darkGreen);
                        set_it->second.cline_ptr->setColor(color);
                        set_it->second.cline_ptr->update();
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
                    if(r_it->second.cline_ptr->getIs_empty() && read_once){
                        qDebug("Read Miss\n");
                        stat_out->recordReadMiss();
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
                        QColor color(Qt::green);
                        r_it->second.cline_ptr->setColor(color);
                        r_it->second.cline_ptr->update();
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
                            qDebug("Read Miss + Evict\n");
                            stat_out->recordReadMiss();
                            re_it->second.cline_ptr->setTag(it->tag);
                            re_it->second.cline_ptr->setAddressLow(it->add_low);
                            re_it->second.cline_ptr->setAddressHigh(it->add_high);
                            re_it->second.cline_ptr->setIdxLow(it->idx_low);
                            re_it->second.cline_ptr->setIdxHigh(it->idx_high);
                            re_it->second.cline_ptr->setDataStructure(it->name);
                            re_it->second.cline_ptr->setAge(0);
                            re_it->second.age = 0; //!!decide on if to keep in struct
                            re_it->second.tag = it->tag; //!!decide on if to keep in struct
                            re_it->second.cline_ptr->setSector_one_filled(it->s0);
                            re_it->second.cline_ptr->setSector_two_filled(it->s1);
                            re_it->second.cline_ptr->setSector_three_filled(it->s2);
                            re_it->second.cline_ptr->setSector_four_filled(it->s3);
                            QColor color(Qt::yellow);
                            re_it->second.cline_ptr->setColor(color);
                            re_it->second.cline_ptr->update();
                        } else {
                            re_it->second.cline_ptr->incAge();
                            re_it->second.age++;
                        }
                    }

                } else if(replacement_policy == 2){
                    //!!consider random evict
                    std::random_device rd;
                    std::mt19937 rng(rd());
                    std::uniform_int_distribution<int> uni(0,way_size_l2); //!!check the iterator to decide on the range(0-5 or 1-6)
                    auto random_integer = uni(rng);
                    //...
                }
            }

        } else if (it->oper == WRITE){ //!!RETHINK the write scenarios
            write_once = true;
            ret = idx_map.equal_range(it->set_idx);
            for (auto s_it = ret.first; s_it != ret.second; s_it++) {
                if(s_it->second.cline_ptr->getTag() == it->tag){ //write hit
                    if(s_it->second.cline_ptr->getSector_one_filled() == it->s0 && s_it->second.cline_ptr->getSector_two_filled() == it->s1
                            && s_it->second.cline_ptr->getSector_three_filled() == it->s2 && s_it->second.cline_ptr->getSector_four_filled() == it->s3){
                        //Full hit
                        stat_out->recordWriteFullHit();
                        s_it->second.cline_ptr->setAge(0);
                        //qDebug("Full Write Hit\n");
                    } else {
                        //qDebug("Partial Write Hit\n");
                        stat_out->recordWritePartialHit();
                        if (it->add_low < s_it->second.cline_ptr->getAddressLow()) {s_it->second.cline_ptr->setAddressLow(it->add_low);}
                        if (it->add_high > s_it->second.cline_ptr->getAddressHigh()) {s_it->second.cline_ptr->setAddressHigh(it->add_high);}
                        if (it->idx_low < s_it->second.cline_ptr->getIdxLow()) {s_it->second.cline_ptr->setIdxLow(it->idx_low);}
                        if (it->idx_high > s_it->second.cline_ptr->getIdxHigh()) {s_it->second.cline_ptr->setIdxHigh(it->idx_high);}
                        if (!s_it->second.cline_ptr->getSector_one_filled() && it->s0){s_it->second.cline_ptr->setSector_one_filled(true);}
                        if (!s_it->second.cline_ptr->getSector_two_filled() && it->s1){s_it->second.cline_ptr->setSector_two_filled(true);}
                        if (!s_it->second.cline_ptr->getSector_three_filled() && it->s2){s_it->second.cline_ptr->setSector_three_filled(true);}
                        if (!s_it->second.cline_ptr->getSector_four_filled() && it->s3){s_it->second.cline_ptr->setSector_four_filled(true);}
                        //s_it->second.cline_ptr->setAddressLow(it->add_low);
                        //s_it->second.cline_ptr->setAddressHigh(it->add_high);
                        //s_it->second.cline_ptr->setIdxLow(it->idx_low);
                        //s_it->second.cline_ptr->setIdxHigh(it->idx_high);
                        s_it->second.cline_ptr->setDataStructure(it->name); //maybe not needed?
                        s_it->second.cline_ptr->setAge(0);
                        s_it->second.age = 0; //!!decide on if to keep in struct
                        s_it->second.tag = it->tag; //!!decide on if to keep in struct
                        //s_it->second.cline_ptr->setSector_one_filled(it->s0);
                        //s_it->second.cline_ptr->setSector_two_filled(it->s1);
                        //s_it->second.cline_ptr->setSector_three_filled(it->s2);
                        //s_it->second.cline_ptr->setSector_four_filled(it->s3);
                        QColor color(Qt::green);
                        s_it->second.cline_ptr->setColor(color);
                        s_it->second.cline_ptr->update();
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
                    if(w_it->second.cline_ptr->getIs_empty() && write_once){
                        //qDebug("Write Miss\n");
                        stat_out->recordWriteMiss();
                        w_it->second.cline_ptr->setTag(it->tag);
                        w_it->second.cline_ptr->setAddressLow(it->add_low);
                        w_it->second.cline_ptr->setAddressHigh(it->add_high);
                        w_it->second.cline_ptr->setIdxLow(it->idx_low);
                        w_it->second.cline_ptr->setIdxHigh(it->idx_high);
                        w_it->second.cline_ptr->setDataStructure(it->name);
                        w_it->second.cline_ptr->setAge(0);
                        w_it->second.age = 0; //!!decide on if to keep in struct
                        w_it->second.tag = it->tag; //!!decide on if to keep in struct
                        w_it->second.cline_ptr->setSector_one_filled(it->s0);
                        w_it->second.cline_ptr->setSector_two_filled(it->s1);
                        w_it->second.cline_ptr->setSector_three_filled(it->s2);
                        w_it->second.cline_ptr->setSector_four_filled(it->s3);
                        QColor color(Qt::green);
                        w_it->second.cline_ptr->setColor(color);
                        w_it->second.cline_ptr->update();
                        write_once = false;
                    } else {
                        w_it->second.cline_ptr->incAge();
                        w_it->second.age++;
                    }
                }

            } else if (!found_data_on_write && !detected_empy_cline) {
                for (auto we_it = ret.first; we_it != ret.second; we_it++) {
                    if(we_it->second.cline_ptr->getTag() == oldest_tag){ //!!should also add random eviction on write
                        //qDebug("Write Miss + Evict\n");
                        stat_out->recordWriteMiss();
                        we_it->second.cline_ptr->setTag(it->tag);
                        we_it->second.cline_ptr->setAddressLow(it->add_low);
                        we_it->second.cline_ptr->setAddressHigh(it->add_high);
                        we_it->second.cline_ptr->setIdxLow(it->idx_low);
                        we_it->second.cline_ptr->setIdxHigh(it->idx_high);
                        we_it->second.cline_ptr->setDataStructure(it->name);
                        we_it->second.cline_ptr->setAge(0);
                        we_it->second.age = 0; //!!decide on if to keep in struct
                        we_it->second.tag = it->tag; //!!decide on if to keep in struct
                        we_it->second.cline_ptr->setSector_one_filled(it->s0);
                        we_it->second.cline_ptr->setSector_two_filled(it->s1);
                        we_it->second.cline_ptr->setSector_three_filled(it->s2);
                        we_it->second.cline_ptr->setSector_four_filled(it->s3);
                        QColor color(Qt::yellow);
                        we_it->second.cline_ptr->setColor(color);
                        we_it->second.cline_ptr->update();
                    } else {
                        we_it->second.cline_ptr->incAge();
                        we_it->second.age++;
                    }
                }
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
        stat_out->incMemRequests();
        if(it->oper == READ){
            read_once = true;
            ret = idx_map.equal_range(it->set_idx);
            for (auto set_it = ret.first; set_it != ret.second; set_it++) {
                if(set_it->second.cline_ptr->getTag() == it->tag){ // Cache hit
                    stat_out->recordReadFullHit();
                    if (it->add_low < set_it->second.cline_ptr->getAddressLow()) {set_it->second.cline_ptr->setAddressLow(it->add_low);}
                    if (it->add_high > set_it->second.cline_ptr->getAddressHigh()) {set_it->second.cline_ptr->setAddressHigh(it->add_high);}
                    if (it->idx_low < set_it->second.cline_ptr->getIdxLow()) {set_it->second.cline_ptr->setIdxLow(it->idx_low);}
                    if (it->idx_high > set_it->second.cline_ptr->getIdxHigh()) {set_it->second.cline_ptr->setIdxHigh(it->idx_high);}
                    set_it->second.cline_ptr->setDataStructure(it->name); //Maybe not required
                    set_it->second.cline_ptr->setAge(0);
                    set_it->second.age = 0; //!!decide on if to keep in struct
                    set_it->second.tag = it->tag; //!!decide on if to keep in struct
                    QColor color(Qt::darkGreen);
                    set_it->second.cline_ptr->setColor(color);
                    set_it->second.cline_ptr->update();
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
                    if(r_it->second.cline_ptr->getIs_empty() && read_once){
                        stat_out->recordReadMiss();
                        r_it->second.cline_ptr->setTag(it->tag);
                        r_it->second.cline_ptr->setAddressLow(it->add_low);
                        r_it->second.cline_ptr->setAddressHigh(it->add_high);
                        r_it->second.cline_ptr->setIdxLow(it->idx_low);
                        r_it->second.cline_ptr->setIdxHigh(it->idx_high);
                        r_it->second.cline_ptr->setDataStructure(it->name);
                        r_it->second.cline_ptr->setAge(0);
                        r_it->second.age = 0; //!!decide on if to keep in struct
                        r_it->second.tag = it->tag; //!!decide on if to keep in struct
                        QColor color(Qt::green);
                        r_it->second.cline_ptr->setColor(color);
                        r_it->second.cline_ptr->update();
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
                            stat_out->recordReadMiss();
                            re_it->second.cline_ptr->setTag(it->tag);
                            re_it->second.cline_ptr->setAddressLow(it->add_low);
                            re_it->second.cline_ptr->setAddressHigh(it->add_high);
                            re_it->second.cline_ptr->setIdxLow(it->idx_low);
                            re_it->second.cline_ptr->setIdxHigh(it->idx_high);
                            re_it->second.cline_ptr->setDataStructure(it->name);
                            re_it->second.cline_ptr->setAge(0);
                            re_it->second.age = 0; //!!decide on if to keep in struct
                            re_it->second.tag = it->tag; //!!decide on if to keep in struct
                            QColor color(Qt::yellow);
                            re_it->second.cline_ptr->setColor(color);
                            re_it->second.cline_ptr->update();
                        } else {
                            re_it->second.cline_ptr->incAge();
                            re_it->second.age++;
                        }
                    }

                } else if(replacement_policy == 2){
                    //!!consider random evict
                    std::random_device rd;
                    std::mt19937 rng(rd());
                    std::uniform_int_distribution<int> uni(0,way_size_l2); //!!check the iterator to decide on the range(0-5 or 1-6)
                    auto random_integer = uni(rng);
                    //...
                }
            }

        } else if (it->oper == WRITE){ //!!RETHINK the write scenarios
            write_once = true;
            ret = idx_map.equal_range(it->set_idx);
            for (auto s_it = ret.first; s_it != ret.second; s_it++) {
                if(s_it->second.cline_ptr->getTag() == it->tag){ //write hit
                    stat_out->recordWriteFullHit();
                    if (it->add_low < s_it->second.cline_ptr->getAddressLow()) {s_it->second.cline_ptr->setAddressLow(it->add_low);}
                    if (it->add_high > s_it->second.cline_ptr->getAddressHigh()) {s_it->second.cline_ptr->setAddressHigh(it->add_high);}
                    if (it->idx_low < s_it->second.cline_ptr->getIdxLow()) {s_it->second.cline_ptr->setIdxLow(it->idx_low);}
                    if (it->idx_high > s_it->second.cline_ptr->getIdxHigh()) {s_it->second.cline_ptr->setIdxHigh(it->idx_high);}
                    if (!s_it->second.cline_ptr->getSector_one_filled() && it->s0){s_it->second.cline_ptr->setSector_one_filled(true);}
                    if (!s_it->second.cline_ptr->getSector_two_filled() && it->s1){s_it->second.cline_ptr->setSector_two_filled(true);}
                    if (!s_it->second.cline_ptr->getSector_three_filled() && it->s2){s_it->second.cline_ptr->setSector_three_filled(true);}
                    if (!s_it->second.cline_ptr->getSector_four_filled() && it->s3){s_it->second.cline_ptr->setSector_four_filled(true);}
                    s_it->second.cline_ptr->setDataStructure(it->name); //maybe not needed?
                    s_it->second.cline_ptr->setAge(0);
                    s_it->second.age = 0; //!!decide on if to keep in struct
                    s_it->second.tag = it->tag; //!!decide on if to keep in struct
                    QColor color(Qt::green);
                    s_it->second.cline_ptr->setColor(color);
                    s_it->second.cline_ptr->update();
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
                    if(w_it->second.cline_ptr->getIs_empty() && write_once){
                        stat_out->recordWriteMiss();
                        w_it->second.cline_ptr->setTag(it->tag);
                        w_it->second.cline_ptr->setAddressLow(it->add_low);
                        w_it->second.cline_ptr->setAddressHigh(it->add_high);
                        w_it->second.cline_ptr->setIdxLow(it->idx_low);
                        w_it->second.cline_ptr->setIdxHigh(it->idx_high);
                        w_it->second.cline_ptr->setDataStructure(it->name);
                        w_it->second.cline_ptr->setAge(0);
                        w_it->second.age = 0; //!!decide on if to keep in struct
                        w_it->second.tag = it->tag; //!!decide on if to keep in struct
                        QColor color(Qt::green);
                        w_it->second.cline_ptr->setColor(color);
                        w_it->second.cline_ptr->update();
                        write_once = false;
                    } else {
                        w_it->second.cline_ptr->incAge();
                        w_it->second.age++;
                    }
                }

            } else if (!found_data_on_write && !detected_empy_cline) {
                for (auto we_it = ret.first; we_it != ret.second; we_it++) {
                    if(we_it->second.cline_ptr->getTag() == oldest_tag){
                        stat_out->recordWriteMiss();
                        we_it->second.cline_ptr->setTag(it->tag);
                        we_it->second.cline_ptr->setAddressLow(it->add_low);
                        we_it->second.cline_ptr->setAddressHigh(it->add_high);
                        we_it->second.cline_ptr->setIdxLow(it->idx_low);
                        we_it->second.cline_ptr->setIdxHigh(it->idx_high);
                        we_it->second.cline_ptr->setDataStructure(it->name);
                        we_it->second.cline_ptr->setAge(0);
                        we_it->second.age = 0; //!!decide on if to keep in struct
                        we_it->second.tag = it->tag; //!!decide on if to keep in struct
                        QColor color(Qt::yellow);
                        we_it->second.cline_ptr->setColor(color);
                        we_it->second.cline_ptr->update();
                    } else {
                        we_it->second.cline_ptr->incAge();
                        we_it->second.age++;
                    }
                }
            }

        }//end if WRITE
    }
}
