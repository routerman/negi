#pragma once
//#include "UserClassifier.H"
//#include "Result.H"
#include "Include.H"
//#include "Packet.H"
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
//using jubatus::classifier::client::classifier;

class UserClassifier{
	int count;
	jubatus::classifier::client::classifier *jubatus_classifier;
public:
	datum make_datum(int access_month, int cart, int buy);
	void Proc();
	void counter(){count++;}
	UserClassifier();
};

