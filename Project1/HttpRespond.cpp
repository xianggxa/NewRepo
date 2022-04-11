#include "HttpRespond.h"
#include <iostream>


HttpRespond::~HttpRespond(){}

void HttpRespond::respond(int connfd)
{
	if (httpparsing->getUrl() == "/"){
		int _filelen = 0;

		char* _filecontent = Tools::readfile("/root/login/index.html", _filelen);

		std::string head("HTTP/1.1 200 OK\r\n");
		head += "Contene-Type: text/html\r\n";
		head += "Content-Length: " + std::to_string(_filelen) + "\r\n\r\n";
		send(connfd, head.c_str(), head.length(), 0);
		Tools::_send(connfd, _filecontent, _filelen);
		
		
	}
	else if (httpparsing->getMethod() == "GET") {
		int _filelen = 0;
		std::string path = Tools::deescapeURL(httpparsing->getUrl());//utf8解码
		std::string _head_ = "/root";
		std::string __head = "/disk";
		if (path.compare(0, _head_.size(), _head_) != 0&& path.compare(0, __head.size(), __head) != 0) {
            path = "/root"+path;
		}
		//std::cout << "path=" << path << "\n";
		
		std::string _url = Tools::deescapeURL(httpparsing->getUrl());
		std::string _file;
		std::map<std::string, std::string> pro_val;
		std::string _property;
		std::string _val;
		int _len = _url.length();
		
		for (int i = 1, pos = 1, tag_f = 0, p_pos = 0; i < _len; ) {//解析url中的属性和值
			if (_url[i] == '@') {
				if (!tag_f) {
					_file = _url.substr(pos, i - pos);
					tag_f = 1;
				}

				for (int _i = i + 1; _i < _len; _i++) {
					if (_url[_i] == '=') {
						_property = _url.substr(i + 1, _i - i - 1);
						for (int i_ = _i + 1; i_ < _len; i_++) {
							p_pos = i_;
							if (_url[i_] == '@') {
                                _val = _url.substr(_i + 1,i_ - _i - 1);
								pro_val[_property] = _val;

						        break;
							}
							if (i_ == (_len - 1)) {
								
								_val = _url.substr(_i + 1, i_ - _i );
								pro_val[_property] = _val;
								break;
							}
						}break;
						
					}

				}
				
				
			}
			if (p_pos > i)i = p_pos;
			else i++;
		}//std::cout << _file << " " << _property << " " << _val;


		if (_file == "mainpage") {


			//以下查询预览界面
			std::string sql_query = "select * from artist_preview ;";//第一次查询总的数量
			ServerMysql* servermysql = ServerMysql::getinstance();
			if (servermysql->_mysql_query(sql_query)) {
				return;
			}
			MYSQL_RES* mysql_result = servermysql->_mysql_store_result();
			

			int row_num = mysql_num_rows(mysql_result);
			mysql_free_result(mysql_result);
			int n_page = 1;
			
			int page_num = row_num/ARTIST_PAGE_MAX + (row_num%ARTIST_PAGE_MAX?1:0);
			if (pro_val["page"] != "") {
				n_page = std::stoi(pro_val["page"]);
			}
			if (n_page > page_num || n_page < 1 )return;

			std::string in_pi_string; //插入图片的字符串
			std::string in_pa_string; //插入分页的字符串
			std::string in_ti_string = "artist"; //插入标题的字符串

	        //第二次查询应当显示在当前界面的对象
			sql_query = "select * from artist_preview order by order_number limit " + std::to_string((n_page-1)*ARTIST_PAGE_MAX)+","+std::to_string(ARTIST_PAGE_MAX)+";";
			if (servermysql->_mysql_query(sql_query)) {
				return;
			}
			mysql_result = servermysql->_mysql_store_result();

			MYSQL_ROW row = servermysql->_mysql_fetch_row(mysql_result);
			//row_num = mysql_num_rows(mysql_result);

			int n_row = 1;
			//std::cout << "row_num=" << row_num << "\n";
			while (row != NULL )//图片显示动态html生成
			{
				std::cout << "n_row=" << n_row << "\n";
				std::string n_artist_name = row[0];
				std::string n_preview_path = row[1];
				n_preview_path = Tools::escapeURL(n_preview_path);
				std::cout << n_artist_name << " " << n_preview_path << "\n";
				std::string n_string = "<div class=\"fh5co-project masonry-brick\">"
					"<a href = \"artistpage@artist=" + n_artist_name + "@page=1"+"\">"
					"<img src = \"" + n_preview_path + "\" alt = \"Free HTML5 by FreeHTML5.co\" width = \"300px\">"
					"<h2>" + n_artist_name + "</h2>"
					"</a>"
					"</div>";

				in_pi_string += n_string;

				row = mysql_fetch_row(mysql_result);
				n_row++;
			}
			
			mysql_free_result(mysql_result);
			
			in_pa_string += "<li><a href = \"#\">«</a></li>";
			for (int _i = 1; _i <= page_num; _i++) {//分页html生成
				if (_i == n_page) {
					in_pa_string += "<li><a class = \"active\" href = \"#\">" + std::to_string(_i) + "</a></li>";
				}
				else {
					in_pa_string += "<li><a href = \"mainpage@page=" + std::to_string(_i) + "\">" + std::to_string(_i) + "</a></li>";
				}
			}
			in_pa_string += "<li><a href = \"#\">»</a></li>";
			
			//以下插入生成动态页面
			int _filelen = 0;
			char* _filecontent = Tools::readfile("/root/mainpage/index.html", _filelen);
			std::string* htmlcontent = new std::string(_filecontent, _filelen);
            delete []_filecontent;
			
			std::string find_ = "clearfix masonry\">";

			int _pos = Tools::KMP(htmlcontent, find_);
			//printf("%d\n",_pos);
			if (_pos == -1) {
				printf("kmp faile 1\n");
				return;
			}
			htmlcontent->insert(_pos, in_pi_string);

			find_ = "pagination\">";
			//std::cout << "1\n";
			_pos = Tools::KMP(htmlcontent, find_);
			//printf("%d\n", _pos);
			if (_pos == -1) {
				printf("kmp faile 2\n");
				return;
			}
			htmlcontent->insert(_pos, in_pa_string);

			find_ = "id=\"ti_insert\">";
			_pos = Tools::KMP(htmlcontent, find_);
			if (_pos == -1) {
				printf("kmp faile 3\n");
				return;
			}
			htmlcontent->insert(_pos, in_ti_string);

			
			
			char* n_content = const_cast<char*>(htmlcontent->c_str());
			std::string head("HTTP/1.1 200 OK\r\n");
			head += "Contene-Type: text/html; charset=utf-8\r\n";
			head += "Content-Length: " + std::to_string(htmlcontent->length()) + "\r\n\r\n";
			std::cout<<
			send(connfd, head.c_str(), head.length(), 0);
			//send(connfd, n_content, htmlcontent->length(), 0);
			Tools::_send(connfd, n_content, htmlcontent->length());

			//delete htmlcontent;
            

		}
		else if (_file == "artistpage") {
			/*
			std::vector<std::string> _works;
			std::string path = "/root/web_picture/comic/" + _property + '/';
			DIR* dir = opendir(path.c_str());
			struct dirent* entry;
			while ((entry = readdir(dir)) != NULL)
			{
				if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
					continue;
				_works.push_back(entry->d_name);
			}
			closedir(dir);*/
			ServerMysql* servermysql = ServerMysql::getinstance();
			std::string in_pi_string; //插入图片的字符串
			std::string in_pa_string; //插入分页的字符串
			std::string in_ti_string = "artist:" + pro_val["artist"]; //插入标题的字符串
			std::string sql_query = "select work_name,preview_path from works_preview where artist_name=\""  + pro_val["artist"] + "\";";
			if (servermysql->_mysql_query(sql_query)) {
				return;
			}
			MYSQL_RES* mysql_result = servermysql->_mysql_store_result();
			int row_num = mysql_num_rows(mysql_result);
			mysql_free_result(mysql_result);
			//std::string w_ = "row_num=" + std::to_string(row_num) + "\n";
			//Tools::writelog(w_);
			int n_page = 1;
			int page_num = row_num/WORK_PAGE_MAX + (row_num%WORK_PAGE_MAX?1:0);
			if (pro_val["page"] != "") {
				n_page = std::stoi(pro_val["page"]);
			}
			if (n_page > page_num || n_page < 1)return;
			sql_query = "select work_name,preview_path from works_preview where artist_name=\"" + pro_val["artist"] + "\" limit "+std::to_string((n_page - 1) * WORK_PAGE_MAX) + "," + std::to_string(WORK_PAGE_MAX) + ";";
			if (servermysql->_mysql_query(sql_query)) {
				return;
			}
			mysql_result = servermysql->_mysql_store_result();
			MYSQL_ROW row = servermysql->_mysql_fetch_row(mysql_result);
			

			//MYSQL_ROW row = servermysql->_mysql_fetch_row(mysql_result);
			while (row != NULL)
			{

				std::string work_name = row[0];
				std::string n_preview_path = row[1];
				n_preview_path = Tools::escapeURL(n_preview_path);//编码为utf8防止浏览器判断符号错误
				std::cout << work_name << " " << n_preview_path << "\n";
				std::string n_string = "<div class=\"fh5co-project masonry-brick\">"
					"<a href = \"workpage@artist=" + pro_val["artist"] + "@work=" + work_name + "@page=1\">"
					"<img src = \"" + n_preview_path + "\" alt = \"Free HTML5 by FreeHTML5.co\" width = \"300px\">"
					"<h2>" + work_name + "</h2>"
					"</a>"
					"</div>";

				in_pi_string += n_string;
				row = mysql_fetch_row(mysql_result);


			}
			mysql_free_result(mysql_result);
			in_pa_string += "<li><a href = \"#\">«</a></li>";
			//std::string _w = "pageal="+std::to_string(page_num) + '\n';
			//Tools::writelog(_w);
			
			for (int _i = 1; _i <= page_num; _i++) {//分页html生成
				if (_i == n_page) {
					in_pa_string += "<li><a class = \"active\" href = \"#\">" + std::to_string(_i) + "</a></li>";
				}
				else {
					in_pa_string += "<li><a href = \"artistpage@artist="+ pro_val["artist"]+"@page=" + std::to_string(_i) + "\">" + std::to_string(_i) + "</a></li>";
				}
			}
			in_pa_string += "<li><a href = \"#\">»</a></li>";

			int _filelen = 0;
			char* _filecontent = Tools::readfile("/root/mainpage/index.html", _filelen);
			std::string* htmlcontent = new std::string(_filecontent);
			delete []_filecontent;
			std::string find_ = "clearfix masonry\">";
			int _pos = Tools::KMP(htmlcontent, find_);
			htmlcontent->insert(_pos, in_pi_string);

			find_ = "pagination\">";
			std::cout << "1\n";
			_pos = Tools::KMP(htmlcontent, find_);
			
			if (_pos == -1) {
				printf("kmp faile 2\n");
				return;
			}
			htmlcontent->insert(_pos, in_pa_string);

			find_ = "id=\"ti_insert\">";
			_pos = Tools::KMP(htmlcontent, find_);
			if (_pos == -1) {
				printf("kmp faile 3\n");
				return;
			}
			htmlcontent->insert(_pos, in_ti_string);


			char* n_content = const_cast<char*>(htmlcontent->c_str());
			std::string head("HTTP/1.1 200 OK\r\n");
			head += "Contene-Type: text/html; charset=utf-8\r\n";
			head += "Content-Length: " + std::to_string(htmlcontent->length()) + "\r\n\r\n";
			send(connfd, head.c_str(), head.length(), 0);
			Tools::_send(connfd, n_content, htmlcontent->length());

			//delete htmlcontent;


		}
        else if (_file == "workpage") {
		std::string in_pi_string; //插入图片的字符串
		std::string in_pa_string; //插入分页的字符串
		std::string in_ti_string = "work:"+ pro_val["work"]; //插入标题的字符串
		/**/
			std::vector<std::pair<std::string,int> >p_works;
		    std::string _artist = pro_val["artist"];
			std::string _work = pro_val["work"];
			std::string path = "/disk/web_picture/comic/" + _artist + '/'+_work+"/";
			//std::cout << "path=" << path << "\n";
			DIR* dir = opendir(path.c_str());
			struct dirent* entry;
			while ((entry = readdir(dir)) != NULL)
			{
				if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
					continue;
				p_works.push_back({ entry->d_name ,Tools::stringtoint(entry->d_name)});
			}
			closedir(dir);
		std::sort(p_works.begin(), p_works.end(), [](const std::pair<std::string, int>& aa, const std::pair<std::string, int>& bb) {return aa.second < bb.second; });
		
		
		int n_page = 1;
		int p_num = p_works.size();
		int page_num = p_num/P_PAGE_MAX + (p_num%P_PAGE_MAX?1:0);
		if (pro_val["page"] != "") {
			n_page = std::stoi(pro_val["page"]);
		}
		if (n_page > page_num)return;
		
		//MYSQL_ROW row = servermysql->_mysql_fetch_row(mysql_result);
		for(int _i = (n_page-1)*P_PAGE_MAX; _i < (n_page*P_PAGE_MAX)&&_i<p_num; _i++)
		{
			std::string n_preview_path = path + p_works[_i].first;
		    n_preview_path = Tools::escapeURL(n_preview_path);//编码为utf8防止浏览器判断符号错误
			//std::cout << work_name << " " << n_preview_path << "\n";
			std::string n_string = "<div class=\"fh5co-project masonry-brick\">"
				"<a href = \"viewpage@artist=" + _artist + "@work=" + _work + "@page="+ std::to_string(_i+1)+"\">"
				"<img src = \"" + n_preview_path + "\" alt = \"Free HTML5 by FreeHTML5.co\" width = \"300px\">"
				"<h2>" + p_works[_i].first + "</h2>"
				"</a>"
				"</div>";

			in_pi_string += n_string;

		}
		in_pa_string += "<li><a href = \"#\">«</a></li>";
		//std::string _w = "pageal=" + std::to_string(page_num) + '\n';
		//Tools::writelog(_w);

		for (int _i = 1; _i <= page_num; _i++) {//分页html生成
			if (_i == n_page) {
				in_pa_string += "<li><a class = \"active\" href = \"#\">" + std::to_string(_i) + "</a></li>";
			}
			else {
				in_pa_string += "<li><a href = \"workpage@artist=" + _artist + "@work=" + _work + "@page=" + std::to_string(_i) + "\">" + std::to_string(_i) + "</a></li>";
			}
		}
		in_pa_string += "<li><a href = \"#\">»</a></li>";

		int _filelen = 0;
		char* _filecontent = Tools::readfile("/root/mainpage/index.html", _filelen);
		std::string* htmlcontent = new std::string(_filecontent);
		delete[]_filecontent;
		std::string find_ = "clearfix masonry\">";
		int _pos = Tools::KMP(htmlcontent, find_);
		htmlcontent->insert(_pos, in_pi_string);

		find_ = "pagination\">";
		_pos = Tools::KMP(htmlcontent, find_);

		if (_pos == -1) {
			printf("kmp faile 2\n");
			return;
		}
		htmlcontent->insert(_pos, in_pa_string);

		find_ = "id=\"ti_insert\">";
		_pos = Tools::KMP(htmlcontent, find_);
		if (_pos == -1) {
			printf("kmp faile 3\n");
			return;
		}
		htmlcontent->insert(_pos, in_ti_string);


		char* n_content = const_cast<char*>(htmlcontent->c_str());
		std::string head("HTTP/1.1 200 OK\r\n");
		head += "Contene-Type: text/html; charset=utf-8\r\n";
		head += "Content-Length: " + std::to_string(htmlcontent->length()) + "\r\n\r\n";
		send(connfd, head.c_str(), head.length(), 0);
		Tools::_send(connfd, n_content, htmlcontent->length());

		}
		else if (_file == "viewpage") {

		std::string in_ti_string = "work:" + pro_val["work"]; //插入标题的字符串
		/**/
		std::vector<std::pair<std::string, int> >p_works;
		std::string _artist = pro_val["artist"];
		std::string _work = pro_val["work"];
		std::string path = "/disk/web_picture/comic/" + _artist + '/' + _work + "/";
		//std::cout << "path=" << path << "\n";
		DIR* dir = opendir(path.c_str());
		struct dirent* entry;
		while ((entry = readdir(dir)) != NULL)
		{
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
				continue;
			p_works.push_back({ entry->d_name ,Tools::stringtoint(entry->d_name) });
		}
		closedir(dir);
		std::sort(p_works.begin(), p_works.end(), [](const std::pair<std::string, int>& aa, const std::pair<std::string, int>& bb) {return aa.second < bb.second; });


		int n_page = 1;
		int p_num = p_works.size();
		
		if (pro_val["page"] != "") {
			n_page = std::stoi(pro_val["page"]);
		}
		if (n_page > p_num)return;
		int pworks = p_works.size();
		std::string in_vec = "[";//插入的页面数组
			for (int i = 0; i < pworks; i++){
				if (i != (pworks - 1)) {
					in_vec += "\"" + p_works[i].first + "\",";
				}else in_vec += "\"" + p_works[i].first + "\"]";

		}
		std::string in_dir = std::to_string(n_page) + "/" + std::to_string(pworks);//插入表示当前目录的情况
		std::string in_pic_path = "\"" + path + p_works[n_page - 1].first + "\"";//插入当前显示的图片路径

		int _filelen = 0;
		char* _filecontent = Tools::readfile("/root/mainpage/viewindex.html", _filelen);
		std::string* htmlcontent = new std::string(_filecontent);
		delete[]_filecontent;

		path = "\"" + path + "\";\nvar page = " + std::to_string(n_page)+";";

		std::string find_ = "id=\"ti_insert\">";
		int _pos = Tools::KMP(htmlcontent, find_);
		if (_pos == -1) {
			printf("kmp faile 1\n");
			return;
		}
		htmlcontent->insert(_pos, in_ti_string);
		 
		find_ = "id=\"pic\" src =";
		_pos = Tools::KMP(htmlcontent, find_);
		if (_pos == -1) {
			printf("kmp faile 2\n");
			return;
		}
		htmlcontent->insert(_pos, in_pic_path);

		find_ = "var src_vec =";
		_pos = Tools::KMP(htmlcontent, find_);
		if (_pos == -1) {
			printf("kmp faile 3\n");
			return;
		}
		htmlcontent->insert(_pos, in_vec);

		find_ = "var path =";
		_pos = Tools::KMP(htmlcontent, find_);
		if (_pos == -1) {
			printf("kmp faile 4\n");
			return;
		}
		htmlcontent->insert(_pos, path);

		find_ = "id=\"directory\">";
		_pos = Tools::KMP(htmlcontent, find_);
		if (_pos == -1) {
			printf("kmp faile 5\n");
			return;
		}
		htmlcontent->insert(_pos, in_dir);


		char* n_content = const_cast<char*>(htmlcontent->c_str());
		std::string head("HTTP/1.1 200 OK\r\n");
		head += "Contene-Type: text/html; charset=utf-8\r\n";
		head += "Content-Length: " + std::to_string(htmlcontent->length()) + "\r\n\r\n";
		send(connfd, head.c_str(), head.length(), 0);
		Tools::_send(connfd, n_content, htmlcontent->length());


		}
		else {//请求普通资源
		
        char* _filecontent = Tools::readfile(path.c_str(), _filelen);
		if (_filecontent == nullptr) {
			std::cout << "path no exist\n";
			return;
		}
		std::string head = "HTTP/1.1 200 OK\r\n"
			"Contene-Type: text/html\r\n"
			"Cache-Control: max-age=300\r\n"
			"Content-Length: " + std::to_string(_filelen) + "\r\n\r\n";
		//char* head_content = new char[_filelen + head.length() + 1];
		//send(connfd, head.c_str(), head.length(), 0);
		//strcpy(head_content, head.c_str());
		//strcat(head_content, _filecontent);
		//delete []_filecontent;

		Tools::senthreadpool->commit(Tools::file_send, connfd, _filecontent, _filelen, head);
		
		
		}
		

	}
	else if (httpparsing->getMethod() == "POST") {
		if (httpparsing->getUrl() == "/mainpage") {//登陆响应
			
			std::string _s = httpparsing->getBody();
			int s_len = _s.length();
			if (s_len < 15) {
				printf("login format is incorrect");
			}
			if (_s.substr(0, 5) != "user=") {
				printf("user_name format is incorrect");
				return ;
			}int pos = 5;
			for (; pos < s_len; pos++) {
				if (_s[pos] == '&')break;
			}
			std::string user_name = _s.substr(5, pos - 5);
			if (_s.substr(pos + 1, 9) != "password=") {
				printf("user_password format is incorrect");
				return;
			}
			std::string user_password = _s.substr(pos + 10, s_len - pos - 10);
			std::string sql_query = "select user_password from name_password\n"
				"where user_name=\"" + user_name + "\";";
			ServerMysql* servermysql = ServerMysql::getinstance();
			if (servermysql->_mysql_query(sql_query)) {
                return ;
			}
			//登录验证
			MYSQL_RES* mysql_result = servermysql->_mysql_store_result();
			MYSQL_ROW row = servermysql->_mysql_fetch_row(mysql_result);
			servermysql->_mysql_free_result(mysql_result);
			if (row == NULL) {
				return;
			}
			if (row[0] != user_password) {
				return;
			}printf("login success\n");

			std::string cookie_ = Tools::cookieCreate();//生成cookie并返回设置
			CookieVerify* cookieverify = CookieVerify::getinstance();
			cookieverify->_insert(user_name, cookie_);

			std::string head("HTTP/1.1 200 OK\r\n");
			head += "Set-Cookie: "+ user_name + "=" + cookie_ +"\r\n\r\n";
			send(connfd, head.c_str(), head.length(), 0);


		}

	}
}

HttpRespond::HttpRespond(HttpParsing* httpparsing) {
	this->httpparsing = httpparsing;

}

