#include <QThread>
#include "backgroundworker.h"

backgroundworker::backgroundworker(simulation *worker_sim, int upd_delay)
{
    this->delay = upd_delay;
    this->wrk_sim = worker_sim;

}

void backgroundworker::startAutoSim()
{
    while (!wrk_sim->isSimulationComplete()){

        std::list<update_line_info> wrk_upd;
        wrk_upd = wrk_sim->getUpdateInfoFromBlock();
        if (wrk_upd.empty()){//No more visual updates produced

        } else {
            emit guiUpdate(wrk_upd);
        }
    }

}

void backgroundworker::run(){
    while (!wrk_sim->isSimulationComplete()){
        qDebug("Before sleep\n");
        this->sleep(1);
        qDebug("Before sleep\n");
        std::list<update_line_info> wrk_upd;
        wrk_upd = wrk_sim->getUpdateInfoFromBlock();
        if (wrk_upd.empty()){//No more visual updates produced
            qDebug("Update Info Empty\n");
        } else {
            qDebug("Emmiting SIGNAL from worker!\n");
            emit guiUpdate(wrk_upd);
        }
    }
    emit hasFinished(true);
}
