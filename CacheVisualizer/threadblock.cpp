#include "threadblock.h"

//define
threadBlock::threadBlock(int threadNum, int id, int dim, int tx, int ty, int sm, int cc_warps)
{
    this->numThreads = threadNum;
    this->blockDim = dim;
    this->blockId = id;
    this->numThreadsX = tx;
    this->numThreadsY = ty;
    this->mappedToSM = sm;
    this->concurrentWarps = cc_warps;
    this->retired = false;
    this->running = false;
}


/*Setters and Getters*/
int threadBlock::getNumWarps() const
{
    return numWarps;
}

void threadBlock::setNumWarps(int value)
{
    numWarps = value;
}

int threadBlock::getNumThreadsZ() const
{
    return numThreadsZ;
}

void threadBlock::setNumThreadsZ(int value)
{
    numThreadsZ = value;
}

int threadBlock::getNumThreadsY() const
{
    return numThreadsY;
}

void threadBlock::setNumThreadsY(int value)
{
    numThreadsY = value;
}

int threadBlock::getNumThreadsX() const
{
    return numThreadsX;
}

void threadBlock::setNumThreadsX(int value)
{
    numThreadsX = value;
}

int threadBlock::getNumThreads() const
{
    return numThreads;
}

void threadBlock::setNumThreads(int value)
{
    numThreads = value;
}

int threadBlock::getMappedToSM() const
{
    return mappedToSM;
}

void threadBlock::setMappedToSM(int value)
{
    mappedToSM = value;
}

int threadBlock::getBlockId() const
{
    return blockId;
}

void threadBlock::setBlockId(int value)
{
    blockId = value;
}

int threadBlock::getBlockIdX() const
{
    return blockIdX;
}

void threadBlock::setBlockIdX(int value)
{
    blockIdX = value;
}

int threadBlock::getBlockIdY() const
{
    return blockIdY;
}

void threadBlock::setBlockIdY(int value)
{
    blockIdY = value;
}

int threadBlock::getBlockIdZ() const
{
    return blockIdZ;
}

void threadBlock::setBlockIdZ(int value)
{
    blockIdZ = value;
}

int threadBlock::getBlockDim() const
{
    return blockDim;
}

void threadBlock::setBlockDim(int value)
{
    blockDim = value;
}

int threadBlock::getConcurrentWarps() const
{
    return concurrentWarps;
}

void threadBlock::setConcurrentWarps(int value)
{
    concurrentWarps = value;
}

bool threadBlock::getRetired() const
{
    return retired;
}

void threadBlock::setRetired(bool value)
{
    retired = value;
}

bool threadBlock::getRunning() const
{
    return running;
}

void threadBlock::setRunning(bool value)
{
    running = value;
}


