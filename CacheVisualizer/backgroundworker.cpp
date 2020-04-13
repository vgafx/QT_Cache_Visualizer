#include "backgroundworker.h"

BackgroundWorker::BackgroundWorker(ExecutionSimulation *worker_sim, int upd_delay)
    : delay(upd_delay), wrk_sim(*worker_sim)
{
    this->stop_flag = false;
    this->pause_flag = false;
    this->update_finished = true;
}


void BackgroundWorker::run(){

    while (!wrk_sim.isSimulationComplete()){
        if (update_finished){
            update_finished = false;
            std::vector<update_cline_info<unsigned int, unsigned long long>>& wrk_upd = wrk_sim.getUpdateInfoFromBlock();
            if (wrk_upd.front().set_idx == NORET && wrk_upd.front().tag == NORET){//No more visual updates produced
                qDebug("Update NORET\n");
            } else {
                /*sync point*/
                emit guiUpdate(wrk_upd);
            }
        }

        if (this->stop_flag){
            qDebug("run::stopflag\n");
            emit hasFinished(true);
            break;
        }

        while(pause_flag){
            qDebug("In pause flag loop\n");
            this->sleep(10);
        }

    }

    if(!this->stop_flag){
        emit hasFinished(false);
        this->exit();
    }

    this->exit();
}

void BackgroundWorker::handlePause()
{
    this->pause_flag = (this->pause_flag) ? false : true;
}

void BackgroundWorker::handleStop()
{
    this->stop_flag = true;
}

void BackgroundWorker::handleUpdateFinish()
{
    wrk_sim.popBlockUpdate();
    this->update_finished = true;
}
