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

UserRecommender::UserRecommender(){
   server_connection=true;
   if(server_connection){

   }
}

void UserRecommender::Proc(){
   if(server_connection){
      //RED cout<<"UserRecommender::Proc() start!"<<endl; RESET
      jubatus_recommender = new jubatus::recommender::client::recommender("localhost",19199,"recommender",5);
      UpdateRecommender();
      CompleteScore();
      delete jubatus_recommender;
   }
}

void UserRecommender::UpdateRecommender(){
   result *result_list;
   result_list = getResult("select src_ip,host,score from action_count where update_flag=1");
   //update
   for( result::const_iterator c = result_list->begin(); c != result_list->end(); c++ ){
      datum d;
      d.add_number( c[1].as(string()),  c[2].as(int()) );
      jubatus_recommender->update_row(c[0].as(string()), d);
      getResult("update action_count set update_flag=0 where src_ip='"+ c[0].as(string()) +"' and host='"+ c[1].as(string())+"'");
   }
}

void UserRecommender::CompleteScore(){
   ostringstream sql;
   datum complete;
   result *top_user_list,*su_score_list;
   //top_user_list = getResult("select src_ip,host from action_count where host like '173.194.38.*' order by score limit 1");
   top_user_list = getResult("select src_ip,host from action_count order by access_day desc limit 5");
   //analysis
   for( result::const_iterator tu = top_user_list->begin(); tu != top_user_list->end(); tu++ ){
      similar_user = jubatus_recommender->similar_row_from_id( tu[0].as(string()), 5);
      //ShowSimilarUser(tu[0].as(string()),5);
      //complete simlar user 
      //似ているユーザのhostのスコアを補正する，
      for (size_t i = 1; i < similar_user.size(); ++i){
         complete = jubatus_recommender->complete_row_from_id( similar_user[i].id );
         su_score_list = getResult("select score from action_count where src_ip='"+ similar_user[i].id +"' and host='"+ tu[1].as(string()) +"' ");
         if( su_score_list->size() == 0){
            for(vector< pair<string, double> >::iterator it = complete.num_values.begin(); it!=complete.num_values.end() ;it++ ){
               if( it->first==tu[1].as(string()) ){
                  //sql << "insert into action_count(src_ip,host,access_day,access_month,cart,buy,score,train_flag) values('"+similar_user[i].id+"','"+tu[1].as(string()) +"',0,0,0,0," << it->second << ",0)";
                  sql << "insert into action_count(src_ip,host,access_day,access_month,cart,buy,score,learn_flag,classify_flag,update_flag) values('"+similar_user[i].id+"','"+it->first +"',0,0,0,0," << it->second <<",0,0,0)";
                  break;
               }
            }
         }else{
            for(vector< pair<string, double> >::iterator it = complete.num_values.begin(); it!=complete.num_values.end() ;it++ ){
               if( it->first==tu[1].as(string()) ){
                  sql << "update action_count set score=score+" << it->second << " where src_ip='"+ similar_user[i].id + "' and host='"+ tu[1].as(string()) +"'";
                  break;
               }
            }
         }
         //cout<< sql.str()<<endl;
         sql.str("");
         getResult( sql.str() );
      }
   }
}


void UserRecommender::ShowSimilarUser(string src_ip, int size){
   similar_user = jubatus_recommender->similar_row_from_id( src_ip, size);
   cout<< setw(15)<<src_ip <<",   ";
   for (size_t i = 1; i < similar_user.size(); ++i){
      cout<< setw(15) <<similar_user[i].id<<", ";
   }
   cout<<endl;
}

void UserRecommender::ShowAllSimilarUsers(){
   jubatus_recommender = new jubatus::recommender::client::recommender("localhost",19199,"recommender",5);
   vector<string> src_ip_list = jubatus_recommender->get_all_rows();
   for( vector<string>::iterator it= src_ip_list.begin(); it != src_ip_list.end(); it++ ){
      ShowSimilarUser(*it,5);
   }
   delete jubatus_recommender;
}



