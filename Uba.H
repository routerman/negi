#pragma once
#include "Uba.H"
#include "Result.H"
#include "Include.H"
#include "Packet.H"
#include "Global.H"

//#include "/opt/jubatus/include/jubatus/client.hpp"
using namespace std;

class User{
public:
	int id;
	string src_ip;
	int access;
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
		access = cart = buy = 0;
	}
};


class Uba{
public:
	unsigned int counter;
	vector<Result> result_list;
	void count(){counter++;}
	void Proc();
	Uba();
};

