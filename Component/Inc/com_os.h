#ifndef __COM_OS_H__
#define __COM_OS_H__

#include "component.h"
#include "com_time.h"

//#include ""

#define COM_OS_MAX_THREAD 10
#define COM_OS_MAX_STACK_SIZE 1024

class com_os;
class com_thread
{
public:
	com_thread(const char *name, uint8_t priority, void (*fun)(void *para));
	uint8_t operator = (com_thread &thread);
	void Ready();
	void Terminate();
	void Suspend();
protected:
	com_thread();
	com_thread(char *name, uint8_t priority, void (*fun)(void *para));
	friend class com_os;

	char *Name[10];
	uint8_t Priority;
	void (*Fun)(void *para);

	uint16_t Tick;
	uint32_t Top_Stack[16];

	enum class Thread_State
	{
		Terminated,
		Ready,
		Running,
		Suspended,
		Blocked
	}State;
};

class com_os
{
public:
	com_os();
	com_thread &new_thread(const char *name, uint8_t priority, void (*fun)(void *para));
	uint8_t new_thread(com_thread &thread);
	uint8_t delete_thread(com_thread &thread);
	void Scheduler();
private:
	void Switch(com_thread &thread);

	uint8_t thread_count;

	com_thread default_thread;
	com_thread *thread_ptr;
	com_thread *threads_ptr[10];
	com_thread threads[COM_OS_MAX_THREAD];
	uint8_t Stack[COM_OS_MAX_STACK_SIZE];

	enum class Scheduler_State
	{
		Init,
		Sort,
		Running,
		Suspended,
		Stopped
	}State;
};

#endif