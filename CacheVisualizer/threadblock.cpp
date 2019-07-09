#include "threadblock.h"

//define
threadBlock::threadBlock()
{

}


/*Setters and Getters*/
unsigned int threadBlock::getNumWarps() const
{
    return numWarps;
}

void threadBlock::setNumWarps(unsigned int value)
{
    numWarps = value;
}

unsigned int threadBlock::getNumThreadsZ() const
{
    return numThreadsZ;
}

void threadBlock::setNumThreadsZ(unsigned int value)
{
    numThreadsZ = value;
}

unsigned int threadBlock::getNumThreadsY() const
{
    return numThreadsY;
}

void threadBlock::setNumThreadsY(unsigned int value)
{
    numThreadsY = value;
}

unsigned int threadBlock::getNumThreadsX() const
{
    return numThreadsX;
}

void threadBlock::setNumThreadsX(unsigned int value)
{
    numThreadsX = value;
}

unsigned int threadBlock::getNumThreads() const
{
    return numThreads;
}

void threadBlock::setNumThreads(unsigned int value)
{
    numThreads = value;
}

unsigned int threadBlock::getMappedToSM() const
{
    return mappedToSM;
}

void threadBlock::setMappedToSM(unsigned int value)
{
    mappedToSM = value;
}

unsigned int threadBlock::getBlockId() const
{
    return blockId;
}

void threadBlock::setBlockId(unsigned int value)
{
    blockId = value;
}

unsigned int threadBlock::getBlockIdX() const
{
    return blockIdX;
}

void threadBlock::setBlockIdX(unsigned int value)
{
    blockIdX = value;
}

unsigned int threadBlock::getBlockIdY() const
{
    return blockIdY;
}

void threadBlock::setBlockIdY(unsigned int value)
{
    blockIdY = value;
}

unsigned int threadBlock::getBlockIdZ() const
{
    return blockIdZ;
}

void threadBlock::setBlockIdZ(unsigned int value)
{
    blockIdZ = value;
}

unsigned int threadBlock::getBlockDim() const
{
    return blockDim;
}

void threadBlock::setBlockDim(unsigned int value)
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


