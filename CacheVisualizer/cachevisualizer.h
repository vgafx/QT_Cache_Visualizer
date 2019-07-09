#ifndef CACHEVISUALIZER_H
#define CACHEVISUALIZER_H

#include <QMainWindow>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QLineEdit>

#include "fileio.h"
#include "view.h"
#include "statuscontroller.h"
#include "simulation.h"

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

    void on_actionDebug_Action_triggered();

private:
    Ui::CacheVisualizer *ui;
    QString currentFile="";
    QGraphicsScene *scene;
    View *l2View;
    //QLineEdit lineEditStatusBar;

    void populateScene();
    void populateSceneSectored();
    void populateSceneNormal();
    void setupMatrix();
    void createStatusBar();
public slots:
    void updateStatusBar(QString sts);
};

#endif // CACHEVISUALIZER_H
