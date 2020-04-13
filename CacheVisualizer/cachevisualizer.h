/*** Cache Visualizer
 *  The application's main window. Handles communication, user actions, and
 *  generally glues everything together.
 ***/
#pragma once

#include <QMainWindow>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QLineEdit>
#include <QActionGroup>
#include <QLabel>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QActionGroup>
#include <QMetaType>
#include <math.h>
#include <utility>
#include <map>
#include <fstream>

#include "executionsimulation.h"
#include "cacheline.h"
#include "cache.h"
#include "view.h"
#include "statuscontroller.h"
#include "simulationstatistics.h"
#include "backgroundworker.h"
#include "filehandler.h"
#include "settings.h"
#include "runtimeflags.h"
#include "scopedtimer.h"

class QGraphicsScene;
class QSplitter;

namespace Ui {
class CacheVisualizer;
}

class CacheVisualizer : public QMainWindow
{
    Q_OBJECT

public:
    StatusController* sts_c;
    SimulationStatistics* statistics;
    FileHandler* f_handler;
    ExecutionSimulation* e_sim;
    Cache* l2_cache;

    explicit CacheVisualizer(QWidget *parent = nullptr);
    ~CacheVisualizer();

/*Handling user actions*/
private slots:
    void on_actionExit_triggered();

    void on_actionOpen_Trace_triggered();

    void on_actionStart_triggered();

    void on_actionPause_triggered();

    void on_actionStop_triggered();

    void on_actionClear_triggered();

    void on_actionChange_Configuration_File_triggered();

    void on_actionPrint_Configuration_triggered();

    void on_actionAbout_triggered();

    void on_actionSave_Simulation_Results_triggered();

    void on_autoplay_triggered();

    void on_actionManual_Step_wise_triggered();

    void on_actionNext_Step_triggered();

private:
    Ui::CacheVisualizer* ui;
    QGraphicsScene* scene;
    View* l2View;
    QActionGroup* m_e_group;

    const std::string cname = "configuration";

    void populateScene();
    void populateSceneSectored();
    void populateSceneNormal();
    void setupMatrix();
    void createStatusBar();

public slots:
    void updateStatusBar(QString sts);
    void handleWorkerThreadUpdate(std::vector<update_cline_info<unsigned int, unsigned long long>>& wrk_upd);
    void handleWorkerThreadFinished(bool stopped);

signals:
    void sendPauseSignal();
    void sendStopSignal();
    void sendUpdateFinishedSignal();

};
