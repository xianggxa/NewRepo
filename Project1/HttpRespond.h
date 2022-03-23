#pragma once
#ifndef HTTP_RESPOND
#define HTTP_RESPOND

#include"httpparsing.h"
#include"tools.h"
#include"servermysql.h"
#include"cookieverify.h"
#include <codecvt>
#include <memory.h>
#include <dirent.h>
#include <unordered_map>
#include <algorithm>

#define ARTIST_PAGE_MAX 20
#define WORK_PAGE_MAX 20
#define P_PAGE_MAX 50


class HttpRespond
{
	HttpParsing* httpparsing;

public:
	HttpRespond(HttpParsing* httpparsing);

	~HttpRespond();

	void respond(int connfd);


};


#endif // !HTTP_RESPOND


