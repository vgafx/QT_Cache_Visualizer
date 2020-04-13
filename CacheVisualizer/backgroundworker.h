/*** Background Worker
 * Class inheriting from QThread. Its purpose is to automatically produce cacheline update
 * information to be consumed by the visualizer. The thread of this type runs in the background
 * while the main (GUI) thread receives updates(for the Autoplay mode) from this thread.
 ***/
#pragma once
#include <QThread>
#include "executionsimulation.h"

class BackgroundWorker : public QThread
{
    Q_OBJECT

public:
    BackgroundWorker(ExecutionSimulation *w_sim, int upd_delay);
    void run();

signals:
    void guiUpdate(std::vector<update_cline_info<unsigned int, unsigned long long>>& wrk_upd);
    void hasFinished(bool stopped);

public slots:
    void handlePause();
    void handleStop();
    void handleUpdateFinish();

private:
    int delay;
    ExecutionSimulation& wrk_sim;
    bool pause_flag;
    bool stop_flag;
    bool update_finished;

};

