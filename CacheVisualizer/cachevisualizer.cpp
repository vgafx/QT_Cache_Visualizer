#include "cachevisualizer.h"
#include "ui_cachevisualizer.h"
#include "globals.h"
#include "cacheline.h"
#include "view.h"
#include <math.h>

#include <QLabel>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QSplitter>


CacheVisualizer::CacheVisualizer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CacheVisualizer)
{
    ui->setupUi(this);
    //QWidget *uiGrid = new QWidget;
    //QGridLayout *layout = new QGridLayout;
    if (missingConfigAttribute){
        QMessageBox::critical(this, "Warning", "Configuration not loaded properly!\n Some of the predifined attributes are missing from the configuration file.\n Consult the provided default file and restart the application");
        exit(0);
    }
    //populateScene();
    //populateSceneSectored();
    populateSceneNormal();
    l2View = new View("L2 Cache");
    l2View->view()->setScene(scene);

    l2View->show();
    setCentralWidget(l2View);
}

CacheVisualizer::~CacheVisualizer()
{
    delete ui;
}


void CacheVisualizer::populateScene(){
    scene = new QGraphicsScene(this);
    scene->setBackgroundBrush(Qt::white);

    //Used config Data to draw the scene
    int xx = 0, yy =0;
    int numLines = 0;

    for (int i = -11000; i < 11000; i += 110) {
        ++xx;
        for (int j = -7000; j < 7000; j += 70) {
            ++yy;

            QColor color(Qt::gray);
            QGraphicsItem *item = new cacheline(color, xx, yy,0);
            item->setPos(QPointF(i, j));
            scene->addItem(item);

            ++numLines;
        }
    }
    printf("DREW: %d \n", numLines);
}


void CacheVisualizer::populateSceneSectored(){
    scene = new QGraphicsScene(this);
    scene->setBackgroundBrush(Qt::white);

    //Used config Data to draw the scene
    int xx = 0, yy =0;
    int numLines = 0;

    for (int i = 0; i < 0; i+=1 ){
        for (int j = 0; j < 0; j+=1){

        }
    }



//    for (int i = -11000; i < 11000; i += 110) {
//        ++xx;
//        for (int j = -7000; j < 7000; j += 70) {
//            ++yy;

//            QColor color(Qt::gray);
//            QGraphicsItem *item = new cacheline(color, xx, yy);
//            item->setPos(QPointF(i, j));
//            scene->addItem(item);

//            ++numLines;
//        }
//    }
    printf("DREW: %d \n", numLines);
}

void CacheVisualizer::populateSceneNormal(){
    scene = new QGraphicsScene(this);
    scene->setBackgroundBrush(Qt::white);

    //Used config Data to draw the scene
    int xx = -1, yy =0;
    int numSets = 0;
    int i = 0, j = 0;
    int j_start = 0, i_start = 0, j_end = 0, i_inc = 40, j_inc = 380, i_gap = 40 * 2;
    int i_next_line = i_gap + (i_inc * way_size_l2);
    int i_iter = way_size_l2 * i_inc;
    int extra_lines = 0;

    //num_sets_l2 = 8192;
    //Have to check if display dimensions of the set (i.e. 4096 sets = 64 x 64
    int dispDims = int(sqrt(num_sets_l2));
    int wideDimsCol = dispDims + (dispDims / 2);
    int j_total_range = wideDimsCol * j_inc;
    j_total_range /= 2;
    j_start = - j_total_range;
    j_end = j_total_range;
    j = j_start;

    int wideDimesRow = int(num_sets_l2 / wideDimsCol);
    if (num_sets_l2 % wideDimsCol != 0){
        extra_lines = 1;
    }
    int i_total_range = (wideDimesRow + extra_lines) * i_inc;
    i_total_range /= 2;
    i_start = -i_total_range;
    i = i_start;

    for (int s = 1; s < num_sets_l2+1; s++){
        ++xx;
        for (int l = i; l < i + i_iter; l+=i_inc) {
            QColor color(Qt::gray);
            QGraphicsItem *line = new cacheline(color, xx, yy, s-1);
            line->setPos(QPointF(j, l));
            scene->addItem(line);
            ++yy;
        }
        yy -= way_size_l2;
        if(s % wideDimsCol == 0){
            j = j_start;
            i += i_next_line;
            xx=-1;
            yy+=way_size_l2;
        } else {
            j+=j_inc;
        }
    }

    //REVERSE i and j if uncommenting this
//    int i_total_range = dispDims * i_inc;
//    i_total_range /= 2;
//    i_start = -i_total_range;
//    i_end = i_total_range;
//    i = i_start;

//    int j_total_range = (dispDims + extraLines) * j_inc;
//    j_total_range /= 2;
//    j_start = -j_total_range;
//    j = j_start;
//    for (int s = 1; s < num_sets_l2+1; s++){
//        for (int l = j; l < j + j_iter; l+=j_inc) {
//            QColor color(Qt::gray);
//            QGraphicsItem *line = new cacheline(color, xx, yy, s-1);
//            line->setPos(QPointF(i, l));
//            scene->addItem(line);
//        }

//        if(s % dispDims == 0){
//            i = i_start;
//            j += j_next_line;
//        } else {
//            i+=i_inc;
//        }
//    }

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
        QMessageBox::critical(this, "Warning", "Cannot Open file : " + file.errorString());
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
    if(!trace_loaded){
        //QMessageBox::error(this, "Warning", "Cannot Open file : ");
        QMessageBox::critical(this, "No Trace Data", "No Trace data were loaded for simulation.\nLoad a trace file first and try again");
    } else {

    }

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
    scene->clear();
    //populateScene();
    //populateSceneSectored();
    populateSceneNormal();
    l2View->view()->setScene(scene);

    //l2View->show();
    //l2View
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
        QMessageBox::critical(this, "Warning", "Cannot Open file : " + file.errorString());
        return;
    }
    QTextStream in(&file);
    bool result = readConfigFromQstream(in);

    if(result){
        printf("Configuration file read succesfully\n!");
        QMessageBox::information(this, "Info", "Configuration File loaded successfully!\n" );
    } else {
        QMessageBox::critical(this, "Warning", "Operation aborted!\nOne or more required attributes is missing from the specified configuration file.\nLoad a file that adheres to the default specifications");
    }
    file.close();

    //!! Re-draw the scene here...

}

/*Menu Bar Trigger
  Create the dialog to dispaly the simulation configuration
*/
void CacheVisualizer::on_actionPrint_Configuration_triggered()
{
    QString gpuName = QString::fromStdString(name);
    QString arc = QString::fromStdString(architecture);
    QString chipset = QString::fromStdString(chip);
    QString l1active = (is_l1_enabled == 1) ? "Yes":"No";
    QString l2sectored = (is_sectored == 1) ? "Yes":"No";
    QString replacementPol = (replacement_policy == 1) ? "LRU": "Random within Set";
    QString writePol = (write_policy == 1) ? "Write-Allocate": "Non-Cached";

    QMessageBox::about (this, "Configuration Summary", "GPU Name:                                            " + gpuName +
                        "\nArchitecture:                                        " + arc +
                        "\nChip:                                                        " + chipset +
                        "\n------------------------------------------------------------------------------------------"  +
                        "\nNumber of SMs:                                  " + QString::number(num_sm) +
                        "\nCuda Cores(total):                              " + QString::number(cuda_cores) +
                        "\nWarp Size:                                             " + QString::number(warp_size) +
                        "\nMax Active Warps(per SM):             " + QString::number(concurrent_warps) +
                        "\n------------------------------------------------------------------------------------------"  +
                        "\nGlobal Memory(bytes):                    " + QString::number(global_size) +
                        "\nL2 Size(bytes):                                     " + QString::number(l2_size) +
                        "\nL1 Size(bytes):                                     " + QString::number(l1_size) +
                        "\nCache Line Size(bytes):                    " + QString::number(line_size) +
                        "\nL1 Enabled?:                                         " + l1active +
                        "\nL2 Sectored?:                                       " + l2sectored +
                        "\nSector Size:                                           " + QString::number(sector_size) +
                        "\nSet Associativety(L1):                       " + QString::number(way_size_l1) +"-way"+
                        "\nSet Associativety(L2):                       " + QString::number(way_size_l2) +"-way"+
                        "\nLoad Granularity(L1):                       " + QString::number(l1_load_granularity) +
                        "\nLoad Granularity(L2):                       " + QString::number(l2_load_granularity) +
                        "\n------------------------------------------------------------------------------------------"  +
                        "\nNumber of Sets(L2):                         " + QString::number(num_sets_l2) +
                        "\nNumber of Cache Lines(L2):          " + QString::number(num_clines_l2) +
                        "\nBytes per Set(L2):                             " + QString::number(bytes_in_set_l2) +
                        "\nNumber of Sets(L1):                         " + QString::number(num_sets_l1) +
                        "\nNumber of Cache Lines(L1):          " + QString::number(num_clines_l1) +
                        "\nBytes per Set(L1):                             " + QString::number(bytes_in_set_l1) );

}

/*Menu Bar Trigger
  Prints Application information
*/
void CacheVisualizer::on_actionAbout_triggered()
{
    QMessageBox::about (this, "About this Application", "Name:                GPU Cache Visualizer\nAuthor:              Vasileios Gkanasoulis\nVersion:             1.0.0\nCreated with:  QT Framework\nGitHub:              https://github.com/vgafx/QT_Cache_Visualizer\n \nCreated for the requirements of Master Thesis in CS:PCS,\nSupervisor: Pieter Hijma");
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
            QMessageBox::critical(this, "Warning", "Cannot save file : " + file.errorString());
            return;
        }
        QTextStream out(&file);
        //!! fill QString text with output data
        QString text;
        out << text;
        file.close();

    }
}



