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
    for (int t=0; t<15;t++) {
        std::random_device rd;
        std::mt19937 rng(rd());
        std::uniform_int_distribution<int> uni(0,way_size_l2);
        auto random_integer = uni(rng);
        qDebug("Random Integer is: %d\n",random_integer);
    }


    QApplication a(argc, argv);

    CacheVisualizer w;
    w.show();


    return a.exec();
}
