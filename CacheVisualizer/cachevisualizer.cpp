#include "cachevisualizer.h"
#include "ui_cachevisualizer.h"
#include "globals.h"
#include <QLabel>
#include <QGridLayout>

CacheVisualizer::CacheVisualizer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CacheVisualizer)
{
    ui->setupUi(this);
    QWidget *uiGrid = new QWidget;
    QGridLayout *layout = new QGridLayout;
    setCentralWidget(uiGrid);
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            QLabel *label = new QLabel("1", uiGrid); // Text could be 1 or 0.
            layout->addWidget(label, r, c);
        }
    }
    uiGrid->setLayout(layout);

}

CacheVisualizer::~CacheVisualizer()
{
    delete ui;
}

void CacheVisualizer::on_actionExit_triggered()
{
    printf("Exit Option Pressed\n");
    exit(0);
}

/*Menu Bar Trigger
  Create the dialog to open a trace file for simulation
*/
void CacheVisualizer::on_actionOpen_Trace_triggered()
{
    QString trace_fname = QFileDialog::getOpenFileName(this, tr("Open trace"), "", tr("Trace Files (*.trc)"));
    QFile file(trace_fname);
    currentFile = trace_fname;
    if(!file.open(QIODevice::ReadOnly | QFile::Text)){
        QMessageBox::warning(this, "Warning", "Cannot Open file : " + file.errorString());
    }
    //setWindowTitle(trace_fname);
    QTextStream in(&file);
    QString text = in.readAll();

    file.close();


}

/*Menu Bar Trigger
  Starts the simulation of the trace data
*/
void CacheVisualizer::on_actionStart_triggered()
{

}

/*Menu Bar Trigger
  Pauses the simulation
*/
void CacheVisualizer::on_actionPause_triggered()
{

}

/*Menu Bar Trigger
  Stops the simulation
*/
void CacheVisualizer::on_actionStop_triggered()
{

}

/*Menu Bar Trigger
  Clears all simulation data
*/
void CacheVisualizer::on_actionClear_triggered()
{

}

/*Menu Bar Trigger
  Change configuration file
*/
void CacheVisualizer::on_actionChange_Configuration_File_triggered()
{

}

/*Menu Bar Trigger
  Create the dialog to dispaly the simulation configuration
*/
void CacheVisualizer::on_actionPrint_Configuration_triggered()
{

}

/*Menu Bar Trigger
  Prints Application information
*/
void CacheVisualizer::on_actionAbout_triggered()
{

}

void CacheVisualizer::on_actionSave_Simulation_Results_triggered()
{
    if(!simulation_done){
        printf("Simulation Data not ready!\n");
        //Pop a dialogue here

    } else {

    }
}



