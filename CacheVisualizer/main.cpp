#include "cachevisualizer.h"
#include <QApplication>
#include <string>
#include "fileio.h"

//using namespace std;

int main(int argc, char *argv[])
{
    readConfig();
    printf("Configuration finished...\n");

    QApplication a(argc, argv);
    CacheVisualizer w;
    w.show();

    return a.exec();
}
