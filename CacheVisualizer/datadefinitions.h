/*** DataDefinitions
 *  Contains the definitions for the structs that the visualizer will use to store
 *  and process memory access data. The structs are templated to allow for some freedom
 *  in the datatypes of choice. In general, thread indexes, warp ids and the number of
 *  requests are sufficiently represented using int. Based on observations, the data structure
 *  indexes are typically within the range of an unsigned integer (unless the data structure
 *  used is very large in size), and the memory address / timestamp are typically larger than
 *  that (unsinged long long required).
 *
 *  The input_entry and update_line_info structs are templated with 2 T's
 *  T for the data structure index (recommended unsinged int)
 *  T2 for the memory address / timestamp (recommended unsinged long long)
 *
 *  If the generated memory trace information can be represented with datatypes that
 *  consume less memory, then compile for it, as the visualizer will perform better
 *  in this case.
 ***/
#pragma once

#include <string>
#include "settings.h"

/*schedule_entry structs are used to store block scheduling information*/
struct schedule_entry{
    int block_id_x;
    int block_id_y;
    int sm_id;
    long long g_time;

    schedule_entry(int bx, int by, int sm, long long t)
        : block_id_x(bx), block_id_y(by), sm_id(sm), g_time(t)
    {
    }

    bool operator<(const schedule_entry& a) const {
        return g_time < a.g_time;
    }
};

/*input_entry structs are used for storing trace input*/
template<typename T, typename T2>
struct input_entry{
    int id_x;
    int id_y;
    int id_z;
    int block_x;
    int block_y;
    int block_z;
    int warp_id;
    char ds_name;
    OperationType operation;
    T ds_index;
    T2 address;
    T2 timestamp;
};

/*mem_acc_entry structs are held by the instruction stream*/
template<typename T, typename T2>
struct mem_acc_entry{
    int warp_id;
    char ds_name;
    OperationType op;
    T ds_index;
    T2 address;
    T2 timestamp;

    mem_acc_entry(int wid, char dname, OperationType o, T idx, T2 add, T2 t)
        : warp_id(wid), ds_name(dname), op(o), ds_index(idx), address(add), timestamp(t)
    {
    }

    bool operator<(const mem_acc_entry& a)const{
        return timestamp > a.timestamp;
    }
};


/*update_line_info structs are ready to be 'consumed' by the cache state for updating*/
template<typename T, typename T2>
struct update_cline_info{
    int set_idx;
    int tag; //check
    OperationType oper;
    char name;
    T idx_low;
    T idx_high;
    T2 add_low;
    T2 add_high;
    T2 timestamp;
    bool s0;
    bool s1;
    bool s2;
    bool s3;
    int s0_req;
    int s1_req;
    int s2_req;
    int s3_req;

    update_cline_info(int s_id, int tg, OperationType op, char n, T i_l, T i_h, T2 a_l, T2 a_h, T2 ts,
                      bool s_0, bool s_1, bool s_2, bool s_3, int s0_r, int s1_r,int s2_r,int s3_r)
        : set_idx(s_id), tag(tg), oper(op), name(n), idx_low(i_l), idx_high(i_h), add_low(a_l), add_high(a_h), timestamp(ts),
          s0(s_0), s1(s_1), s2(s_2), s3(s_3), s0_req(s0_r), s1_req(s1_r), s2_req(s2_r),s3_req(s3_r)
    {
    }
};

/*distinct_entries structs are used for intermediate states*/
template<typename T, typename T2>
struct distinct_entries{
    int set_id;
    int tag;
    T min_idx;
    T max_idx;
    T2 min_add;
    T2 max_add;
    T2 timestamp;
    bool s0, s1, s2, s3;
    int s0_reqs;
    int s1_reqs;
    int s2_reqs;
    int s3_reqs;
};
