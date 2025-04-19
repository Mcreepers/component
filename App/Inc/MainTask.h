#ifndef __MAIN_TASK_H__
#define __MAIN_TASK_H__

#include "com_statistic.h"
#ifdef __cplusplus
extern "C" {
#endif
void maintask(void *argument);
void com_main(void *argument);
#ifdef __cplusplus
}
#endif

#include "component.h"

class cMain:public com_statistic
{
    public:
    cMain();
    void Task();
    private:
    
};
#endif