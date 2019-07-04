#ifndef STATUSCONTROLLER_H
#define STATUSCONTROLLER_H

#include <QObject>

class statusController: public QObject
{
    Q_OBJECT
public:
    statusController();
    QString stsTxt;
    void setStatusText(QString intxt);

signals:
    void clickSignal(QString txt);
};

#endif // STATUSCONTROLLER_H
