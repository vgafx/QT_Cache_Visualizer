#ifndef CACHERULES_H
#define CACHERULES_H

#include "globals.h"
#include "statistics.h"

class cacherules
{
public:
    cacherules();
};

void updateSceneFromInfo(std::list<update_line_info> up_info, statistics *stat_out);
void updateSceneFromInfoNonSectored(std::list<update_line_info> up_info, statistics *stat_out);

#endif // CACHERULES_H
