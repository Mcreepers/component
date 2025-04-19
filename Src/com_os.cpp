#include "com_os.h"
#include "string.h"

com_thread::com_thread(const char *name, uint8_t priority, void (*fun)(void *para))
	: Priority(priority), Fun(fun)
{
	State = Thread_State::Terminated;
	memcpy(Name, name, 10);
}

com_thread::com_thread()
{
	State = Thread_State::Terminated;
}

uint8_t com_thread::operator =  (com_thread &thread)
{
	if(this == &thread)
	{
		return COM_ERROR;
	}
	memcpy(Name, thread.Name, 10);
	Priority = thread.Priority;
	Fun = thread.Fun;
	State = Thread_State::Terminated;
	return COM_OK;
}

void com_thread::Ready()
{
	State = Thread_State::Ready;
}

void com_thread::Terminate()
{
	State = Thread_State::Terminated;
}

void com_thread::Suspend()
{
	State = Thread_State::Suspended;
}

com_os::com_os()
{
	thread_count = 0;
	thread_ptr = &threads[0];
	State = Scheduler_State::Init;
}

com_thread &com_os::new_thread(const char *name, uint8_t priority, void (*fun)(void *para))
{
	com_thread thread(name, priority, fun);
	*thread_ptr = thread;
	if(++thread_count <= COM_OS_MAX_THREAD)
	{
		thread_ptr++;
		thread_count++;
		thread_ptr->State = com_thread::Thread_State::Ready;
		return *thread_ptr;
	}
	return default_thread;
}

uint8_t com_os::new_thread(com_thread &thread)
{
	*thread_ptr = thread;
	if(++thread_count <= COM_OS_MAX_THREAD)
	{
		thread_ptr++;
		thread_count++;
		thread_ptr->State = com_thread::Thread_State::Blocked;
		if(State == Scheduler_State::Running)
		{
			State = Scheduler_State::Sort;
			Scheduler();
		}

		return COM_OK;
	}
	return COM_ERROR;
}

uint8_t com_os::delete_thread(com_thread &thread)
{
	for(uint8_t i = 0; i < thread_count; i++)
	{
		if(threads[i].Name == thread.Name)
		{
			threads[i].State = com_thread::Thread_State::Terminated;
			if(State == Scheduler_State::Running)
			{
				State = Scheduler_State::Sort;
				Scheduler();
			}
			return COM_OK;
		}
	}
	return COM_ERROR;
}

void com_os::Scheduler()
{
	if(State == Scheduler_State::Sort || State == Scheduler_State::Init)
	{
		// com_lib::Insertion_Sort<com_thread, uint8_t, &com_thread::Priority>(threads, thread_count);
		State = Scheduler_State::Running;
	}
	switch(State)
	{
	case Scheduler_State::Init:
	// com_lib::Insertion_Sort<com_thread, uint8_t, &com_thread::Priority>(threads, thread_count);
	for(uint8_t i = 0; i < thread_count; i++)
	{
		if(threads[i].State != com_thread::Thread_State::Terminated)
		{
			threads[i].State = com_thread::Thread_State::Blocked;
		}
		else
		{
			break;
		}
	}
	threads[0].State = com_thread::Thread_State::Ready;
	break;
	case Scheduler_State::Sort:
	{
		// com_lib::Insertion_Sort<com_thread, uint8_t, &com_thread::Priority>(threads, thread_count);
	}
	break;
	case Scheduler_State::Running:
	{

	}
	break;
	default:
	break;
	}
	if(State == Scheduler_State::Init)
	{
	}

	for(uint8_t i = 0; i < thread_count; i++)
	{
		if(threads[i].State == com_thread::Thread_State::Ready)
		{
			Switch(threads[i]);
		}
	}

	for(uint8_t i = 0; i < thread_count; i++)
	{
		if(threads[i].State == com_thread::Thread_State::Ready)
		{
			threads[i].State = com_thread::Thread_State::Running;
			break;
		}
	}

	for(uint8_t i = 0; i < thread_count; i++)
	{
		if(threads[i].State == com_thread::Thread_State::Blocked)
		{
			threads[i].State = com_thread::Thread_State::Ready;
			break;
		}
	}
}

void com_os::Switch(com_thread &thread)
{
	return ;
}
