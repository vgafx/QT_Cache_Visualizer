#include <QApplication>
#include <string>
#include <random>
#include <iostream>

#include "cachevisualizer.h"
#include "settings.h"
#include "datadefinitions.h"
#include "filehandler.h"


int main(int argc, char *argv[])
{

    QApplication a(argc, argv);

    CacheVisualizer w;
    w.show();


    return a.exec();
}
