#pragma once
#include "Uba.H"
#include "UserClassifier.H"
#include "Include.H"
#include "Global.H"
//#include "Packet.H"
//#include "Result.H"

using namespace std;
//using namespace pqxx;

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

//User Behavior Accumulation
class Uba{
public:
	string before_timestamp;
	vector<UrlAction> url_action_list;
	map<string,string> record_map;
   map<string,string>::iterator mit;
	void Proc();
	//vyatta
	void VyattaProc();
	pqxx::result* getResult(string query);
	Uba();
};

