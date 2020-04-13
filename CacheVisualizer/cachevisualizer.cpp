/*The main UI class. Handles all user actions*/
#include "cachevisualizer.h"
#include "ui_cachevisualizer.h"


CacheVisualizer::CacheVisualizer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CacheVisualizer)
{
    ui->setupUi(this);


    f_handler = new FileHandler();
    f_handler->initConfigMap();

    qDebug("Reading Config File on Startup\n");
    if(!f_handler->readConfig(c_name)){
        QMessageBox::critical(this, "Warning", "Could not read the configuration file on start-up.\nThe Visualizer defaulted to hardcoded values for the GTX TitanX.\nEnsure the configuration file adheres to the specifications and manually load it or restart the application.");
    } else {
        qDebug("Configuration File read successfuly(startup)!\n");
    }

    /*Instansiate after reading config*/
    e_sim = new ExecutionSimulation();
    statistics = new SimulationStatistics();

    l2_cache = new Cache("L2 Cache",Settings::getIsSectored(),Settings::getReplacePolicy(), Settings::getWritePolicy(),statistics);

    populateSceneNormal();

    l2View = new View("L2 Cache");
    l2View->view()->setScene(scene);
    l2View->show();
    setCentralWidget(l2View);

    //autoplay
    m_e_group = new QActionGroup(this);
    QAction *act = this->menuBar()->actions().at(1);
    QAction *chld_act = act->menu()->actions().at(4);
    QAction *g_chld_act1 = chld_act->menu()->actions().at(0);
    QAction *g_chld_act2 = chld_act->menu()->actions().at(1);
    m_e_group->addAction(g_chld_act1);
    m_e_group->addAction(g_chld_act2);
    m_e_group->setExclusive(true);
    createStatusBar();

}

CacheVisualizer::~CacheVisualizer()
{
    qDebug("Destructor called\n");
    delete m_e_group;
    delete f_handler;
    delete e_sim;
    delete statistics;
    delete l2_cache;
    delete l2View;
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
            QGraphicsItem *item = new Cacheline(color, xx, yy,0,nullptr);
            item->setPos(QPointF(i, j));
            scene->addItem(item);

            ++numLines;
        }
    }
}


void CacheVisualizer::populateSceneNormal(){
    scene = new QGraphicsScene(this);
    scene->setBackgroundBrush(Qt::white);
    sts_c = new StatusController();
    connect(sts_c, &StatusController::clickSignal, this, &CacheVisualizer::updateStatusBar);
    //Used config Data to draw the scene
    int xx = -1, yy =0;
    int i = 0, j = 0;
    int j_start, i_start, i_inc = 40, j_inc = 380, i_gap = 40 * 2;
    int i_next_line = i_gap + (i_inc * Settings::getWaySizeL2());
    int i_iter = Settings::getWaySizeL2() * i_inc;
    int extra_lines = 0;

    int dispDims = static_cast<int>(std::sqrt(Settings::getNumSetsL2()));
    int wideDimsCol = dispDims + (dispDims / 2);
    int j_total_range = wideDimsCol * j_inc;
    j_total_range /= 2;
    j_start = - j_total_range;
    j = j_start;

    int wideDimesRow = int(Settings::getNumSetsL2() / wideDimsCol);
    if (Settings::getNumSetsL2() % wideDimsCol != 0){
        extra_lines = 1;
    }
    int i_total_range = (wideDimesRow + extra_lines) * i_inc;
    i_total_range /= 2;
    i_start = -i_total_range;
    i = i_start;

    for (int s = 1; s < Settings::getNumSetsL2()+1; s++){
        ++xx;
        for (int l = i; l < i + i_iter; l += i_inc) {
            QColor color(Qt::gray);
            /*The scene requires ownership of the item to work properly*/
            Cacheline *line = new Cacheline(color, xx, yy, s-1, sts_c);
            l2_cache->addCacheline(s-1, line);
            line->setPos(QPointF(j, l));
            scene->addItem(line);
            ++yy;
        }
        yy -= Settings::getWaySizeL2();
        if(s % wideDimsCol == 0){
            j = j_start;
            i += i_next_line;
            xx = -1;
            yy += Settings::getWaySizeL2();
        } else {
            j += j_inc;
        }
    }
}


void CacheVisualizer::on_actionExit_triggered()
{
    if (RuntimeFlags::getWorkerRunning()){
        QMessageBox::critical(this, "Thread active", "The background worker is still running a simulation. Use the Stop menu option to terminate it first and then Quit");
        return;
    }
    e_sim->cleanAll();
    this->~CacheVisualizer();
    QApplication::quit();
    exit(0);
}


/*Menu Bar Trigger
  Create the dialog to open a trace file for simulation*/
void CacheVisualizer::on_actionOpen_Trace_triggered()
{
    if (RuntimeFlags::getTraceLoaded()){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Trace already loaded", "A trace file is already loaded!\nSelect Yes to erase the previous trace data and load a new file\nSelect No to keep the old data and not load a new trace",QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes){
            e_sim->cleanAll();
        } else {
            return;
        }
        RuntimeFlags::setTraceLoaded(false);
    }

    QMessageBox::information(this, "Info", "Depending on the input size, opening a trace file might take a while\n" );

    QString trace_fname = QFileDialog::getOpenFileName(this, tr("Open trace"), "", tr("Trace Files (*.trc)"));

    bool result;
    std::ifstream input_stream(trace_fname.toStdString());

    if (!input_stream){
        qDebug("CANNOT OPEN IFSTREAM!\n");
        QMessageBox::critical(this, "Warning", "Cannot Open file : " + trace_fname);
    }

    {
        QScopedTimer timer("ifstream read\n");
        result = f_handler->readTraceData(input_stream, e_sim);
    }

    if(result){
        qDebug("Trace file read succesfully\n!");
        QMessageBox::information(this, "Info", "Trace File loaded successfully!\n" );
        RuntimeFlags::setTraceLoaded(true);
    } else {
        if (input_stream.is_open()) {input_stream.close();}
        e_sim->cleanAll();
        QMessageBox::critical(this, "Warning", "Operation aborted!\nSomething went wrong when reading the trace input.\nLoad a file that adheres to the default specifications");
    }

    if (input_stream.is_open()) {input_stream.close();}
}


/*Menu Bar Trigger
  Starts the simulation of the trace data*/
void CacheVisualizer::on_actionStart_triggered()
{
    if(!RuntimeFlags::getTraceLoaded()){
        QMessageBox::critical(this, "No Trace Data", "No Trace data were loaded for simulation.\nLoad a trace file first and try again.");
        return;
    }

    if(!RuntimeFlags::getSimModeSelected()){
        QMessageBox::critical(this, "No Mode Selected", "The simulation mode is not set.\nSelect a simulation mode from the menu options before running a simulation.");
        return;
    }

    if(RuntimeFlags::getWorkerRunning()){
        QMessageBox::critical(this, "Simulation Already Running", "A background worker thread is currently running a simulation.\n Either wait for its completion or use the Stop Option.");
        return;
    }

    if(RuntimeFlags::getStartFlag()){
        return;
    }

    e_sim->sortAllBlockInstructions();
    e_sim->prepareInitialBlocks();
    qDebug("Instructions Sorted & Blocks prepared\n");
    RuntimeFlags::setStartFlag(true);

    if(RuntimeFlags::getSimMode() == AUTOPLAY){

        BackgroundWorker *wrk_thread = new BackgroundWorker(e_sim, Settings::getWorkerDelay());
        qRegisterMetaType<std::vector<update_cline_info<unsigned int, unsigned long long>>>("std::vector<update_cline_info<unsigned int, unsigned long long>>&");

        connect(wrk_thread, SIGNAL(guiUpdate(std::vector<update_cline_info<unsigned int, unsigned long long>>&)), this, SLOT(handleWorkerThreadUpdate(std::vector<update_cline_info<unsigned int, unsigned long long>>&)));
        connect(wrk_thread, SIGNAL(finished()), wrk_thread, SLOT(deleteLater()));
        connect(wrk_thread, SIGNAL(hasFinished(bool)), this, SLOT(handleWorkerThreadFinished(bool)));

        connect(this, SIGNAL(sendPauseSignal()), wrk_thread, SLOT(handlePause()));
        connect(this, SIGNAL(sendStopSignal()), wrk_thread, SLOT(handleStop()));
        connect(this, SIGNAL(sendUpdateFinishedSignal()), wrk_thread, SLOT(handleUpdateFinish()));
        RuntimeFlags::setWorkerRunning(true);
        qDebug("Connections Done, starting thread\n");
        wrk_thread->start();
    } else {
        QMessageBox::information(this, "Setup Completed", "The simulation has been set up in step-wise mode\nUse the Next Step option from this menu to proceed through it.");
    }

}

/*Menu Bar Trigger
  Pauses the simulation*/
void CacheVisualizer::on_actionPause_triggered()
{
    if (RuntimeFlags::getSimMode() == STEP_WISE){
        QMessageBox::information(this, "Not Applicable", "This option has no effect when the simulation is in step-wise mode.");
    } else {
        emit sendPauseSignal();
        RuntimeFlags::setPauseFlag((RuntimeFlags::getPauseFlag()) ? false : true);
    }
}

/*Menu Bar Trigger
  Stops the simulation*/
void CacheVisualizer::on_actionStop_triggered()
{
    if (RuntimeFlags::getSimMode() == STEP_WISE){
        QMessageBox::information(this, "Not Applicable", "This option has no effect when the simulation is in step-wise mode.\nTo remove the current simulation data use the Clear option instead.");
    } else {
        if (RuntimeFlags::getPauseFlag()){
            QMessageBox::information(this, "Simulation still active in the background", "The simulation is still paused in the background.\nTo completely stop it use the Resume menu option first and then Stop.");
        } else {
            emit sendStopSignal();
        }
    }
}

/*Menu Bar Trigger
  Clears all simulation data*/
void CacheVisualizer::on_actionClear_triggered()
{
    if(RuntimeFlags::getWorkerRunning()){
        QMessageBox::critical(this, "Cannot Clear", "Simulation still running in the background.\nResume the simulation first,then use the Stop menu option and then Clear." );
        return;
    }
    /*Status controller needs to be deleted before the scene is redrawn.*/
    delete sts_c;
    statistics->reset();
    l2_cache->cleanAll();
    e_sim->cleanAll();
    scene->clear();
    populateSceneNormal();
    l2View->view()->setScene(scene);

    RuntimeFlags::setTraceLoaded(false);
    RuntimeFlags::setSimulationDone(false);
    RuntimeFlags::setWorkerRunning(false);
    RuntimeFlags::setStartFlag(false);
    RuntimeFlags::setPauseFlag(false);
    RuntimeFlags::setSimModeSelected(false);

    if (m_e_group->actions().at(0)->isChecked() || m_e_group->actions().at(1)->isChecked()){
        m_e_group->checkedAction()->setChecked(false);
    }

}

/*Menu Bar Trigger
  Change configuration file*/
void CacheVisualizer::on_actionChange_Configuration_File_triggered()
{
    if(RuntimeFlags::getWorkerRunning() || RuntimeFlags::getStartFlag()){
        QMessageBox::critical(this, "Simulation Running", "A simulation is currently running.\nStop and Clear the current simuation first");
        return;
    }

    QMessageBox::information(this, "Info", "Changing the configuration file will force the application to re-draw the visual output. This might take a while..." );

    QString config_fname = QFileDialog::getOpenFileName(this, tr("Open configuration file"), "", tr("Trace Files (*)"));

    bool result = f_handler->readConfig(config_fname.toStdString());

    if(result){
        qDebug("Configuration file read succesfully\n!");
        QMessageBox::information(this, "Info", "Configuration File loaded successfully!\n" );
    } else {
        QMessageBox::critical(this, "Warning", "Operation aborted!\nOne or more required attributes is missing from the specified configuration file.\nLoad a file that adheres to the default specifications");
        return;
    }
    qDebug("Change config-reseting data\n");
    e_sim->updateBitMask();
    scene->clear();
    populateSceneNormal();
    l2View->view()->setScene(scene);

    if(!RuntimeFlags::getWorkerRunning()) { e_sim->cleanAll(); }
    RuntimeFlags::setTraceLoaded(false);
    RuntimeFlags::setSimulationDone(false);
    RuntimeFlags::setWorkerRunning(false);
    RuntimeFlags::setStartFlag(false);
    RuntimeFlags::setPauseFlag(false);
    RuntimeFlags::setSimModeSelected(false);
    if (m_e_group->actions().at(0)->isChecked() || m_e_group->actions().at(1)->isChecked()){
        m_e_group->checkedAction()->setChecked(false);
    }
    statistics->reset();
}

/*Menu Bar Trigger
  Create the dialog to dispaly the simulation configuration*/
void CacheVisualizer::on_actionPrint_Configuration_triggered()
{
    QString gpuName = QString::fromStdString(Settings::getGpuName());
    QString arc = QString::fromStdString(Settings::getArchitecture());
    QString chipset = QString::fromStdString(Settings::getChip());
    QString l1active = (Settings::getL1Enabled() == 1) ? "Yes":"No";
    QString l2sectored = (Settings::getIsSectored() == 1) ? "Yes":"No";
    QString replacementPol = (Settings::getReplacePolicy() == 1) ? "LRU": "Random within Set";
    QString writePol = (Settings::getWritePolicy() == 1) ? "Write-Allocate": "Non-Cached";

    QMessageBox::about (this, "Configuration Summary", "GPU Name:                                            " + gpuName +
                        "\nArchitecture:                                        " + arc +
                        "\nChip:                                                        " + chipset +
                        "\n------------------------------------------------------------------------------------------"  +
                        "\nNumber of SMs:                                  " + QString::number(Settings::getNumSm()) +
                        "\nCuda Cores(total):                              " + QString::number(Settings::getCores()) +
                        "\nWarp Size:                                             " + QString::number(Settings::getWarpSize()) +
                        "\nMax Active Warps(per SM):             " + QString::number(Settings::getConcurrentWarps()) +
                        "\n------------------------------------------------------------------------------------------"  +
                        "\nGlobal Memory(bytes):                    " + QString::number(Settings::getGlobalMemSize()) +
                        "\nL2 Size(bytes):                                     " + QString::number(Settings::getL2SizeBytes()) +
                        "\nL1 Size(bytes):                                     " + QString::number(Settings::getL1SizeBytes()) +
                        "\nCache Line Size(bytes):                    " + QString::number(Settings::getClineSize()) +
                        "\nL1 Enabled?:                                         " + l1active +
                        "\nL2 Sectored?:                                       " + l2sectored +
                        "\nSector Size:                                           " + QString::number(Settings::getSectorSize()) +
                        "\nSet Associativety(L1):                       " + QString::number(Settings::getWaySizeL1()) +"-way"+
                        "\nSet Associativety(L2):                       " + QString::number(Settings::getWaySizeL2()) +"-way"+
                        "\nLoad Granularity(L1):                       " + QString::number(Settings::getClineSize()) +
                        "\nLoad Granularity(L2):                       " + QString::number(Settings::getSectorSize()) +
                        "\n------------------------------------------------------------------------------------------"  +
                        "\nNumber of Sets(L2):                         " + QString::number(Settings::getNumSetsL2()) +
                        "\nNumber of Cache Lines(L2):          " + QString::number(Settings::getNumClinesL2()) +
                        "\nBytes per Set(L2):                             " + QString::number(Settings::getNumSetsL2()) +
                        "\nNumber of Sets(L1):                         " + QString::number(Settings::getNumSetsL1()) +
                        "\nNumber of Cache Lines(L1):          " + QString::number(Settings::getNumClinesL1()) +
                        "\nBytes per Set(L1):                             " + QString::number(Settings::getBytesInSetL1()) );

}

/*Menu Bar Trigger
  Prints Application information*/
void CacheVisualizer::on_actionAbout_triggered()
{
    QMessageBox::about (this, "About this Application", "Name:                GPU Cache Visualizer\nAuthor:              Vasileios Gkanasoulis\nVersion:             1.0.0\nCreated with:  QT Framework\nGitHub:              https://github.com/vgafx/QT_Cache_Visualizer\n \nCreated for the requirements of Master Thesis in CS:PCS,\nSupervisor: Pieter Hijma");
}

void CacheVisualizer::on_actionSave_Simulation_Results_triggered()
{
    if(!RuntimeFlags::getSimulationDone()){
        qDebug("Simulation Data not ready!\n");
        QMessageBox::warning(this, "Warning", "Simulation data not ready for saving. Complete a simulation and then use this option.");
        return;
    } else {
        QString simResultsName = QFileDialog::getSaveFileName(this, "Save As");
        QFile file(simResultsName);
        if(!file.open(QFile::WriteOnly | QFile::Text)){
            QMessageBox::critical(this, "Warning", "Cannot save file : " + file.errorString());
            return;
        }
        QTextStream out(&file);
        QString text = statistics->getOutputString();
        out << text;
        file.close();
    }
}

void CacheVisualizer::createStatusBar(){
    statusBar()->showMessage(tr("Ready"));
}

/*Public Slots (2)*/
void CacheVisualizer::updateStatusBar(QString sts){
    statusBar()->showMessage(sts);
}

void CacheVisualizer::handleWorkerThreadUpdate(std::vector<update_cline_info<unsigned int, unsigned long long>>& wrk_upd)
{
    l2_cache->processUpdate(wrk_upd);
    emit sendUpdateFinishedSignal();
}

void CacheVisualizer::handleWorkerThreadFinished(bool stopped)
{
    qDebug("Received Finished from worker!\n");

    if (stopped){
        QMessageBox::information(this, "Simulation Interrupted", "The simulation was interrupted via the Stop option.\n The results are incomplete.\n Cleanup required.");
    } else {
        QMessageBox::information(this, "Simulation Completed", "The simulation has been completed. The reults can now be printed");
    }

    RuntimeFlags::setWorkerRunning(false);
    RuntimeFlags::setSimulationDone(true);
    RuntimeFlags::setStartFlag(false);
}

void CacheVisualizer::on_autoplay_triggered()
{      
    if(!RuntimeFlags::getStartFlag()){RuntimeFlags::setSimMode(static_cast<SimulationMode>(0));}
    RuntimeFlags::setSimModeSelected(true);
}

void CacheVisualizer::on_actionManual_Step_wise_triggered()
{
    if(!RuntimeFlags::getStartFlag()){RuntimeFlags::setSimMode(static_cast<SimulationMode>(1));}
    RuntimeFlags::setSimModeSelected(true);
}

void CacheVisualizer::on_actionNext_Step_triggered()
{

    if(RuntimeFlags::getSimulationDone() && RuntimeFlags::getSimMode() == STEP_WISE){
        QMessageBox::warning(this, "Warning", "The simulation has been completed.\n Clear from the menu and load a new trace to begin another simulation");
        return;
    }

    if(!RuntimeFlags::getStartFlag()){
        QMessageBox::warning(this, "Warning", "The Start menu option must be used before taking a simulation step, as it sets up crucial information!");
        return;
    }



    if(RuntimeFlags::getSimModeSelected() && RuntimeFlags::getSimMode() == STEP_WISE){
        qDebug("Taking sim step\n");
        if(e_sim->isSimulationComplete()){
            QMessageBox::information(this, "Simulation Complete", "There are no more available instructions to simulate\nThe simulation is completed");
            RuntimeFlags::setSimulationDone(true);
            RuntimeFlags::setStartFlag(false);
        }
        l2_cache->processUpdate(e_sim->getUpdateInfoFromBlock());
        e_sim->popBlockUpdate();
    }
}
