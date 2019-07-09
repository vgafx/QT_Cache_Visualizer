#ifndef SIMULATION_H
#define SIMULATION_H


class simulation
{
public:
    simulation();

    int getNumBlocks() const;
    void setNumBlocks(int value);

    int getThreadsPerBlock() const;
    void setThreadsPerBlock(int value);

    bool getPauseFlag() const;
    void setPauseFlag(bool value);

    bool getReadyToStart() const;
    void setReadyToStart(bool value);

    bool getStopFlag() const;
    void setStopFlag(bool value);

    int getBlocks_z() const;
    void setBlocks_z(int value);

    int getBlocks_y() const;
    void setBlocks_y(int value);

    int getBlocks_x() const;
    void setBlocks_x(int value);

    int getThreads_x() const;
    void setThreads_x(int value);

    int getThreads_y() const;
    void setThreads_y(int value);

    int getThreads_z() const;
    void setThreads_z(int value);

private:
    int numBlocks;
    int blocks_x;
    int blocks_y;
    int blocks_z;
    int threadsPerBlock;
    int threads_x;
    int threads_y;
    int threads_z;
    bool pauseFlag;
    bool stopFlag;
    bool readyToStart;
};

#endif // SIMULATION_H
