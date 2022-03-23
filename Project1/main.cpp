//#include<cstdio>
//#include<cstring>
//#include <iostream>
#include "server.h"
//#include <mysql.h>
//#include "tools.h"

 

int  main()
{/**/
	
	
	
	
	/*
	FILE* file = fopen("/root/artistimport.txt", "ab");
	
	fwrite(s.c_str(), sizeof(char), s.length(), file);
	fclose(file);

	

	DIR* dir = opendir(path.c_str());
	struct dirent* entry;
	while ((entry = readdir(dir)) != NULL)
	{
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
				continue;
				if(partist.find(entry->d_name)!=partist.end())continue;
			std::string _path = path + entry->d_name;
			handle_artist(_path,entry->d_name);
			

	}
	closedir(dir);
*/
/*	*/
	Server* server = new Server();
	server->run();
	
	
/*	
	
	int _filelen;
	char* c = FileSendTools::readfile("//root//login//index.html", _filelen);
	printf("%s", c);

	
	MYSQL mysql;
	MYSQL_ROW row;

	MYSQL_FIELD* field = NULL;

	MYSQL_RES* result;
	mysql_init(&mysql);
	if (!mysql_real_connect(&mysql, "172.23.5.220", "root", "13725249746zjh",
		"web_server", 3306, NULL, 0)) { //连接mysql数据库，并判断是否出错(返回0为失败)
		printf("mysql_real_connect:%s\n", mysql_error(&mysql));
		//return -2;
	}
	std::string s = "select user_name from name_password where user_name=\"admin\";";
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


	}*/
		

	

	return 0;
}

