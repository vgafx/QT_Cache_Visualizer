#include <QThread>
#include "backgroundworker.h"

backgroundworker::backgroundworker(simulation *worker_sim, int upd_delay)
{
    this->delay = upd_delay;
    this->wrk_sim = worker_sim;
    this->stopFlag = false;
    this->pauseFlag = false;
}


void backgroundworker::run(){
    while (!wrk_sim->isSimulationComplete()){
        this->msleep(this->delay);
        std::list<update_line_info> wrk_upd;
        wrk_upd = wrk_sim->getUpdateInfoFromBlock();
        if (wrk_upd.empty()){//No more visual updates produced
            //qDebug("Update Info Empty\n");
            wrk_sim->getInstructionCounts();
        } else {
            //qDebug("Emmiting SIGNAL from worker!\n");
            emit guiUpdate(wrk_upd);
        }

        if (this->stopFlag){
            qDebug("run::stopflag\n");
            this->wrk_sim->cleanAll();
            emit hasFinished(true);
            break;
        }

        while(pauseFlag){
            qDebug("In pause flag loop\n");
            this->sleep(1);
        }
    }

    if(!this->stopFlag){
        emit hasFinished(true);
        this->wrk_sim->cleanAll();
        this->exit();
    }
}

void backgroundworker::handlePause()
{
    //qDebug("Worker pause flag flipped!\n");
    this->pauseFlag = (this->pauseFlag) ? false : true;
}

void backgroundworker::handleStop()
{
    //qDebug("Worker stop flag set!\n");
    this->stopFlag = true;
}
