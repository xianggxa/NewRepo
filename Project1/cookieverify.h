#pragma once
#ifndef COOKIEVERIFY 
#define COOKIEVERIFY

#include <map>
#include <cstring>

class CookieVerify {
	std::map<std::string, std::string> cookiestorage;
	static CookieVerify* cookieverify;
public:
	static CookieVerify* getinstance() {
		return cookieverify;
	}

     inline void _insert(std::string _name, std::string _cookie) {
		cookiestorage.insert({_name, _cookie});
	}

	 inline std::string _find(std::string _name) {
		auto _p = cookiestorage.find(_name);
		if (_p == cookiestorage.end()) {
			return "queryfails";
		}
		else return _p->second;

	}


};


#endif // !

