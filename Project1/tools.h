#pragma once


#ifndef FILE_SEND_TOOLS
#define FILE_SEND_TOOLS

#include <cstdio>
#include <cstring>
#include <memory.h>
#include <dirent.h>
#include <vector>
#include <iostream>
#include "threadpool.h"
#include <sys/socket.h>

#define BUF_SIZE 2048

class Tools {
public:
	static std::threadpool* senthreadpool;
public:
	static char* readfile(const char* _filepath, int& _filelen) {//文件读取函数
		FILE* file = fopen(_filepath, "rb");

		if (!file) {
			//printf("%s path no exist \n", _filepath);
			std::string _s = _filepath;
			_s+="@path no exist\n";
			Tools::writelog(_s);
			return nullptr;
		}
		fseek(file, 0, SEEK_END);//文件指针到达文件尾部
		_filelen = ftell(file);//获取文集长度
		rewind(file);//文件指针指向文件头部

		char* _filebuf = new char[_filelen + 1];

		memset(_filebuf, 0, _filelen + 1);
		fread(_filebuf, sizeof(char), _filelen, file);

		fclose(file);
		return _filebuf;

	}
	static void _send(int connfd, char* _filecontent, int _filelen) {//大报文发送
		char buf[BUF_SIZE];
		for (int i = 0, _size = 0; i < _filelen; i += BUF_SIZE) {
			if (i + BUF_SIZE + 1 > _filelen) {
				_size = _filelen - i;
			}
			else {
				_size = BUF_SIZE;
			}
			memcpy(buf, _filecontent + i, _size);

			send(connfd, buf, _size, 0);

		}
		delete []_filecontent;

	}
	static void file_send(int connfd, char* _filecontent, int _filelen, std::string& head) {
		send(connfd, head.c_str(), head.length(), 0);
		_send(connfd, _filecontent, _filelen);

	}
	static int* getNext(std::string* p)//kmp获得next数组
	{
		int len = p->length();
		int* next = new int[len+1];
		next[0] = -1;           //while the first char not match, i++,j++
		int j = 0;
		int k = -1;
		while (j < len)
		{
			if (k == -1 || p->at(j) == p->at(k))
			{
				j++;
				k++;
				next[j] = k;
			}
			else
			{
				k = next[k];
			}
		}
		return next;
	}
	static int KMP(std::string *T, std::string& p)
	{
		int i = 0;
		int j = 0;

		int* next = getNext(T);
		while (i < (int)T->length() && j < (int)p.length())
		{
			if (j == -1 || T->at(i) == p[j])
			{
				i++;
				j++;
			}
			else
			{
				j = next[j];
			}
		}
		delete [] next;
		if (j == (int)p.length())
		{
			return i;//返回能直接插入的位置
		}
		
		return -1;
	}
	static std::string cookieCreate() {//cookie生成
		std::string cookie_;
		srand(time(0));
		for (int i = 0; i < 20; i++) {
			int _num = rand() % 62;
			if (_num < 10) {
				cookie_ += char(_num + '0');
			}
			else if (_num < 36) {
				cookie_ += char(_num - 10 + 'a');
			}
			else {
				cookie_ += char(_num - 36 + 'A');
			}

			

		}
		return cookie_;
	}
	static char dec2hexChar(short int n) {
		if (0 <= n && n <= 9) {
			return char(short('0') + n);
		}
		else if (10 <= n && n <= 15) {
			return char(short('A') + n - 10);
		}
		else {
			return char(0);
		}
	}

	static short int hexChar2dec(char c) {
		if ('0' <= c && c <= '9') {
			return short(c - '0');
		}
		else if ('a' <= c && c <= 'f') {
			return (short(c - 'a') + 10);
		}
		else if ('A' <= c && c <= 'F') {
			return (short(c - 'A') + 10);
		}
		else {
			return -1;
		}
	}

	static std::string escapeURL(const std::string& URL)
	{
		std::string result = "";
		for (unsigned int i = 0; i < URL.size(); i++) {
			char c = URL[i];
			if (
				('0' <= c && c <= '9') ||
				('a' <= c && c <= 'z') ||
				('A' <= c && c <= 'Z') ||
				c == '/' || c == '.'
				) {
				result += c;
			}
			else {
				int j = (short int)c;
				if (j < 0) {
					j += 256;
				}
				int i1, i0;
				i1 = j / 16;
				i0 = j - i1 * 16;
				result += '%';
				result += dec2hexChar(i1);
				result += dec2hexChar(i0);
			}
		}
		return result;
	}

	static std::string deescapeURL(const std::string& URL) {
		std::string result = "";
		for (unsigned int i = 0; i < URL.size(); i++) {
			char c = URL[i];
			if (c != '%') {
				result += c;
			}
			else {
				char c1 = URL[++i];
				char c0 = URL[++i];
				int num = 0;
				num += hexChar2dec(c1) * 16 + hexChar2dec(c0);
				result += char(num);
			}
		}
		return result;
	}
	static std::vector<std::string> getsonfilename(std::string& _path) {//获取路径下所有文件名
		DIR* dir = opendir(_path.c_str());
		struct dirent* entry;
		std::vector<std::string> namelist;


		while ((entry = readdir(dir)) != NULL)
		{
			if (entry->d_type == DT_DIR) {//It's dir
				if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
					continue;
				 namelist.emplace_back(entry->d_name);
			}
			else {
				namelist.emplace_back(entry->d_name);

			}
		}
		closedir(dir);
		return namelist;
	}
	static void writelog(std::string &s) {
		FILE* file = fopen("/root/log.txt", "ab");

		fwrite(s.c_str(), sizeof(char), s.length(), file);
		fclose(file);
	}
	static int stringtoint(std::string s) {//非规则转化
		int ans = 0;
		for (auto i : s) {
			if ('0' <= i && i <= '9') {
				ans = ans * 10 + int(i - '0');
			}
		}
		return ans;

	}




};



#endif // !1
