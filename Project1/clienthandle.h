#pragma once
#ifndef CLIENT_HANDLE_H
#define CLIENT_HANDLE_H


#include <string>
#include "httpparsing.h"
#include "HttpRespond.h"

class ClientHandle{
	private:
		std::string _username;//用户姓名
		
		std::string _userrights;//用户权限

		HttpParsing* _httpparsing;//http解析对象

		HttpRespond* _httprespond;//http响应对象

		bool connection;

		int connfd;
public:
	ClientHandle();

	~ClientHandle(); 

	void requesthandle(int connfd);

	static ClientHandle* stahandle(ClientHandle* ptr, int connfd);

	const bool& getconnection() const;

	const int& getfd() const;


};
#endif // !1




