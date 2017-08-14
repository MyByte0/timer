#include "..\timer.h"
#include <stdio.h>

void fun(void* arg)
{
	printf("time run\n");
}

int main()
{
	timer* ptimer = timer::getInstand();
	ptimer->add_event(1, fun, NULL);
	ptimer->run();
	while (1);
	return 0;
}