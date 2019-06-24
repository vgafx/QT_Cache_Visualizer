#ifndef THREADBLOCK_H
#define THREADBLOCK_H

//declare
class threadBlock
{
public:
    threadBlock();
    unsigned int blockDim;
    unsigned int blockId;
    unsigned int blockIdX;
    unsigned int blockIdY;
    unsigned int blockIdZ;
    unsigned int mappedToSM;
    unsigned int numThreads;
    unsigned int numWarps;
};

#endif // THREADBLOCK_H
