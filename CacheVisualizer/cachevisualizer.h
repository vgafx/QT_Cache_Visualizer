#ifndef CACHEVISUALIZER_H
#define CACHEVISUALIZER_H

#include <QMainWindow>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>


namespace Ui {
class CacheVisualizer;
}

class CacheVisualizer : public QMainWindow
{
    Q_OBJECT

public:
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

private:
    Ui::CacheVisualizer *ui;
    QString currentFile="";
};

#endif // CACHEVISUALIZER_H
