#pragma once
#ifndef SERVER_SOCKET
#define SERVER_SOCKET

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/unistd.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <signal.h>

#define MAXLINK 2048
class ServerSocket {
private:
	int listenfd;
	struct sockaddr_in serveraddr;

public:
	ServerSocket(uint16_t port) {
		bzero(&serveraddr, sizeof serveraddr);
		serveraddr.sin_family = AF_INET;
		//ipv4协议簇
		serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
		//INADDR_ANY泛指本机的所有ip，htonl将点分字符串地址转换为long长整型
		serveraddr.sin_port = htons(port);
		//将一个无符号短整型数值转换为网络字节序
		
	}
	~ServerSocket(){}
	int init() {
         listenfd = socket(AF_INET, SOCK_STREAM, 0);
		//ipv4协议簇和tcp流
		if (-1 == listenfd) {
			perror("socket创建失败");
			return -1;
		}
		if (-1 == bind(listenfd, (struct sockaddr*)&serveraddr, sizeof serveraddr)) {
			perror("绑定端口失败");
			return -1;
		}
		if (-1 == listen(listenfd, MAXLINK)) {
			perror("监听端口失败");
			return -1;
		}

	}
	const int& getfd () const {
		return listenfd;

	}
	
};
#endif // !1