#ifndef FILEIO_H
#define FILEIO_H
#include <QTextStream>
#include <QMessageBox>


class fileIO
{
public:
    fileIO();

};
extern std::map<std::string,int> config_att;


void readConfig();
bool readConfigFromQstream(QTextStream &stream);
void initConfigMap();

#endif // FILEIO_H
