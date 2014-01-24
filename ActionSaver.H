#pragma once
#include "ActionSaver.H"
#include "UserClassifier.H"
#include "Include.H"
#include "Global.H"
#include "Stream.H"
#include "Packet.H"
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


class ActionRule{
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
	ActionRule(string host, string url, string action){
		this->host=host;
		this->url=url;
		this->action=action;
	}
};

//User Behavior Accumulation
class ActionSaver{
	//Entry *entry;

	//vector<Record> record_list;
	map<string,string> record_map;
	map<string,string>::iterator mit;

	string before_timestamp;
	vector<string> filter_str_list;
	vector<ActionRule> action_rule_list;
public:
	ActionSaver();
	void Proc();
	void Proc( Packet *pkt);
	//vyatta
	pqxx::result* getResult(string query);
	bool StringFilter( string result );

	void AnalyzeAction( pqxx::result::const_iterator c, Entry *entry);

	void SaveActionCountTable( Entry *entry );
	void SaveActionLogTable( Entry *entry );
};

