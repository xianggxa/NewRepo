#pragma once
#ifndef SERVER
#define SERVER

#include "serverepoll.h"
#include "serversocket.h"
//#include "servermysql.h"

#define PORT 80
int serverlistenfd;

 void serverclose(int p){
	   close(serverlistenfd);
		printf("server close\n");
		exit(0);

	}

class Server {
    ServerSocket* serversocket;
	ServerEpoll* serverepoll;


public:
	
	void run() {
		signal(SIGINT, serverclose);
		ServerMysql* servermysql = ServerMysql::getinstance();
		servermysql->_init("web_server");
		serversocket = new ServerSocket(PORT);
		serversocket->init();
		serverlistenfd = serversocket->getfd();
		serverepoll = new ServerEpoll(serversocket->getfd());
		serverepoll->start();
		

	}



};

#endif // SERVER
