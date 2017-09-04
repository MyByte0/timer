#include  "../timer/timer.h"
#include <stdio.h>

timer::timer_event_t g_event;

void fun(void* arg)
{
	static int i_count = 0;
	++i_count;
	if(i_count<10)
	{
	  printf("time run\n");
	}
	else{
	  timer* ptimer = (timer*)arg;
      ptimer->del_event(g_event);
	  printf("time pause\n");
	}
}

int main()
{
	timer* ptimer = timer::getInstand();
	g_event = ptimer->add_event(1, fun, ptimer);
	ptimer->run();
	while (1);
	return 0;
}