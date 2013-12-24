#pragma once
//#include "UserClassifier.H"
//#include "Result.H"
#include "Include.H"
//#include "Packet.H"
#include "Global.H"

//#include "/opt/jubatus/include/jubatus/client.hpp"
#include <jubatus/client.hpp>

//#include <jubatus/client/recommender_client.hpp>
//#include <jubatus/client/recommender_types.hpp>
//#include <pficommon/lang/util.h>

using namespace std;
using namespace jubatus::classifier;

using std::make_pair;
using std::pair;
using std::string;
using std::vector;
using jubatus::classifier::datum;
using jubatus::classifier::estimate_result;
//using jubatus::classifier::client::classifier;
//using namespace jubatus;
//using namespace jubatus::recommender;
//using namespace jubatus;
//using namespace jubatus::recommender;
//using namespace pfi::lang;


class UserClassifier{
	bool jubatus_connection;
	int count;
	jubatus::classifier::client::classifier *jubatus_classifier;
	//jubatus::recommender::client::recommender *jubatus_recommender;
public:
	pqxx::result* getResult(string query);
	jubatus::classifier::datum make_datum(int access_month, int cart, int buy);
	//datum make_datum(int access_month, int cart, int buy);
	void Proc();
	void counter(){count++;}
	UserClassifier();
};

