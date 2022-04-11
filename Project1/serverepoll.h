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
#include "timer.h"
class ServerEpoll
{
private:

	struct epoll_event _epev;
	//��ʱ�¼�
	int _epollfd;
	//epoll�ļ�������
	struct epoll_event ep_events[EP_EVENTS_SIZE];
	//�������¼�����
	int _event_count;
	//�������¼�����
	int _listenfd;

	std::threadpool* serthreadpool;

	std::map<int, ClientHandle*> socket_clienthandle_map;

	timer* server_timer;//��ʱ��

	std::mutex mtx;
public:
	ServerEpoll(int _listenfd) {
		this->_listenfd = _listenfd;
		_epollfd = epoll_create(10);
		//����epoll������
		_epev.data.fd = _listenfd;
		_epev.events = EPOLLIN;
		//���ü����¼�
		epoll_ctl(_epollfd, EPOLL_CTL_ADD, _listenfd, &_epev);
		//����ʼ_listenfd���¼���������
		serthreadpool = new std::threadpool(THREAD_POLL_SIZE);

		server_timer = new timer();
	}
	~ServerEpoll(){
		delete serthreadpool;
	
	}
	void handle_Timeout_event() {//����ʱ����
		while (!server_timer->isEmpty()) {
			time_node n_node = server_timer->get_top();
			if (n_node.isTimeout()) {
				int dfd = n_node.getfd();
				if (socket_clienthandle_map.find(dfd) != socket_clienthandle_map.end()) {
					std::lock_guard<std::mutex> mylock_guard(mtx);
					epoll_ctl(_epollfd, EPOLL_CTL_DEL, dfd, nullptr);
					//�Ƴ�����
					close(dfd);
					socket_clienthandle_map.erase(dfd);
				}
				server_timer->pop();

			}
			else break;

		}

	}
	void start() {
		printf("server start\n");
		while (1) {
			_event_count = epoll_wait(_epollfd, ep_events, EP_EVENTS_SIZE, -1);
			//�����ȴ��¼�
			for (int i = 0; i < _event_count; i++) {

				if (ep_events[i].data.fd == _listenfd) { // ���µ����ӽ���
					
					int newfd = accept(_listenfd, nullptr, nullptr);
					printf("newconnection %d\n", newfd);
					_epev.data.fd = newfd;
					_epev.events = EPOLLIN | EPOLLET;
					epoll_ctl(_epollfd, EPOLL_CTL_ADD, newfd, &_epev);
					ClientHandle* clienthandle = new ClientHandle();
					socket_clienthandle_map[newfd] = clienthandle;

				}
				else if (ep_events[i].events & EPOLLIN) {
					printf("old \n");
					ClientHandle* clienthandle = socket_clienthandle_map[ep_events[i].data.fd];
					int nowfd = ep_events[i].data.fd;

					std::future<ClientHandle*> fu = serthreadpool->commit(ClientHandle::stahandle, clienthandle, nowfd);
					//��http��Ӧ��������̳߳��������
					ClientHandle* _handle = fu.get();
					if (_handle->getconnection()==false) {//���ӶϿ�
						printf("close fd:%d\n", nowfd);
						int dfd = _handle->getfd();
						if (socket_clienthandle_map.find(dfd) != socket_clienthandle_map.end()) {
						std::lock_guard<std::mutex> mylock_guard(mtx);
						epoll_ctl(_epollfd, EPOLL_CTL_DEL, dfd, nullptr);
						//�Ƴ�����
						close(dfd);
						socket_clienthandle_map.erase(dfd);
						}

						delete clienthandle;
					}
					



				}
			}
			handle_Timeout_event();
		}

	}



};
#endif // !1



