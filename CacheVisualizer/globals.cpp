#include "globals.h"

globals::globals()
{
    std::string name="";
    int l2_size;
    int l1_size;
    int line_size;
    int global_size;
    int sector_size;
    int way_size;
    int warp_size;
    int is_sectored;
    int is_l1_enabled;
    int num_sm;
    int concurrent_warps;
    int cuda_cores;
    int replacement_policy; //1 =LRU
    int write_policy; //1 = write allocate
    int l1_load_granularity;
    int l2_load_granularity;

    int bytes_in_set;
    int num_sets;
    int num_sectors;
    int num_clines;

}

void printGlobals(){
    printf("PRINTING CACHE CONFIGURATION\n");
    printf("--------------------------------\n");
    printf("GPU Name: %s \n", name.c_str());
    printf("Global Memory(bytes): %d\n", global_size);
    printf("L2 Size: %d\n",l2_size);
    printf("L1 Size: %d\n",l1_size);
    printf("Cache Line size(bytes): %d\n",line_size);
    printf("Is cache line sectored? : %d\n",is_sectored);
    printf("Sector Size: %d\n",sector_size);
    printf("L2 Size: %d\n",l2_size);
    printf("L2 Size: %d\n",l2_size);
    printf("L2 Size: %d\n",l2_size);
    printf("L2 Size: %d\n",l2_size);
    printf("L2 Size: %d\n",l2_size);
    printf("L2 Size: %d\n",l2_size);

}
