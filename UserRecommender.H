#pragma once
#include "Include.H"
#include "Global.H"
//#include <jubatus/client.hpp>
#include <jubatus/client/recommender_client.hpp>
#include <jubatus/client/recommender_types.hpp>


using namespace std;
using namespace jubatus;
using namespace jubatus::recommender;

//using jubatus::clinet::common::datum;
//using jubatus::recommender::client::recommender;
//using jubatus::recommender::datum;

class UserRecommender{
   bool server_connection;
   jubatus::recommender::client::recommender *jubatus_recommender;
   vector<id_with_score> similar_user;

public:
   pqxx::result* getResult(string query);
   void Proc();
   void UpdateRecommender();
   void CompleteScore();
   void ShowSimilarUser(string src_ip,int size);
   void ShowAllSimilarUsers();
   UserRecommender();
};
