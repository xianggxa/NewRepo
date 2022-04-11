#pragma once
#ifndef TIMER
#define TIMER

#define MAX_TIME_LIMIT 300

#include<time.h>
#include<algorithm>
#include<queue>

class time_node {
private:
	time_t node_time;
	int connfd;

public:
	time_node(int nfd);
	time_t get_time() const { return node_time; };
	bool isTimeout();
	int getfd();


	

};
struct time_cmp {
	bool operator ()(const time_node& t_1,
					 const time_node& t_2)const {
		return t_1.get_time() > t_2.get_time();
	}
};


class timer
{
private:
	std::priority_queue<time_node,std::vector<time_node>,time_cmp> time_q;
public:
	timer();
	void add(int nfd);
	time_node get_top();
	bool isEmpty();
	void pop();
};

#endif // TIMER