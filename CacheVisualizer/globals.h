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


class globals
{
public:
    globals();
};

#endif // GLOBALS_H
