#include "timer.h"

time_node::time_node(int nfd)
{
	connfd = nfd;
	node_time = time(NULL);
}

bool time_node::isTimeout()
{
	time_t nowtime = time(NULL);
	return nowtime - node_time > MAX_TIME_LIMIT;
}

int time_node::getfd()
{
	return connfd;
}

timer::timer()
{
}

void timer::add(int nfd)
{
	time_q.push(time_node(nfd));
}

time_node timer::get_top()
{
	if (!time_q.empty()) {
		return time_q.top();
	}
	else return NULL;
}

bool timer::isEmpty()
{
	return time_q.empty();
}

void timer::pop()
{
	if (!time_q.empty()) {
		time_q.pop();
	}
}
