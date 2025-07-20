#ifndef __COM_OS_H__
#define __COM_OS_H__

#include "com_time.h"
// #include "component.h"

#include <cstdint>
#include <stdint.h>
// #include ""

void null_fun(void *para);

#define COM_OS_MAX_THREAD 10
#define COM_OS_MAX_STACK_SIZE 1 // 1024

class com_os;

struct thread_context
{
    // 硬件自动保存的寄存器
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r12;
    uint32_t lr;
    uint32_t pc;
    uint32_t xpsr;

    // 需要手动保存的寄存器
    uint32_t r4;
    uint32_t r5;
    uint32_t r6;
    uint32_t r7;
    uint32_t r8;
    uint32_t r9;
    uint32_t r10;
    uint32_t r11;
};

class com_thread
{
  public:
    com_thread(const char *name, uint8_t priority, void (*fun)(void *para));
    ~com_thread()
    {
        State = Thread_State::Terminated;
        Priority = 0;
        Fun = null_fun;
        Tick = 0;
    }
    uint8_t operator=(com_thread &thread);
    inline void Ready();
    inline void Terminate();
    inline void Suspend();

  protected:
    com_thread();

    com_thread(char *name, uint8_t priority, void (*fun)(void *para));
    friend class com_os;

    char Name[10];
    uint8_t Priority;
    void (*Fun)(void *para);

    int32_t Tick;
    thread_context Context;

    enum class Thread_State
    {               // state of thread
        Terminated, // Init state
        Ready,      // waiting for running
        Running,    // running
        Suspended,  // end of running
        Blocked     // wait time to ready
    } State;
};

class com_os : private com_time
{
  public:
    com_os();
    // com_os(volatile uint32_t *tick_ptr, uint16_t period);
    void Init_time(volatile uint32_t *tick_ptr, uint16_t period);
    com_thread &new_thread(const char *name, uint8_t priority,
                           void (*fun)(void *para));
    uint8_t new_thread(com_thread &thread);
    uint8_t delete_thread(com_thread &thread);

    void Delay(uint32_t ms);

    void Scheduler();

    uint8_t GetScheduler_Type() { return (uint8_t)SchType; }
    uint32_t GetTickCount() { return get_time_ms_count(); }

  private:
    void Switch(com_thread &thread);
    void OSTime_Update();
    void Default_fun(void *para);

    uint8_t thread_count;
    uint32_t tick_count;

    com_thread default_thread;
    com_thread *thread_running_ptr;
    com_thread *thread_ptr;
    com_thread threads[COM_OS_MAX_THREAD];
    // uint8_t Stack[COM_OS_MAX_STACK_SIZE];

    enum class Scheduler_State
    {
        Init,
        Sort,
        Running,
        Suspended,
        Stopped
    } State;

    enum class Scheduler_Type
    {
        TimeSlice,
        Preemptive,
        NonPreemptive,
    } SchType = Scheduler_Type::TimeSlice;
};

#endif