#include"UserClassifier.H"


pqxx::result* UserClassifier::getResult(string query){
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

datum UserClassifier::make_datum(int access_day, int cart, int buy) {
   datum d;
   d.add_number("access_day", access_day);
   d.add_number("cart", cart);
   d.add_number("buy", buy);
   return d;
}

/* initiation of jubaclassifier */
UserClassifier::UserClassifier(){
   this->jubatus_connection = true;
   //RED cout<<"UserClassifier::UserClassifier() start!"<<endl;	RESET
   if(this->jubatus_connection){
      //cout<<"selected Jubatus Mode!"<<endl;

      jubatus_classifier = new jubatus::classifier::client::classifier("localhost", 9199, "test", 1.0);
      vector<labeled_datum> train_data;
      train_data.push_back(labeled_datum("Good", make_datum( 30,0,0)));
      train_data.push_back(labeled_datum( "Bad", make_datum( 0,0,0)));
      jubatus_classifier->train(train_data);
      delete jubatus_classifier;

   }else{
      cout<<"selected original evaluate function!"<<endl;
   }
}

void UserClassifier::Proc(){
   //RED cout<<"UserClassifier::Proc() start!"<<endl; RESET
   //evaluation
   if( this->jubatus_connection ){
      result *result_list;
      result_list = getResult("select src_ip,host,access_day from action_count where classify_flag=1");
      //cout << "jubatus scoring!!!"<<endl;

      jubatus_classifier = new jubatus::classifier::client::classifier("localhost", 9199, "test", 1.0);
      vector<datum> test_data;
      for( result::const_iterator c = result_list->begin(); c != result_list->end(); c++ ){
         test_data.push_back(make_datum( c[2].as(int()), 0, 0));
      }

      vector<vector<estimate_result> > results = jubatus_classifier->classify(test_data);
      size_t id=0;
      ostringstream sql;
      for( result::const_iterator c = result_list->begin(); c != result_list->end(); c++ ){
         if( results.size() <= id)break;
         //sql << "update action_count set score=" << results[id][0].score << " where src_ip='"+  c[0].as(string()) + "' and host='"+ c[1].as(string()) +"'";
         sql << "update action_count set score=" << results[id][0].score << ",classify_flag=0,update_flag=1 where src_ip='"+  c[0].as(string()) + "' and host='"+ c[1].as(string()) +"'";
         sql.str();
         //cout << sql.str() << endl;
         getResult(sql.str());
         sql.str("");
         id++;
      }
      delete jubatus_classifier;
   }else{
      result *result_list;
      result_list = getResult("select src_ip,host,access_day from action_count where train_flag=0");
      cout << "original scoring!!!"<<endl;
      for( result::const_iterator c = result_list->begin(); c != result_list->end(); c++ ){
         getResult("update action_count set score=" + c[2].as(string()) + " where src_ip='"+  c[0].as(string()) + "' and host='"+ c[1].as(string()) +"'");
         cout<<"update action_count set score=" + c[2].as(string()) + " where src_ip='"+  c[0].as(string()) + "' and host='"+ c[1].as(string()) +"'"<<endl;
      }
   }
   //RED cout<<"UserClassifier::Proc() end!"<<endl; RESET
}
