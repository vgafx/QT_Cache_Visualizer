#ifndef BACKGROUNDWORKER_H
#define BACKGROUNDWORKER_H
#include <QThread>
#include "simulation.h"

class backgroundworker : public QThread
{
    Q_OBJECT

public:
    backgroundworker(simulation *wrkSim, int upd_delay);
    void run();

signals:
    void guiUpdate(std::list<update_line_info> wrk_upd);
    void hasFinished(bool fin);
//!!implement these
public slots:
    void handlePause();
    void handleStop();

private:
    int delay;
    bool pauseFlag;
    bool stopFlag;
    simulation *wrk_sim;
};

#endif // BACKGROUNDWORKER_H
