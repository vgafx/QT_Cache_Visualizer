#include "simulation.h"

simulation::simulation()
{
    this->stopFlag = false;
    this->pauseFlag = false;
    this->readyToStart = false;
    this->numBlocks = 0;
    this->threadsPerBlock = 0;
}

int simulation::getNumBlocks() const
{
    return numBlocks;
}

void simulation::setNumBlocks(int value)
{
    numBlocks = value;
}

int simulation::getThreadsPerBlock() const
{
    return threadsPerBlock;
}

void simulation::setThreadsPerBlock(int value)
{
    threadsPerBlock = value;
}

bool simulation::getPauseFlag() const
{
    return pauseFlag;
}

void simulation::setPauseFlag(bool value)
{
    pauseFlag = value;
}

bool simulation::getReadyToStart() const
{
    return readyToStart;
}

void simulation::setReadyToStart(bool value)
{
    readyToStart = value;
}

bool simulation::getStopFlag() const
{
    return stopFlag;
}

void simulation::setStopFlag(bool value)
{
    stopFlag = value;
}

int simulation::getBlocks_z() const
{
    return blocks_z;
}

void simulation::setBlocks_z(int value)
{
    blocks_z = value;
}

int simulation::getBlocks_y() const
{
    return blocks_y;
}

void simulation::setBlocks_y(int value)
{
    blocks_y = value;
}

int simulation::getBlocks_x() const
{
    return blocks_x;
}

void simulation::setBlocks_x(int value)
{
    blocks_x = value;
}

int simulation::getThreads_x() const
{
    return threads_x;
}

void simulation::setThreads_x(int value)
{
    threads_x = value;
}

int simulation::getThreads_y() const
{
    return threads_y;
}

void simulation::setThreads_y(int value)
{
    threads_y = value;
}

int simulation::getThreads_z() const
{
    return threads_z;
}

void simulation::setThreads_z(int value)
{
    threads_z = value;
}
