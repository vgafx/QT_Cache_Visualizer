#include <QApplication>
#include <string>
#include "fileio.h"
#include "globals.h"
#include "cachevisualizer.h"
#include <random>

int main(int argc, char *argv[])
{

    initConfigMap();
    readConfig();
    qDebug("Configuration finished...\n");
    computeConfig();
    printGlobals();


    QApplication a(argc, argv);

    CacheVisualizer w;
    w.show();


    return a.exec();
}
