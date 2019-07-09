#ifndef THREADBLOCK_H
#define THREADBLOCK_H

//declare
class threadBlock
{
public:
    threadBlock();
    unsigned int getNumWarps() const;
    void setNumWarps(unsigned int value);
    unsigned int getNumThreadsZ() const;
    void setNumThreadsZ(unsigned int value);

    unsigned int getNumThreadsY() const;
    void setNumThreadsY(unsigned int value);

    unsigned int getNumThreadsX() const;
    void setNumThreadsX(unsigned int value);

    unsigned int getNumThreads() const;
    void setNumThreads(unsigned int value);

    unsigned int getMappedToSM() const;
    void setMappedToSM(unsigned int value);

    unsigned int getBlockId() const;
    void setBlockId(unsigned int value);

    unsigned int getBlockIdX() const;
    void setBlockIdX(unsigned int value);

    unsigned int getBlockIdY() const;
    void setBlockIdY(unsigned int value);

    unsigned int getBlockIdZ() const;
    void setBlockIdZ(unsigned int value);

    unsigned int getBlockDim() const;
    void setBlockDim(unsigned int value);

    int getConcurrentWarps() const;
    void setConcurrentWarps(int value);

private:
    unsigned int blockDim;
    unsigned int blockId;
    unsigned int blockIdX;
    unsigned int blockIdY;
    unsigned int blockIdZ;
    unsigned int mappedToSM;
    unsigned int numThreads;
    unsigned int numThreadsX;
    unsigned int numThreadsY;
    unsigned int numThreadsZ;
    unsigned int numWarps;
    unsigned long long nextAddress;
    int concurrentWarps;
    //Data Structure for holding the instruction stream



};

#endif // THREADBLOCK_H
