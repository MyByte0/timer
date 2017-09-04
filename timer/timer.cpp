// ConsoleApplication1.cpp : 定义控制台应用程序的入口点。
//

#include <stdint.h>
#include <functional>
#include <list>
#include <map>
#include <iostream> 
#include <thread>
#include "timer.h"

static timer* InstantTimer = new timer();

#ifdef _WIN32
static VOID CALLBACK Func(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	InstantTimer->step();
}
#else
static void Func(int sig)
{
	InstantTimer->step();
	alarm(1);
}
#endif 

timer::timer():cur_time(), event_base(), sys_time(), state(NORMAL)
{

	pthread = new std::thread(run);
}

timer::~timer()
{
	state = PAUSE;

#ifdef _WIN32
	pthread->join();
	delete pthread;
#else

#endif 

	for (auto task : event_base)
	{
		for (auto pjob : *task.second)
		{
			delete pjob;
		}
	}
}
timer* timer::getInstand()
{
	return InstantTimer;
}

void timer::run()
{
	InstantTimer->real_run();
}

#ifdef _WIN32
void timer::real_run()
{

	sys_time = SetTimer(NULL, 1, 1000, Func);
	while (GetMessage(&msg, NULL, NULL, NULL))
	{
		if (msg.message == WM_TIMER && state == NORMAL)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
}
}
#else
void timer::real_run()
{
  signal(SIGALRM, Func);
  alarm(1);
}
#endif 


void timer::pause()
{
	state = PAUSE;
}
void timer::resume()
{
	state = NORMAL;
}

timer::timer_event_t timer::add_event(time_t time, timer_callback_t func, void* arg)
{
	job_t* pjob = new job_t();
	pjob->fun = func;
	pjob->arg = arg;
	pjob->interval = time;

	add_pjob(pjob);
	return (timer_event_t)pjob;
}
void timer::del_event(timer_event_t event)
{
	job_t* pjob = (job_t*)event;

	time_t time = pjob->tigger_time;
	auto it = event_base.find(time);
	if (it == event_base.end())
	{
		return;
	}
	it->second->remove(pjob);
	if(it->second->empty())
	{
		event_base.erase(it);
	}
}

void timer::step()
{
	++cur_time;
	if (tigger_condition())
	{
		tigger_event();
	}
}
bool timer::tigger_condition()
{
	auto task = event_base.begin();
	if (task != event_base.end())
	{
		return task->first == cur_time;
	}
	return false;
}

void timer::tigger_event()
{
	auto task = event_base.begin();
	for (auto pjob : *task->second)
	{
		pjob->fun(pjob->arg);
		add_pjob(pjob);
	}
	event_base.erase(event_base.begin());
}
void timer::add_pjob(job_t* pjob)
{
	if (!pjob || !pjob->interval)
	{
		delete pjob;
		return;
	}
	pjob->tigger_time = pjob->interval + cur_time;
	auto it = event_base.find(cur_time + pjob->interval);
	if (it != event_base.end())
	{
		pjob->ptask = it->second;
		it->second->push_back(pjob);
	}
	else
	{
		task_t* ptask = new task_t();
		ptask->push_front(pjob);
		event_base.insert(std::pair<time_t, task_t*>(cur_time + pjob->interval, ptask));
		pjob->ptask = ptask;
	}
}
