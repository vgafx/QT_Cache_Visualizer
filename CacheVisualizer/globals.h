#include <string>
#include <map>
#include <QColor>
#include <QGraphicsItem>
#include "cacheline.h"

#ifndef GLOBALS_H
#define GLOBALS_H

void printGlobals();
void computeConfig();

//loaded from file
extern std::string name;
extern int l2_size;
extern int l1_size;
extern long long global_size;
extern int sector_size;
extern int line_size;
extern int way_size_l1;
extern int way_size_l2;
extern int warp_size;
extern int is_sectored;
extern int is_l1_enabled;
extern int num_sm;
extern int concurrent_warps;
extern int cuda_cores;
extern int replacement_policy; //1 =LRU
extern int write_policy; //1 = write allocate
extern int l1_load_granularity;
extern int l2_load_granularity;
extern std::string architecture;
extern std::string chip;
extern int data_type_bytes; //!! add to config

extern bool missingConfigAttribute;

//computed
extern int bytes_in_set_l2;
extern int num_sets_l2;
extern int num_sectors_l2;
extern int num_clines_l2;
extern int sectorsPerSetL2;

extern int bytes_in_set_l1;
extern int num_sets_l1;
extern int num_clines_l1;

extern bool simulation_done;
extern bool trace_loaded;
extern bool config_loaded;
extern bool sim_mode_selected;
extern int sim_mode; //0=autoplay , 1= step wise

#define EMPTY_RET -1
#define NO_RET -2

#define READ 0
#define WRITE 1

#define MASK_7bit 127
#define MASK_8bit 255
#define MASK_9bit 511
#define MASK_10bit 1023
#define MASK_11bit 2047
#define MASK_12bit 4095
#define MASK_13bit 8191
#define MASK_14bit 16383
#define MASK_15bit 32767
#define MASK_16bit 65535


#define BLOCK_OFFSET_BITS 7


struct cline_info{
    cacheline *cline_ptr;
    int tag;
    int age;
};

extern std::multimap<int, cline_info> idx_map;

struct input_entry{
    int idx;
    int idy;
    int idz;
    int block_x;
    int block_y;
    int block_z;
    int warp_id;
    std::string data_str;
    int operation; //0 = read, 1 = write
    unsigned long long ds_index;
    unsigned long long address;
    unsigned long long cycles;
};

struct update_line_info{
    int set_idx;
    int tag;
    int oper;
    std::string name;
    long long add_low;
    long long add_high;
    long long idx_low;
    long long idx_high;
    long long cycles;
    bool s0;
    bool s1;
    bool s2;
    bool s3;

//    bool operator<(const update_line_info& a)const{
//        return cycles < a.cycles;
//    }
};

struct distinct_entries{
    int set_id;
    int tag;
    long long min_add;
    long long max_add;
    long long min_idx;
    long long max_idx;
    bool sec0, sec1, sec2, sec3;
};





class globals
{
public:
    globals();
};

#endif // GLOBALS_H
