#include "UserRecommender.H"

pqxx::result* UserRecommender::getResult(string query){
   result *result_list;
   try{
      connection *conn = pgsql->GetConn();
      work T(*conn);
      result_list = new result( T.exec(query) );
      T.commit();
   }catch(const exception &e){
      cerr << e.what() << endl;
   }catch(...){
      cerr << "unhandled error!!" << endl;
   }
   return result_list;
}

void UserRecommender::Proc(){
   if(server_connection){
	   RED cout<<"UserRecommender::Proc() start!"<<endl; RESET
      result *result_list;
      result_list = getResult("select src_ip from action_count where train_flag=0");
      for( result::const_iterator c = result_list->begin(); c != result_list->end(); c++ ){
         int i = c[0].as(int());
         //similar_result sr = jubatus_recommender->similar_row_from_id("recommender_ml", pfi::lang::lexical_cast<string>(i), 10);
         vector<id_with_score> sr = jubatus_recommender->similar_row_from_id( c[0].as(string()), 10);
         cout <<  "src_ip " << i << " is similar to :";
         for (size_t i = 1; i < sr.size(); ++i){
            cout <<  sr[i].id << ", ";
         }
         cout << endl;
      }
   }
}

UserRecommender::UserRecommender(){
   server_connection=true;
   if(server_connection){
      jubatus_recommender = new jubatus::recommender::client::recommender("localhost",19199,"recommender",5);
      result *result_list;
      result_list = getResult("select src_ip,host,score from action_count where train_flag=1");
      for( result::const_iterator c = result_list->begin(); c != result_list->end(); c++ ){
         datum d;
         d.add_number( c[1].as(string()),  c[2].as(int()) );
         jubatus_recommender->update_row(c[0].as(string()), d);
      }          
   }
}

