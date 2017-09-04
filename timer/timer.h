/**
 a instance timer for windows and linux
**/

#ifndef _TIMER_H_
#define _TIMER_H_

#include <stdint.h>
#include <thread>
#include <functional>
#include <list>
#include <map>
#include <iostream> 
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#include <signal.h>  
#include <sys/time.h>
#endif 

//VOID CALLBACK Func(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

class timer {
public:
	timer();
	~timer();

public:
	typedef std::function<void(void*)> timer_callback_t;
	typedef uint32_t timer_event_t;
	typedef uint64_t time_t;

public:
	static timer* getInstand();
    static void run();
	void step();
	void pause();
	void resume();
	timer_event_t add_event(time_t time, timer_callback_t event, void* arg);
	void del_event(timer_event_t event);

private:
	typedef struct {
		timer_callback_t fun;
		void* arg;
		time_t interval;
		time_t tigger_time;
		void* ptask;
	}job_t;
	typedef std::list<job_t*> task_t;
	typedef std::map<time_t, task_t*> event_base_t;

private:
	typedef enum {
		NORMAL = 0,
		PAUSE,
	}state_t;

private:
	void real_run();
	bool tigger_condition();
	void tigger_event();
	void add_pjob(job_t* pjob);

private:
	event_base_t event_base;

private:
	time_t cur_time;
	volatile state_t state;
	
	UINT_PTR sys_time;
	MSG msg;
	std::thread* pthread;
	
};

#endif