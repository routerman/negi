#pragma once
#include "Uba.H"
#include "UserClassifier.H"
#include "Include.H"
#include "Global.H"
//#include "Packet.H"
//#include "Result.H"

using namespace std;
//using namespace pqxx;

class Entry{
public:
	string timestamp;
	string src_ip;
	string host;
	string type;
	string action;
	//
	//string pattern;
	//string result;
	//
	//for log
	string url;
	string title;
	string object;
	//
	Entry( string timestamp, string src_ip, string host){
		this->timestamp = timestamp;
		this->src_ip = src_ip;
		this->host = host;
		this->action = "access";
	}
};


class UrlAction{
private:
	string method;
	string host;
	string url;
	string action;
	string data;
	string referer;
public:
	string getHost(){return this->host;}
	string getUrl(){return this->url;}
	string getAction(){return this->action;}
	UrlAction(string host, string url, string action){
		this->host=host;
		this->url=url;
		this->action=action;
	}
};

//User Behavior Accumulation
class Uba{
	//Entry *entry;

	//vector<Record> record_list;
	map<string,string> record_map;
	map<string,string>::iterator mit;

	string before_timestamp;
	vector<string> except_extension;
	vector<UrlAction> url_action_list;
public:
	Uba();
	void Proc();
	//vyatta
	pqxx::result* getResult(string query);
	bool extension_filter( string result );

	void AnalyzeAction( pqxx::result::const_iterator c, Entry *entry);

	void CountTable( Entry *entry );
	void LogTable( Entry *entry );
	void VyattaProc();
};

