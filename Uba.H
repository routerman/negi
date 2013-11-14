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
	string url;
	string host;
	string action;
	UrlAction(string url, string host, string action){
		this->url=url;
		this->host=host;
		this->action=action;
	}
};


class Uba{
public:
	unsigned int counter,jubacounter;
	vector<UrlAction> urlaction_list;
	vector<User> user_list;
	void count(){counter++;}
	void Proc();
	void jubatus_train();
	void jubatus_test();
	Uba();
};



