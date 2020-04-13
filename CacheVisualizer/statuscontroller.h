/* Dummy class used to connect the cacheline objects to the status bar.
 * Basically this was created to emit SIGNALs for status bar updates
 * to CacheVisualizer SLOT. All of the cacheline objects use this object (by passing a ref on init)
 * as an intermediary. Otherwise, each cacheline would have to have its own signal. Since the status
 * update is on-click, this solution is fine.
*/
#pragma once

#include <QObject>

class StatusController: public QObject
{
    Q_OBJECT
public:
    StatusController();
    QString sts_txt;
    void setStatusText(QString intxt);

signals:
    void clickSignal(QString txt);
};
