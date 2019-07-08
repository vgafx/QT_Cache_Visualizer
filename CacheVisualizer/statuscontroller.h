/* Dummy class used to connect the cacheline objects to the status bar.
 * Basically this was created to emit SIGNALs for status bar updates
 * to CacheVisualizer SLOT. All of the cacheline objects use this object (by passing a ref on init)
 * as an intermediary. Otherwise, each cacheline would have to have its own signal. Since the status
 * update is on-click, this solution is fine.
*/

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
