#ifndef BACKGROUNDWORKER_H
#define BACKGROUNDWORKER_H
#include <QThread>
#include "simulation.h"

class backgroundworker:QThread
{
public:
    backgroundworker(simulation *wrkSim, int upd_delay);

    void startAutoSim();

signals:
    void emitGuiUpdate(std::list<update_line_info> wrk_upd);

private:
    int delay;
};

#endif // BACKGROUNDWORKER_H
