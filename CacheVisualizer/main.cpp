#include "cachevisualizer.h"
#include <QApplication>
#include <string>
#include "fileio.h"
#include "globals.h"

//using namespace std;

int main(int argc, char *argv[])
{
    //exit(0);
    readConfig();
    printf("Configuration finished...\n");
    computeConfig();
    printGlobals();

    QApplication a(argc, argv);
    CacheVisualizer w;
    w.show();

    return a.exec();
}
