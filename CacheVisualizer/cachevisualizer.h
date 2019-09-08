#ifndef CACHEVISUALIZER_H
#define CACHEVISUALIZER_H

#include <QMainWindow>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QLineEdit>
#include <QActionGroup>

#include "fileio.h"
#include "view.h"
#include "statuscontroller.h"
#include "simulation.h"
#include "statistics.h"

class QGraphicsScene;
class QSplitter;

namespace Ui {
class CacheVisualizer;
}

class CacheVisualizer : public QMainWindow
{
    Q_OBJECT

public:
    statusController *stsC;
    simulation *mySim;
    statistics *myStatistics;
    explicit CacheVisualizer(QWidget *parent = nullptr);
    ~CacheVisualizer();

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
    Ui::CacheVisualizer *ui;
    //QString currentFile=""; //!!why is this here?
    QGraphicsScene *scene;
    View *l2View;
    QActionGroup *m_e_group;

    void populateScene();
    void populateSceneSectored();
    void populateSceneNormal();
    void setupMatrix();
    void createStatusBar();
public slots:
    void updateStatusBar(QString sts);
    void handleWorkerThreadUpdate(std::list<update_line_info> wrk_upd);
    void handleWorkerThreadFinished(bool fin);

signals:
    void sendPauseSignal();
    void sendStopSignal();

};

#endif // CACHEVISUALIZER_H
