#include <QThread>
#include "backgroundworker.h"

backgroundworker::backgroundworker(simulation *worker_sim, int upd_delay)
{
    this->delay = upd_delay;
}

void backgroundworker::startAutoSim()
{

}
