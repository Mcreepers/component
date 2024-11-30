#ifndef __MESSAGE_TASK_H__
#define __MESSAGE_TASK_H__

#include "com_printf.h"
#include "com_uart.h"
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
extern void messagetask(void *argument);
#ifdef __cplusplus
}
#endif

#include "component.h"
// namespace{

class cMessage : public com_printf, public com_uart {
public:
  cMessage();
  void Task();

private:
  uint8_t Heart;
};

// } // namespace task
#endif