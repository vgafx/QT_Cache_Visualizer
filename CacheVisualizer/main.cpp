#include <QApplication>
#include <string>
#include "fileio.h"
#include "globals.h"
#include "cachevisualizer.h"

int main(int argc, char *argv[])
{
    //exit(0);

    initConfigMap();
    readConfig();
    printf("Configuration finished...\n");
    computeConfig();
    printGlobals();

    QApplication a(argc, argv);
    //a.setWindowIcon(QIcon(":App.png"));

    CacheVisualizer w;
    w.show();


    return a.exec();
}
