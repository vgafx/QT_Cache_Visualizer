#ifndef THREADBLOCK_H
#define THREADBLOCK_H

//declare
class threadBlock
{
public:
    threadBlock(int threadNum, int idx, int idy, int dim, int tx, int ty, int sm);

    int getNumWarps() const;
    void setNumWarps(int value);

    int getNumThreadsZ() const;
    void setNumThreadsZ(int value);

    int getNumThreadsY() const;
    void setNumThreadsY(int value);

    int getNumThreadsX() const;
    void setNumThreadsX(int value);

    int getNumThreads() const;
    void setNumThreads(int value);

    int getMappedToSM() const;
    void setMappedToSM(int value);

    int getBlockId() const;
    void setBlockId(int value);

    int getBlockIdX() const;
    void setBlockIdX(int value);

    int getBlockIdY() const;
    void setBlockIdY(int value);

    int getBlockIdZ() const;
    void setBlockIdZ(int value);

    int getBlockDim() const;
    void setBlockDim(int value);

    int getConcurrentWarps() const;
    void setConcurrentWarps(int value);

    bool getRetired() const;
    void setRetired(bool value);

    bool getRunning() const;
    void setRunning(bool value);

private:
    int blockDim;
    int blockId;
    int blockIdX;
    int blockIdY;
    int blockIdZ;
    int mappedToSM;
    int numThreads;
    int numThreadsX;
    int numThreadsY;
    int numThreadsZ;
    int numWarps;
    long long nextAddress;
    int concurrentWarps;
    bool retired;
    bool running;
    //Data Structure for holding the instruction stream



};

#endif // THREADBLOCK_H
