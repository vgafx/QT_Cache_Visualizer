#include "cacherules.h"
#include "globals.h"

#include <list>
#include <map>
#include <utility>
#include <QColor>
#include <QObject>


cacherules::cacherules()
{
}


void updateSetOnRead(){

}

void updateSetOnWrite(){

}


void updateSceneFromInfo(std::list<update_line_info> up_info){
    bool found_data_on_read = false, found_data_on_write = false, detected_empy_cline = false;
    bool read_once = true, write_once = true;
    int oldest_age = INT_MIN, oldest_tag = 0;
    int hit_tag = 0;
    std::pair <std::multimap<int,cline_info>::iterator, std::multimap<int,cline_info>::iterator> ret;

    for (auto it = up_info.begin(); it != up_info.end(); it++) { //For all the update entry returned by the threadblock
        if(it->oper == READ){
            ret = idx_map.equal_range(it->set_idx);
            for (auto set_it = ret.first; set_it != ret.second; set_it++) {
                if(set_it->second.cline_ptr->getTag() == it->tag){ // Cache hit
                    //Check if it is a partial hit
                    if(set_it->second.cline_ptr->getSector_one_filled() == it->s0 && set_it->second.cline_ptr->getSector_two_filled() == it->s1
                            && set_it->second.cline_ptr->getSector_three_filled() == it->s2 && set_it->second.cline_ptr->getSector_four_filled() == it->s3){
                        //Full hit
                        set_it->second.cline_ptr->setAge(0);
                    } else {
                        set_it->second.cline_ptr->setAddressLow(it->add_low);
                        set_it->second.cline_ptr->setAddressHigh(it->add_high);
                        set_it->second.cline_ptr->setIdxLow(it->idx_low);
                        set_it->second.cline_ptr->setIdxHigh(it->idx_high);
                        set_it->second.cline_ptr->setDataStructure(it->name);
                        set_it->second.cline_ptr->setAge(0);
                        set_it->second.age = 0; //!!decide on if to keep in struct
                        set_it->second.tag = it->tag; //!!decide on if to keep in struct
                        set_it->second.cline_ptr->setSector_one_filled(it->s0);
                        set_it->second.cline_ptr->setSector_two_filled(it->s1);
                        set_it->second.cline_ptr->setSector_three_filled(it->s2);
                        set_it->second.cline_ptr->setSector_four_filled(it->s3);
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
                    }
                    //Age all other lines
                    r_it->second.cline_ptr->incAge();
                    r_it->second.age++;
                }


            } else if (!found_data_on_read && !detected_empy_cline) { //Data not found and the set is full, so we have to evict
                if(replacement_policy == 1){//LRU
                    for (auto re_it = ret.first; re_it != ret.second; re_it++) {
                        if(re_it->second.cline_ptr->getTag() == oldest_tag){
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
                        }
                        re_it->second.cline_ptr->incAge();
                        re_it->second.age++;
                    }


                } else if(replacement_policy == 2){

                }
            }

        } else if (it->oper == WRITE){ //!!RETHINK the write scenarios
            ret = idx_map.equal_range(it->set_idx);
            for (auto s_it = ret.first; s_it != ret.second; s_it++) {
                if(s_it->second.cline_ptr->getTag() == it->tag){ //write hit
                    s_it->second.cline_ptr->setAddressLow(it->add_low);
                    s_it->second.cline_ptr->setAddressHigh(it->add_high);
                    s_it->second.cline_ptr->setIdxLow(it->idx_low);
                    s_it->second.cline_ptr->setIdxHigh(it->idx_high);
                    s_it->second.cline_ptr->setDataStructure(it->name);
                    s_it->second.cline_ptr->setAge(0);
                    s_it->second.age = 0; //!!decide on if to keep in struct
                    s_it->second.tag = it->tag; //!!decide on if to keep in struct
                    s_it->second.cline_ptr->setSector_one_filled(it->s0);
                    s_it->second.cline_ptr->setSector_two_filled(it->s1);
                    s_it->second.cline_ptr->setSector_three_filled(it->s2);
                    s_it->second.cline_ptr->setSector_four_filled(it->s3);
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
                    }
                    w_it->second.cline_ptr->incAge();
                    w_it->second.age++;
                }


            } else if (!found_data_on_write && !detected_empy_cline) {
                for (auto we_it = ret.first; we_it != ret.second; we_it++) {
                    if(we_it->second.cline_ptr->getTag() == oldest_tag){
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
                    }
                    we_it->second.cline_ptr->incAge();
                    we_it->second.age++;
                }

            }

        }//end if WRITE
    }

}

