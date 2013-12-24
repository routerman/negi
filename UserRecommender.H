#pragma once
#include "Include.H"
#include "Global.H"
//#include <jubatus/client.hpp>
#include <jubatus/client/recommender_client.hpp>
#include <jubatus/client/recommender_types.hpp>
#include <pficommon/lang/util.h>

using namespace std;
using namespace jubatus;
using namespace jubatus::recommender;
using namespace pfi::lang;
//using jubatus::recommender::datum;

class UserRecommender{
   bool server_connection;
   jubatus::recommender::client::recommender *jubatus_recommender;
public:
   pqxx::result* getResult(string query);
   void Proc();
   UserRecommender();
};
