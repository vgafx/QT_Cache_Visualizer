/*** Settings
 * Class designed to keep a state for the application settings that the visualizer uses.
 * The settings listed in this class are the most common (and essential) GPU architecture
 * characteristics used for the kernel execution visualization. Designed as a singleton class
 * due to the need for a singular set of settings being supported at a time. The default
 * constructor will initialize the object with the characteristics of the GTX TitanX GPU.
 * On startup, the visualizer is designed to read a configuration file containing values
 * for the members of this class. If this fails, the visualizer will default to the hardcoded
 * TitanX values so that a scene is rendered in any case.
 ***/
#pragma once

#include <string>
#include <cmath>
//#include <QDebug>

/* Bitmasks for the hashing function
 * (2^x) - 1, i.e. Masks with the x LSBs set.
 */
constexpr int MASK_5_BIT = 31;
constexpr int MASK_6_BIT = 63;
constexpr int MASK_7_BIT = 127;
constexpr int MASK_8_BIT = 255;
constexpr int MASK_9_BIT = 511;
constexpr int MASK_10_BIT = 1023;
constexpr int MASK_11_BIT = 2047;
constexpr int MASK_12_BIT = 4095;
constexpr int MASK_13_BIT = 8191;
constexpr int MASK_14_BIT = 16383;
constexpr int MASK_15_BIT = 32767;
constexpr int MASK_16_BIT = 65535;
constexpr int MASK_17_BIT = 131071;
constexpr int MASK_18_BIT = 262143;
constexpr int MASK_19_BIT = 524287;
constexpr int MASK_20_BIT = 1048575;

constexpr int BLOCK_OFFSET_BITS = 7;

const std::string c_name = "configuration";

/*Enums for settings and input data*/
enum OperationType : unsigned char {
    READ = 0, WRITE
};

enum ReplacementPolicy : unsigned char {
    RANDOM = 0, LRU
};

enum MemWritePolicy : unsigned char {
  BYPASS = 0, W_ALLOCATE
};

enum ReturnType : int {
    EMPTY = -1, NORET = -2
};


class Settings
{
private:
    /*Device Info*/
    std::string m_gpu_name;
    std::string m_architecture;
    std::string m_chip;
    /*Device Characteristics*/
    long long m_global_mem_size;
    int m_num_sm;
    int m_cores;
    int m_concurrent_warps;
    int m_concurrent_blocks;
    int m_warp_size;
    int m_max_threads;

    /*Cache Memory Characteristics*/
    int m_l2_size_bytes;
    int m_l1_size_bytes;
    int m_cline_size;
    int m_sector_size;
    int m_way_size_l2;
    int m_way_size_l1;
    bool m_is_sectored;
    bool m_is_l1_enabled;

    ReplacementPolicy m_r_policy;
    MemWritePolicy m_w_policy;

    /*Computed Cache Memory Characteristics*/
    int m_bytes_in_set_l2;
    int m_num_sets_l2;
    int m_num_sectors_l2;
    int m_num_clines_l2;
    int m_sectors_per_set_l2;
    int m_block_offset_bits;
    /*L1 currently unsupported*/
    int m_bytes_in_set_l1;
    int m_num_sets_l1;
    int m_num_clines_l1;

    /*Misc*/
    int m_global_to_cache_latency;
    int m_worker_delay;
    int m_sim_dtype_bytes;
    bool m_missing_config_attribute;
    size_t m_reserve_size;

    Settings();

    /*Setters & Getters that modify the actual object*/
    const std::string& getGpuNameImpl() const;
    void setGpuNameImpl(const std::string& gpu_name);

    const std::string& getArchitectureImpl() const;
    void setArchitectureImpl(const std::string& arch);

    const std::string& getChipImpl() const;
    void setChipImpl(const std::string& chip);

    long long getGlobalMemSizeImpl() const;
    void setGlobalMemSizeImpl(long long size_bytes);

    int getNumSmImpl() const;
    void setNumSmImpl(int num_sm);

    int getCoresImpl() const;
    void setCoresImpl(int cores);

    int getConcurrentWarpsImpl() const;
    void setConcurrentWarpsImpl(int concurrent_warps);

    int getConcurrentBlocksImpl() const;
    void setConcurrentBlocksImpl(int concurrent_blocks);

    int getWarpSizeImpl() const;
    void setWarpSizeImpl(int warp_size);

    int getMaxThreadsImpl() const;
    void setMaxThreadsImpl(int max_threads);

    int getL2SizeBytesImpl() const;
    void setL2SizeBytesImpl(int l2_bytes);

    int getL1SizeBytesImpl() const;
    void setL1SizeBytesImpl(int l1_bytes);

    int getClineSizeImpl() const;
    void setClineSizeImpl(int cline_bytes);

    int getSectorSizeImpl() const;
    void setSectorSizeImpl(int sector_bytes);

    int getWaySizeL2Impl() const;
    void setWaySizeL2Impl(int l2_wayness);

    int getWaySizeL1Impl() const;
    void setWaySizeL1Impl(int l1_wayness);

    bool getIsSectoredImpl() const;
    void setIsSectoredImpl(bool sectored);

    bool getL1EnabledImpl() const;
    void setL1EnabledImpl(bool enabled);

    ReplacementPolicy getReplacePolicyImpl() const;
    void setReplacePolicyImpl(ReplacementPolicy rp);

    MemWritePolicy getWritePolicyImpl() const;
    void setWritePolicyImpl(MemWritePolicy wp);

    int getBytesInSetL2Impl() const;

    int getNumSetsL2Impl() const;

    int getNumSectorsL2Impl() const;

    int getNumClinesL2Impl() const;

    int getSectorsPerSetL2Impl() const;

    int getBlockOffsetBitsImpl() const;

    int getBytesInSetL1Impl() const;

    int getNumSetsL1Impl() const;

    int getNumClinesL1Impl() const;

    int getGlobalToCacheLatencyImpl() const;
    void setGlobalToCacheLatencyImpl(int latency);

    int getWorkerDelayImpl() const;
    void setWorkerDelayImpl(int delay);

    int getDatatypeBytesImpl() const;
    void setDatatypeBytesImpl(int dt_bytes);

    size_t getReserveSizeImpl() const;
    void setReserveSizeImpl(const size_t &reserve_size);

    /*Object functions*/
    void computeConfigImpl();

    void printSettingsImpl();

public:
    /*No copy constructor*/
    Settings(const Settings&) = delete;
    /*No assignment operator*/
    Settings& operator= (const Settings&) = delete;

    static Settings& GetInstance();

    /*Statically accessible Setters & Getters that call the equivalent implementation*/
    static const std::string& getGpuName();
    static void setGpuName(const std::string& gpu_name);

    static const std::string& getArchitecture();
    static void setArchitecture(const std::string& arch);

    static const std::string& getChip();
    static void setChip(const std::string& chip);

    static long long getGlobalMemSize();
    static void setGlobalMemSize(long long size_bytes);

    static int getNumSm();
    static void setNumSm(int num_sm);

    static int getCores();
    static void setCores(int cores);

    static int getConcurrentWarps();
    static void setConcurrentWarps(int concurrent_warps);

    static int getConcurrentBlocks();
    static void setConcurrentBlocks(int concurrent_blocks);

    static int getWarpSize();
    static void setWarpSize(int warp_size);

    static int getMaxThreads();
    static void setMaxThreads(int max_threads);

    static int getL2SizeBytes();
    static void setL2SizeBytes(int l2_size_bytes);

    static int getL1SizeBytes();
    static void setL1SizeBytes(int l1_bytes);

    static int getClineSize();
    static void setClineSize(int cline_bytes);

    static int getSectorSize();
    static void setSectorSize(int sector_bytes);

    static int getWaySizeL2();
    static void setWaySizeL2(int l2_wayness);

    static int getWaySizeL1();
    static void setWaySizeL1(int l1_wayness);

    static bool getIsSectored();
    static void setIsSectored(bool sectored);

    static bool getL1Enabled();
    static void setL1Enabled(bool enabled);

    static ReplacementPolicy getReplacePolicy();
    static void setReplacePolicy(ReplacementPolicy rp);

    static MemWritePolicy getWritePolicy();
    static void setWritePolicy(MemWritePolicy wp);

    static int getBytesInSetL2();

    static int getNumSetsL2();

    static int getNumSectorsL2();

    static int getNumClinesL2();

    static int getSectorsPerSetL2();

    static int getBlockOffsetBits();

    static int getBytesInSetL1();

    static int getNumSetsL1();

    static int getNumClinesL1();

    static int getGlobalToCacheLatency();
    static void setGlobalToCacheLatency(int latency);

    static int getWorkerDelay();
    static void setWorkerDelay(int delay);

    static int getDatatypeBytes();
    static void setDatatypeBytes(int dt_bytes);

    static size_t getReserveSize();
    static void setReserveSize(const size_t &reserve_size);

    /*Object functions*/
    static void computeConfig();

    /*Debug*/
    static void printSettings();

};
