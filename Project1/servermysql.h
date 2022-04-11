#pragma once
#ifndef MYSQL_
#define MYSQL_


#include <mysql.h>
#include <stdio.h>
#include <string>
#include <mutex>


class ServerMysql {
protected:
	ServerMysql(){}
private:
	MYSQL mysql;
	std::mutex mtx;
	static ServerMysql* _mysql;
public:
	static ServerMysql* getinstance() {
		return _mysql;
	}
	inline void _init(std::string database_name) {
		
		mysql_init(&mysql);
		if (!mysql_real_connect(&mysql, "172.23.5.220", "root", "", database_name.c_str(), 3306, NULL, 0)) { //连接mysql数据库，并判断是否出错(返回0为失败)
		printf("mysql_real_connect:%s\n", mysql_error(&mysql));
		//return -2;
	}
	}
	inline int _mysql_query(const std::string& query) {
		std::lock_guard<std::mutex> mylock_guard(mtx);
		if (mysql_real_query(&mysql, query.c_str(), query.length())) {
			printf("Database query failed\n");
			printf("%s\n", query);
			return 1;
		}return 0;

	}
	inline MYSQL_RES* _mysql_store_result() {
		std::lock_guard<std::mutex> mylock_guard(mtx);
		MYSQL_RES* _res = mysql_store_result(&mysql);
		if (_res == NULL) {
			printf("Database query store nothing\n");
		}
		return _res;
	}
	inline MYSQL_ROW _mysql_fetch_row(MYSQL_RES* result) {
		std::lock_guard<std::mutex> mylock_guard(mtx);
		return mysql_fetch_row(result);

	}
	inline void _mysql_free_result(MYSQL_RES* result) {
		std::lock_guard<std::mutex> mylock_guard(mtx);
		mysql_free_result(result);
	}
	inline void _mysql_close() {
		mysql_close(&mysql);

	}
	MYSQL* get() {
		return &mysql;
	}

	/*
	MYSQL mysql;
	MYSQL_ROW row;

	MYSQL_FIELD* field = NULL;

	MYSQL_RES* result;
	mysql_init(&mysql);
	
	std::string s = "select user,host from user ;";
	mysql_query(&mysql, s.c_str());
	result = mysql_store_result(&mysql);
	for (int i = 0; i < mysql_num_fields(result); i++) {
		field = mysql_fetch_field_direct(result, i);

		std::cout << field->name << "\t\t";
	}
	row = mysql_fetch_row(result);

	while (row != NULL)
	{
		for (int i = 0; i < mysql_num_fields(result); i++) {
			std::cout << row[i] << "\t\t";

		}
		std::cout << "\n";
		row = mysql_fetch_row(result);
	}
	*/
};


#endif // !1
