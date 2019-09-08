/*Configuration variables with global scope*/
#include "globals.h"
#include "math.h"

/*Init with Titanx configuration on default*/
std::string name = "GTX TitanX";
int l2_size = 3145728;
int l1_size = 24576;
int line_size = 128;
long long global_size = 12884901888;
int sector_size = 32;
int way_size_l1 = 6;
int way_size_l2 = 6;
int warp_size = 6;
int is_sectored = 1;
int is_l1_enabled = 0;
int num_sm = 24;
int concurrent_warps = 4;
int cuda_cores = 3072;
int replacement_policy = 1; //1 =LRU
int write_policy = 1; //1 = write allocate
int l1_load_granularity = 128;
int l2_load_granularity = 32;
std::string architecture = "2nd generation Maxwell";
std::string chip = "GM200-400-A1";
int worker_delay = 200;
int global_latency = 400;
int data_type_bytes = 4;//!!Add this in config file

bool missingConfigAttribute = false;

int bytes_in_set_l2;
int num_sets_l2;
int num_sectors_l2;
int num_clines_l2;
int sectorsPerSetL2;

int bytes_in_set_l1;
int num_sets_l1;
int num_clines_l1;
int block_offset_bits;

bool simulation_done = false;
bool trace_loaded = false;
bool config_loaded = false;
bool sim_mode_selected = false;
int sim_mode = 0;
bool start_flag = false;
bool pause_flag = false;
bool worker_running = false;

std::multimap<int, cline_info> idx_map;

globals::globals()
{
}

void printGlobals(){
    printf("PRINTING CACHE CONFIGURATION\n");
    printf("--------------------------------\n");
    printf("GPU Name: %s \n", name.c_str());
    printf("Global Memory(bytes): %llu\n", global_size);
    printf("L2 Size: %d\n",l2_size);
    printf("L1 Size: %d\n",l1_size);
    printf("Cache Line size(bytes): %d\n",line_size);
    printf("Is cache line sectored? : %d\n",is_sectored);
    printf("Sector Size: %d\n",sector_size);
    printf("Warp Size: %d\n",warp_size);
    printf("Set Associativety(L1): %d-way \n",way_size_l1);
    printf("Set Associativety(L2): %d-way \n",way_size_l2);
    printf("Is L1 enabled? : %d\n",is_l1_enabled);
    printf("Load granularity(L1): %d\n",l1_load_granularity);
    printf("Load granularity(L2): %d\n",l2_load_granularity);
    printf("Streaming multiproccesors: %d\n",num_sm);
    printf("Concurrently active warps: %d\n",concurrent_warps);
    printf("Total # of cache lines(L2): %d\n",num_clines_l2);
    printf("Total # of sets (L2): %d\n",num_sets_l2);
    printf("Total # of sectors (L2): %d\n",num_sectors_l2);
    printf("Total # of bytes in set (L2): %d\n",bytes_in_set_l2);
    printf("Total # of sectors in set (L2): %d\n",sectorsPerSetL2);
    printf("Total # of cache lines(L1): %d\n",num_clines_l1);
    printf("Total # of sets (L1): %d\n",num_sets_l1);
    printf("Total # of bytes in set (L1): %d\n",bytes_in_set_l1);

}

void computeConfig(){
    num_clines_l2 = l2_size / line_size;
    num_sets_l2 = num_clines_l2 / way_size_l2;
    num_sectors_l2 = num_sets_l2 * (line_size/sector_size);
    bytes_in_set_l2 = way_size_l2 * line_size;
    sectorsPerSetL2 = way_size_l2 * (line_size / sector_size);

    num_clines_l1 = l1_size / line_size;
    num_sets_l1 = num_clines_l1 / way_size_l1;
    bytes_in_set_l1 = way_size_l1 * line_size;
    block_offset_bits = int(log2(line_size));
}
