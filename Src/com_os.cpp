#include "com_os.h"
#include "com_time.h"
#include "stm32g473xx.h"
#include "string.h"
#include <cstdint>
#include <cstring>

com_thread::com_thread(const char *name, uint8_t priority,
                       void (*fun)(void *para))
    : Priority(priority), Fun(fun)
{
    State = Thread_State::Terminated;
    memcpy(Name, name, 10);

    Context.r0 = 0;
    Context.r1 = 0;
    Context.r2 = 0;
    Context.r3 = 0;
    Context.r12 = 0;
    Context.lr = 0xFFFFFFFD;
    Context.pc = (uint32_t)Fun;
    Context.xpsr = 0x01000000;
}

com_thread::com_thread() { State = Thread_State::Terminated; }

uint8_t com_thread::operator=(com_thread &thread)
{
    if (this == &thread)
    {
        return COM_ERROR;
    }
    memcpy(Name, thread.Name, 10);
    memcpy(&Context, &thread.Context, sizeof(thread_context));
    Priority = thread.Priority;
    Fun = thread.Fun;
    State = Thread_State::Terminated;
    return COM_OK;
}

inline void com_thread::Ready() { State = Thread_State::Ready; }

inline void com_thread::Terminate() { State = Thread_State::Terminated; }

inline void com_thread::Suspend() { State = Thread_State::Suspended; }

com_thread *current_thread_ptr;
com_thread *next_thread_ptr;

com_os::com_os()
{
    thread_count = 0;
    thread_ptr = &threads[0];
    thread_running_ptr = &default_thread;
    State = Scheduler_State::Init;
    com_thread temp_thread("Default", 0, null_fun);
    default_thread = temp_thread;
}

// com_os::com_os(volatile uint32_t *tick_ptr, uint16_t period)
//     : com_time(tick_ptr, period)
// {
//     thread_count = 0;
//     thread_ptr = &threads[0];
//     thread_running_ptr = &default_thread;
//     State = Scheduler_State::Init;
//     com_thread temp_thread("Default", 0, null_fun);
//     default_thread = temp_thread;
// }

void com_os::Init_time(volatile uint32_t *tick_ptr, uint16_t period)
{
    com_time::Init(tick_ptr, period);
    thread_count = 0;
    thread_ptr = &threads[0];
    thread_running_ptr = &default_thread;
    State = Scheduler_State::Init;
    com_thread temp_thread("Default", 0, null_fun);
    default_thread = temp_thread;
}

/**
 * @brief  Create a new thread
 * @param  name: the name of the thread
 * @param  priority: the priority of the thread
 * @param  fun: the function of the thread
 * @note   The thread should be created dynamically
 * @retval the thread created
 */
com_thread &com_os::new_thread(const char *name, uint8_t priority,
                               void (*fun)(void *para))
{
    com_thread thread(name, priority, fun);
    *thread_ptr = thread;
    if (++thread_count <= COM_OS_MAX_THREAD)
    {
        thread_ptr->State = com_thread::Thread_State::Ready;
        thread_ptr++;
        return *thread_ptr;
    }
    return default_thread;
}

/**
 * @brief  Create a new thread
 * @param  thread: the thread to be created
 * @note   The thread should be created statically
 * @retval COM_OK: success，COM_ERROR: fail
 */
uint8_t com_os::new_thread(com_thread &thread)
{
    *thread_ptr = thread;
    if (++thread_count <= COM_OS_MAX_THREAD)
    {

        thread_ptr->State = com_thread::Thread_State::Blocked;
        if (State == Scheduler_State::Running)
        {
            State = Scheduler_State::Sort;
        }
        thread_ptr++;
        return COM_OK;
    }
    return COM_ERROR;
}

uint8_t com_os::delete_thread(com_thread &thread)
{
    thread.State = com_thread::Thread_State::Terminated;
    thread.Priority = 0;
    if (State == Scheduler_State::Running)
    {
        State = Scheduler_State::Sort;
    }
    return COM_OK;
}

void com_os::Delay(uint32_t ms)
{
    thread_running_ptr->Tick = ms;
    thread_running_ptr->State = com_thread::Thread_State::Blocked;
    // Scheduler();
}

void com_os::Scheduler()
{
    while (1)
    {
        switch (State)
        {
        case Scheduler_State::Init:
        {
            // com_lib::Insertion_Sort<com_thread,
            // uint8_t,&com_thread::Priority>(threads, thread_count);
            for (uint8_t i = 0; i < thread_count; i++)
            {
                if (threads[i].State != com_thread::Thread_State::Terminated)
                {
                    threads[i].State = com_thread::Thread_State::Blocked;
                }
                else
                {
                    break;
                }
            }
            State = Scheduler_State::Sort;
        }
        break;
        case Scheduler_State::Sort:
        {
            // com_lib::Insertion_Sort<com_thread,
            // uint8_t,&com_thread::Priority>(threads, thread_count);
            com_thread temp_thread;
            for (uint8_t i = 0; i < COM_OS_MAX_THREAD; i++)
            {
                for (uint8_t j = 0; j < thread_count - 2; j++)
                {
                    if (threads[j].Priority > threads[j + 1].Priority &&
                        threads[j].State !=
                            com_thread::Thread_State::Terminated &&
                        threads[j + 1].State !=
                            com_thread::Thread_State::Terminated)
                    {
                        temp_thread = threads[j];
                        threads[j] = threads[j + 1];
                        threads[j + 1] = temp_thread;
                    }
                }
            }
            State = Scheduler_State::Running;
        }
        break;
        case Scheduler_State::Running:
        {
            for (uint8_t i = 0; i < thread_count; i++)
            {
                if (threads[i].State == com_thread::Thread_State::Running)
                {
                    Switch(threads[i]);
                    break;
                }
            }
            for (uint8_t i = 0; i < thread_count; i++)
            {
                if (threads[i].State == com_thread::Thread_State::Ready)
                {
                    threads[i].State = com_thread::Thread_State::Running;
                    break;
                }
            }
            OSTime_Update();
        }
        break;
        default:
            break;
        }
    }
}

void com_os::Switch(com_thread &thread)
{
    next_thread_ptr = &thread;

    if (SchType == Scheduler_Type::TimeSlice)
    {
        thread_running_ptr = &thread;
        thread.Fun(NULL);
        thread_running_ptr = &default_thread;
        thread.State = com_thread::Thread_State::Blocked;
    }
    else if (SchType == Scheduler_Type::Preemptive)
    {
        SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;

        // __DSB();

        // __ISB();

        // TODO
    }
    else if (SchType == Scheduler_Type::NonPreemptive)
    {
        thread_running_ptr = &thread;
        thread.Fun(NULL);
        thread_running_ptr = &default_thread;
        thread.State = com_thread::Thread_State::Blocked;
    }
}

void com_os::OSTime_Update()
{
    time_update();
    uint32_t error = get_time_ms_count() - tick_count;
    for (uint8_t i = 0; i < thread_count; i++)
    {
        if (threads[i].State == com_thread::Thread_State::Blocked)
        {
            threads[i].Tick -= error;
            if (threads[i].Tick <= 0)
            {
                threads[i].State = com_thread::Thread_State::Ready;
            }
        }
    }
    tick_count = get_time_ms_count();
}

// extern "C"
// {
//     void PendSV_Handler(void) __attribute__((naked));
//     void PendSV_Handler(void)
//     {
//         __asm volatile(
//             // 保存上下文
//             "MRS     r0, PSP                \n" // 获取当前进程栈指针
//             "STMDB   r0!, {r4-r11}          \n" // 保存r4-r11到堆栈

//             // 如果使用FPU，添加浮点寄存器保存
//             // "VSTMDB  r0!, {s16-s31}      \n"

//             // 保存PSP到当前任务结构
//             "LDR     r1, =current_task_ptr  \n"
//             "LDR     r1, [r1]               \n"
//             "STR     r0, [r1]               \n" // 保存栈指针

//             // 获取下一个任务
//             "PUSH    {r0, lr}               \n"
//             "BL      next_thread_ptr     \n" // 调用调度函数
//             "POP     {r0, lr}               \n"

//             // 加载新任务的上下文
//             "LDR     r1, =current_task_ptr  \n"
//             "STR     r0, [r1]               \n" // 设置新的当前任务
//             "LDR     r0, [r0]               \n" // 读取新任务的PSP

//             // 恢复浮点寄存器(如果使用)
//             // "VLDMIA  r0!, {s16-s31}      \n"

//             // 恢复寄存器
//             "LDMIA   r0!, {r4-r11}          \n"
//             "MSR     PSP, r0                \n" // 更新PSP

//             // 返回到线程模式
//             "BX      lr                     \n");
//     }
// }

void null_fun(void *para) {}
