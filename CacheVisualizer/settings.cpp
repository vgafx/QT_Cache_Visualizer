#include "settings.h"

/**Default Constructor
 * The visualizer will use the GTX TitanX characteristics for initialization
 * on startup. Called on first and only instantiation.
 **/
Settings::Settings()
{
    m_gpu_name = "GTX TitanX";
    m_architecture = "2nd generation Maxwell";
    m_chip = "GM200-400-A1";
    m_global_mem_size = 12884901888;
    m_num_sm = 24;
    m_cores = 3072;
    m_concurrent_warps = 4;
    m_concurrent_blocks = 1;
    m_warp_size = 32;
    m_max_threads = 1024;
    m_l2_size_bytes = 2359296;
    m_l1_size_bytes = 24576;
    m_cline_size = 128;
    m_sector_size = 32;
    m_way_size_l2 = 9;
    m_way_size_l1 = 9;
    m_is_sectored = true;
    m_is_l1_enabled = false;
    m_r_policy = LRU;
    m_w_policy = W_ALLOCATE;
    m_global_to_cache_latency = 142;
    m_worker_delay = 1;
    m_sim_dtype_bytes = 4;
    m_reserve_size = 500000;

    /*Calculate Cache Attributes based on settings*/
    this->computeConfigImpl();
}

/*Will create a single instance of the settings class on 1st call*/
Settings &Settings::GetInstance(){
    static Settings instance;
    return instance;
}

/**Public-facing Setters & Getters.
 * All of them call their private equivalent.
 **/
const std::string& Settings::getGpuName()
{
    return GetInstance().getGpuNameImpl();
}

void Settings::setGpuName(const std::string& gpu_name)
{
    GetInstance().setGpuNameImpl(gpu_name);
}

const std::string &Settings::getArchitecture()
{
    return GetInstance().getArchitectureImpl();
}

void Settings::setArchitecture(const std::string& arch)
{
    GetInstance().setArchitectureImpl(arch);
}

const std::string &Settings::getChip()
{
    return GetInstance().getChipImpl();
}

void Settings::setChip(const std::string& chip)
{
    GetInstance().setChipImpl(chip);
}

long long Settings::getGlobalMemSize()
{
    return GetInstance().getGlobalMemSizeImpl();
}

void Settings::setGlobalMemSize(long long size_bytes)
{
    GetInstance().setGlobalMemSizeImpl(size_bytes);
}

int Settings::getNumSm()
{
    return GetInstance().getNumSmImpl();
}

void Settings::setNumSm(int num_sm)
{
    GetInstance().setNumSmImpl(num_sm);
}

int Settings::getCores()
{
     return GetInstance().getCoresImpl();
}

void Settings::setCores(int cores)
{
    GetInstance().setCoresImpl(cores);
}

int Settings::getConcurrentWarps()
{
     return GetInstance().getConcurrentWarpsImpl();
}

void Settings::setConcurrentWarps(int concurrent_warps)
{
    GetInstance().setConcurrentWarpsImpl(concurrent_warps);
}

int Settings::getConcurrentBlocks()
{
    return GetInstance().getConcurrentBlocksImpl();
}

void Settings::setConcurrentBlocks(int concurrent_blocks)
{
    GetInstance().setConcurrentBlocksImpl(concurrent_blocks);
}

int Settings::getWarpSize()
{
    return GetInstance().getWarpSizeImpl();
}

void Settings::setWarpSize(int warp_size)
{
    GetInstance().setWarpSizeImpl(warp_size);
}

int Settings::getMaxThreads()
{
    return GetInstance().getMaxThreadsImpl();
}

void Settings::setMaxThreads(int max_threads)
{
    GetInstance().setMaxThreadsImpl(max_threads);
}

int Settings::getL2SizeBytes()
{
    return GetInstance().getL2SizeBytesImpl();
}

void Settings::setL2SizeBytes(int l2_size_bytes)
{
    GetInstance().setL2SizeBytesImpl(l2_size_bytes);
}

int Settings::getL1SizeBytes()
{
    return GetInstance().getL1SizeBytesImpl();
}

void Settings::setL1SizeBytes(int l1_bytes)
{
    GetInstance().setL1SizeBytesImpl(l1_bytes);
}

int Settings::getClineSize()
{
    return GetInstance().getClineSizeImpl();
}

void Settings::setClineSize(int cline_bytes)
{
    GetInstance().setClineSizeImpl(cline_bytes);
}

int Settings::getSectorSize()
{
    return GetInstance().getSectorSizeImpl();
}

void Settings::setSectorSize(int sector_bytes)
{
    GetInstance().setSectorSizeImpl(sector_bytes);
}

int Settings::getWaySizeL2()
{
    return GetInstance().getWaySizeL2Impl();
}

void Settings::setWaySizeL2(int l2_wayness)
{
    GetInstance().setWaySizeL2Impl(l2_wayness);
}

int Settings::getWaySizeL1()
{
    return GetInstance().getWaySizeL1Impl();
}

void Settings::setWaySizeL1(int l1_wayness)
{
    GetInstance().setWaySizeL2Impl(l1_wayness);
}

bool Settings::getIsSectored()
{
    return GetInstance().getIsSectoredImpl();
}

void Settings::setIsSectored(bool sectored)
{
    GetInstance().setIsSectoredImpl(sectored);
}

bool Settings::getL1Enabled()
{
    return GetInstance().getL1EnabledImpl();
}

void Settings::setL1Enabled(bool enabled)
{
    GetInstance().setL1EnabledImpl(enabled);
}

ReplacementPolicy Settings::getReplacePolicy()
{
    return GetInstance().getReplacePolicyImpl();
}

void Settings::setReplacePolicy(ReplacementPolicy rp)
{
    GetInstance().setReplacePolicyImpl(rp);
}

MemWritePolicy Settings::getWritePolicy()
{
    return GetInstance().getWritePolicyImpl();
}

void Settings::setWritePolicy(MemWritePolicy wp)
{
    GetInstance().setWritePolicyImpl(wp);
}

int Settings::getBytesInSetL2()
{
    return GetInstance().getBytesInSetL2Impl();
}

int Settings::getNumSetsL2()
{
    return GetInstance().getNumSetsL2Impl();
}

int Settings::getNumSectorsL2()
{
    return GetInstance().getNumSectorsL2Impl();
}

int Settings::getNumClinesL2()
{
    return GetInstance().getNumClinesL2Impl();
}

int Settings::getSectorsPerSetL2()
{
    return GetInstance().getSectorsPerSetL2Impl();
}

int Settings::getBlockOffsetBits()
{
    return GetInstance().getBlockOffsetBitsImpl();
}

int Settings::getBytesInSetL1()
{
    return GetInstance().getBytesInSetL1Impl();
}

int Settings::getNumSetsL1()
{
    return GetInstance().getNumSetsL1Impl();
}

int Settings::getNumClinesL1()
{
    return GetInstance().getNumClinesL1Impl();
}

int Settings::getGlobalToCacheLatency()
{
    return GetInstance().getGlobalToCacheLatencyImpl();
}

void Settings::setGlobalToCacheLatency(int latency)
{
    GetInstance().setGlobalToCacheLatencyImpl(latency);
}

int Settings::getWorkerDelay()
{
    return GetInstance().getWorkerDelayImpl();
}

void Settings::setWorkerDelay(int delay)
{
    GetInstance().setWorkerDelayImpl(delay);
}

int Settings::getDatatypeBytes()
{
    return GetInstance().getDatatypeBytesImpl();
}

void Settings::setDatatypeBytes(int dt_bytes)
{
    GetInstance().setDatatypeBytesImpl(dt_bytes);
}

size_t Settings::getReserveSize()
{
    return GetInstance().getReserveSizeImpl();
}

void Settings::setReserveSize(const size_t &reserve_size)
{
    GetInstance().setReserveSizeImpl(reserve_size);
}

void Settings::computeConfig()
{
    GetInstance().computeConfigImpl();
}

void Settings::printSettings()
{
    GetInstance().printSettingsImpl();
}


/**Private implementations of the Setters & Getters.
 * These are called by the public static methods.
*/
const std::string& Settings::getGpuNameImpl() const
{
    return m_gpu_name;
}

void Settings::setGpuNameImpl(const std::string& gpu_name)
{
    m_gpu_name = gpu_name;
}

const std::string &Settings::getArchitectureImpl() const
{
    return m_architecture;
}

void Settings::setArchitectureImpl(const std::string& arch)
{
    m_architecture = arch;
}

const std::string &Settings::getChipImpl() const
{
    return m_chip;
}

void Settings::setChipImpl(const std::string &chip)
{
    m_chip = chip;
}

long long Settings::getGlobalMemSizeImpl() const
{
    return m_global_mem_size;
}

void Settings::setGlobalMemSizeImpl(long long size_bytes)
{
    m_global_mem_size = size_bytes;
}

int Settings::getNumSmImpl() const
{
    return m_num_sm;
}

void Settings::setNumSmImpl(int num_sm)
{
    m_num_sm = num_sm;
}

int Settings::getCoresImpl() const
{
    return m_cores;
}

void Settings::setCoresImpl(int cores)
{
    m_cores = cores;
}

int Settings::getConcurrentWarpsImpl() const
{
    return m_concurrent_warps;
}

void Settings::setConcurrentWarpsImpl(int concurrent_warps)
{
    m_concurrent_warps = concurrent_warps;
}

int Settings::getConcurrentBlocksImpl() const
{
    return m_concurrent_blocks;
}

void Settings::setConcurrentBlocksImpl(int concurrent_blocks)
{
    m_concurrent_blocks = concurrent_blocks;
}

int Settings::getWarpSizeImpl() const
{
    return m_warp_size;
}

void Settings::setWarpSizeImpl(int warp_size)
{
    m_warp_size = warp_size;
}

int Settings::getMaxThreadsImpl() const
{
    return m_max_threads;
}

void Settings::setMaxThreadsImpl(int max_threads)
{
    m_max_threads = max_threads;
}

int Settings::getL2SizeBytesImpl() const
{
    return m_l2_size_bytes;
}

void Settings::setL2SizeBytesImpl(int l2_size_bytes)
{
    m_l2_size_bytes = l2_size_bytes;
}

int Settings::getL1SizeBytesImpl() const
{
    return m_l1_size_bytes;
}

void Settings::setL1SizeBytesImpl(int l1_bytes)
{
    m_l1_size_bytes = l1_bytes;
}

int Settings::getClineSizeImpl() const
{
    return m_cline_size;
}

void Settings::setClineSizeImpl(int cline_bytes)
{
    m_cline_size = cline_bytes;
}

int Settings::getSectorSizeImpl() const
{
    return m_sector_size;
}

void Settings::setSectorSizeImpl(int sector_bytes)
{
    m_sector_size = sector_bytes;
}

int Settings::getWaySizeL2Impl() const
{
    return m_way_size_l2;
}

void Settings::setWaySizeL2Impl(int l2_wayness)
{
    m_way_size_l2 = l2_wayness;
}

int Settings::getWaySizeL1Impl() const
{
    return m_way_size_l1;
}

void Settings::setWaySizeL1Impl(int l1_wayness)
{
    m_way_size_l1 = l1_wayness;
}

bool Settings::getIsSectoredImpl() const
{
    return m_is_sectored;
}

void Settings::setIsSectoredImpl(bool sectored)
{
    m_is_sectored = sectored;
}

bool Settings::getL1EnabledImpl() const
{
    return m_is_l1_enabled;
}

void Settings::setL1EnabledImpl(bool enabled)
{
    m_is_l1_enabled = enabled;
}

ReplacementPolicy Settings::getReplacePolicyImpl() const
{
    return m_r_policy;
}

void Settings::setReplacePolicyImpl(ReplacementPolicy rp)
{
    m_r_policy = rp;
}

MemWritePolicy Settings::getWritePolicyImpl() const
{
    return m_w_policy;
}

void Settings::setWritePolicyImpl(MemWritePolicy wp)
{
    m_w_policy = wp;
}

int Settings::getBytesInSetL2Impl() const
{
    return m_bytes_in_set_l2;
}

int Settings::getNumSetsL2Impl() const
{
    return m_num_sets_l2;
}

int Settings::getNumSectorsL2Impl() const
{
    return m_num_sectors_l2;
}

int Settings::getNumClinesL2Impl() const
{
    return m_num_clines_l2;
}

int Settings::getSectorsPerSetL2Impl() const
{
    return m_sectors_per_set_l2;
}

int Settings::getBlockOffsetBitsImpl() const
{
    return m_block_offset_bits;
}

int Settings::getBytesInSetL1Impl() const
{
    return m_bytes_in_set_l1;
}

int Settings::getNumSetsL1Impl() const
{
    return m_num_sets_l1;
}

int Settings::getNumClinesL1Impl() const
{
    return m_num_clines_l1;
}

int Settings::getGlobalToCacheLatencyImpl() const
{
    return m_global_to_cache_latency;
}

void Settings::setGlobalToCacheLatencyImpl(int latency)
{
    m_global_to_cache_latency = latency;
}

int Settings::getWorkerDelayImpl() const
{
    return m_worker_delay;
}

void Settings::setWorkerDelayImpl(int delay)
{
    m_worker_delay = delay;
}

int Settings::getDatatypeBytesImpl() const
{
    return m_sim_dtype_bytes;
}

void Settings::setDatatypeBytesImpl(int dt_bytes)
{
    m_sim_dtype_bytes = dt_bytes;
}

size_t Settings::getReserveSizeImpl() const
{
    return m_reserve_size;
}

void Settings::setReserveSizeImpl(const size_t &reserve_size)
{
    m_reserve_size = reserve_size;
}

/*Object functions*/
void Settings::computeConfigImpl()
{
    m_num_clines_l2 = m_l2_size_bytes / m_cline_size;
    m_num_sets_l2 = m_num_clines_l2 / m_way_size_l2;
    m_num_sectors_l2 = m_num_sets_l2 * (m_cline_size/m_sector_size);
    m_bytes_in_set_l2 = m_way_size_l2 * m_cline_size;
    m_sectors_per_set_l2 = m_way_size_l2 * (m_cline_size / m_sector_size);

    m_num_clines_l1 = m_l1_size_bytes / m_cline_size;
    m_num_sets_l1 = m_num_clines_l1 / m_way_size_l1;
    m_bytes_in_set_l1 = m_way_size_l1 * m_cline_size;
    m_block_offset_bits = static_cast<int>(std::log2(m_cline_size));
}

void Settings::printSettingsImpl()
{
//    qDebug("PRINTING SETTINGS\n");
//    qDebug("--------------------------------\n");
//    qDebug("GPU Name: %s \n", m_gpu_name.c_str());
//    qDebug("Arcitecture: %s \n", m_architecture.c_str());
//    qDebug("Chipset: %s \n", m_chip.c_str());
//    qDebug("--\n");
//    qDebug("Global Memory(bytes): %llu\n", m_global_mem_size);
//    qDebug("Streaming multiproccesors: %d\n", m_num_sm);
//    qDebug("CUDA Cores: %d\n", m_cores);
//    qDebug("Concurrently active warps: %d\n", m_concurrent_warps);
//    qDebug("Concurrently active blocks: %d\n", m_concurrent_blocks);
//    qDebug("Warp Size: %d\n", m_warp_size);
//    qDebug("Max Threads per block: %d\n", m_max_threads);
//    qDebug("--\n");
//    qDebug("L2 Size: %d\n", m_l2_size_bytes);
//    qDebug("L1 Size: %d\n", m_l1_size_bytes);
//    qDebug("Cache Line size(bytes): %d\n", m_cline_size);
//    qDebug("Sector Size: %d\n", m_sector_size);
//    qDebug("Set Associativety(L2): %d-way \n", m_way_size_l2);
//    qDebug("Set Associativety(L1): %d-way \n", m_way_size_l1);
//    qDebug("Is cache line sectored? : %d\n", m_is_sectored);
//    qDebug("Is L1 enabled? : %d\n", m_is_l1_enabled);
//    qDebug("Replacement policy: %d\n", m_r_policy);
//    qDebug("Cache writting policy: %d\n", m_w_policy);
//    qDebug("--\n");
//    qDebug("Total # of cache lines(L2): %d\n", m_num_clines_l2);
//    qDebug("Total # of sets (L2): %d\n", m_num_sets_l2);
//    qDebug("Total # of sectors (L2): %d\n", m_num_sectors_l2);
//    qDebug("Total # of bytes in set (L2): %d\n", m_bytes_in_set_l2);
//    qDebug("Total # of sectors in set (L2): %d\n", m_sectors_per_set_l2);
//    qDebug("Total # of cache lines(L1): %d\n", m_num_clines_l1);
//    qDebug("Total # of sets (L1): %d\n", m_num_sets_l1);
//    qDebug("Total # of bytes in set (L1): %d\n", m_bytes_in_set_l1);
//    qDebug("--\n");
//    qDebug("Global to cache latency(cycles): %d\n", m_global_to_cache_latency);
//    qDebug("Worker delay: %d\n", m_worker_delay);
//    qDebug("Datatype size in bytes(used for simulation): %d\n", m_sim_dtype_bytes);

}
