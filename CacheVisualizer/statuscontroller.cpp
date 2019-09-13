/*Dummy object that handles toolbar updates.
  Each cachline object gets a reference of this object.
  This way we avoid connecting multiple SIGNALS to the
  same SLOT and instead we propagate updates through
  the same object.*/
#include "statuscontroller.h"

statusController::statusController()
{
    this->stsTxt = "";
}

void statusController::setStatusText(QString intxt){
    emit clickSignal(intxt);
}
