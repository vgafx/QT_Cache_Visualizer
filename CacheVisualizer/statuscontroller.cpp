#include "statuscontroller.h"

statusController::statusController()
{
    this->stsTxt = "";
}

void statusController::setStatusText(QString intxt){
    printf("Emmiting signal!\n");
    emit clickSignal(intxt);
}
