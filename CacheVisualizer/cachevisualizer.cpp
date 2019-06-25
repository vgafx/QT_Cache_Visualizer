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
    if (missingConfigAttribute){
        QMessageBox::warning(this, "Warning", "Configuration not loaded properly!\n Some of the predifined attributes are missing from the configuration file.\n Consult the provided default file and restart the application");
        exit(0);
    }
    setCentralWidget(uiGrid);
    for (int r = 0; r < 1; r++) {
        for (int c = 0; c < 1; c++) {
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
    //!!Maybe clean up before quitting
    QApplication::quit();
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
        return;
    }
    //setWindowTitle(trace_fname);
    QTextStream in(&file);
    QString text = in.readAll();
    //!! Do something with the trace data

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
    QMessageBox::information(this, "Info", "Changing the configuration file will force the application to re-draw the visual output. This might take a while..." );

    QString config_fname = QFileDialog::getOpenFileName(this, tr("Open configuration file"), "", tr("Trace Files (*)"));
    QFile file(config_fname);
    if(!file.open(QIODevice::ReadOnly | QFile::Text)){
        QMessageBox::warning(this, "Warning", "Cannot Open file : " + file.errorString());
        return;
    }
    QTextStream in(&file);
    bool result = readConfigFromQstream(in);

    if(result){
        printf("Configuration file read succesfully\n!");
        QMessageBox::information(this, "Info", "Configuration File loaded successfully!\n" );
    } else {
        QMessageBox::warning(this, "Warning", "Operation aborted!\nOne or more required attributes is missing from the specified configuration file.\nLoad a file that adheres to the default specifications");
    }
    file.close();

    //!! Re-draw the scene here...

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
        QMessageBox::warning(this, "Warning", "Simulation data not ready for saving. Complete a simulation and then use this option." );
        return;
    } else {
        QString simResultsName = QFileDialog::getSaveFileName(this, "Save As");
        QFile file(simResultsName);
        if(!file.open(QFile::WriteOnly | QFile::Text)){
            QMessageBox::warning(this, "Warning", "Cannot save file : " + file.errorString());
            return;
        }
        QTextStream out(&file);
        //!! fill QString text with output data
        QString text;
        out << text;
        file.close();

    }
}



