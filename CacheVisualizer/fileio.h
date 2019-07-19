#ifndef FILEIO_H
#define FILEIO_H
#include <QTextStream>
#include <QMessageBox>
#include "simulation.h"


class fileIO
{
public:
    fileIO();

};

extern std::map<std::string,int> config_att;


void readConfig();
bool readConfigFromQstream(QTextStream &stream);
bool readTraceDataFromQstream(QTextStream &stream, simulation *sim);
void initConfigMap();

#endif // FILEIO_H
