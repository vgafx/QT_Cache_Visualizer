#include <string>
#ifndef MAIN_H
#define MAIN_H

//loaded from file
extern std::string name;
extern int l2_size;
extern int l1_size;
extern int line_size;
extern int way_size;
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

//computed
extern int bytes_in_set;
extern int num_sets;
extern int num_sectors;
extern int num_clines;



#endif // MAIN_H
