#include "cachevisualizer.h"
#include <QApplication>
#include <string>

int main(int argc, char *argv[])
{







    QApplication a(argc, argv);
    CacheVisualizer w;
    w.show();

    return a.exec();
}
