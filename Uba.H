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
	string src_ip;
	string pattern;
	string result;
	//
	string host;
	string table;
	//
	string action;
	Entry( string src_ip, string pattern, string result){
		this->src_ip = src_ip;
		this->pattern = pattern;
		this->result = result;
	}
};


class Record{
public:
	string dst_ip;
	string host;
	string table;
	Record(string dst_ip,string host,string table){
		this->dst_ip=dst_ip;
		this->host=host;
		this->table=table;
	}
};

class UrlAction{
private:
	string method;
	string host;
	string url;
	string referer;
	string action;
	string data;
public:
	string getHost(){return host;}
	string getUrl(){return url;}
	string getAction(){return action;}
	UrlAction(string host, string url, string action){
		this->host=host;
		this->url=url;
		this->action=action;
	}
};

//User Behavior Accumulation
class Uba{
	string before_timestamp;
	vector<string> except_extension;
	vector<UrlAction> url_action_list;
	vector<Record> record_list;
public:
	Uba();
	void Proc();
	//vyatta
	pqxx::result* getResult(string query);

	bool setHostTable(string dst_ip, Entry *entry);
	bool extension_filter( string result );

	void update_shoptable( Entry *entry );

	void VyattaProc();
};

