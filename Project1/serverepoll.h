#pragma once
#ifndef SERVER_EPOLL
#define SERVER_EPOLL
#define EP_EVENTS_SIZE 1024
#define THREAD_POLL_SIZE 4

#include <sys/epoll.h>
#include <sys/socket.h>
#include <map>
#include "clienthandle.h"
#include "threadpool.h"
#include <unistd.h>
class ServerEpoll
{
private:

	struct epoll_event _epev;
	//临时事件
	int _epollfd;
	//epoll文件描述符
	struct epoll_event ep_events[EP_EVENTS_SIZE];
	//待处理事件队列
	int _event_count;
	//待处理事件个数
	int _listenfd;

	std::threadpool* serthreadpool;

	std::map<int, ClientHandle*> socket_clienthandle_map;
public:
	ServerEpoll(int _listenfd) {
		this->_listenfd = _listenfd;
		_epollfd = epoll_create(10);
		//创建epoll描述符
		_epev.data.fd = _listenfd;
		_epev.events = EPOLLIN;
		//设置监听事件
		epoll_ctl(_epollfd, EPOLL_CTL_ADD, _listenfd, &_epev);
		//将初始_listenfd的事件添加入监听
		serthreadpool = new std::threadpool(THREAD_POLL_SIZE);
	}
	~ServerEpoll(){
		delete serthreadpool;
	
	}
	void start() {
		printf("server start\n");
		while (1) {
			_event_count = epoll_wait(_epollfd, ep_events, EP_EVENTS_SIZE, -1);
			//阻塞等待事件
			for (int i = 0; i < _event_count; i++) {

				if (ep_events[i].data.fd == _listenfd) { // 有新的连接建立
					
					int newfd = accept(_listenfd, nullptr, nullptr);
					printf("newconnection %d\n", newfd);
					_epev.data.fd = newfd;
					_epev.events = EPOLLIN;
					epoll_ctl(_epollfd, EPOLL_CTL_ADD, newfd, &_epev);
					ClientHandle* clienthandle = new ClientHandle();
					socket_clienthandle_map[newfd] = clienthandle;

				}
				else if (ep_events[i].events & EPOLLIN) {
					printf("old \n");
					ClientHandle* clienthandle = socket_clienthandle_map[ep_events[i].data.fd];
					int nowfd = ep_events[i].data.fd;

					std::future<ClientHandle*> fu = serthreadpool->commit(ClientHandle::stahandle, clienthandle, nowfd);
					//将http响应处理加入线程池任务队列

					if (fu.get()->getconnection()==false) {//连接断开

						epoll_ctl(_epollfd, EPOLL_CTL_DEL, nowfd, nullptr);
						//移除监听
						close(nowfd);
						socket_clienthandle_map.erase(nowfd);

						delete clienthandle;



					}



				}
			}
		}

	}



};
#endif // !1



