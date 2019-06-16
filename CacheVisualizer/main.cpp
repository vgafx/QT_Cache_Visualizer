#include "cachevisualizer.h"
#include <QApplication>

int l2size;

int main(int argc, char *argv[])
{







    QApplication a(argc, argv);
    CacheVisualizer w;
    w.show();

    return a.exec();
}
