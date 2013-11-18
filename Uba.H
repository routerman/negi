#pragma once
#include "Uba.H"
#include "Result.H"
#include "Include.H"
#include "Packet.H"
#include "Global.H"
//#include "/opt/jubatus/include/jubatus/client.hpp"
#include <jubatus/client.hpp>

using namespace std;
using std::make_pair;
using std::pair;
using std::string;
using std::vector;
using jubatus::classifier::datum;
using jubatus::classifier::estimate_result;


class User{
public:
	int id;
	string src_ip;
	int access_day;
	int access_month;
	int cart;
	int buy;
	bool Identify(string src_ip){
		return this->src_ip==src_ip;
	}
	int getSrc_ip(){
		return this->id;		
	}
	User(string src_ip){
		this->src_ip = src_ip;
		access_day = access_month = cart = buy = 0;
	}
};

class UrlAction{
public:
	string host;
	string method;
	string url;
	string referer;
	string action;
	string data;
	UrlAction(string url, string host, string action){
		this->url=url;
		this->host=host;
		this->action=action;
	}
};

class Uba{
public:
	jubatus::classifier::client::classifier *client;
	unsigned int counter,jubacounter;
	string before_timestamp;
	vector<UrlAction> url_action_list;
	vector<User> user_list;
	map<string,string> record_map;
   map<string,string>::iterator mit;
	void count(){counter++;}
	void Proc();
	void Jubatus_test();
	string getHost(string dst_ip);	
	
	void InitRecordList();
	void InitUrlActionList();
	void InitJubatus();
	Uba();
};

