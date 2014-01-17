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

/* initiation of jubaclassifier */
UserClassifier::UserClassifier(){
	this->jubatus_connection = true;
	RED cout<<"UserClassifier::UserClassifier() start!"<<endl;	RESET

	if(this->jubatus_connection){
		cout<<"selected Jubatus Mode!"<<endl;
      //classifier
		jubatus_classifier = new jubatus::classifier::client::classifier("localhost",9199,"test",1.0);

		//result *result_list = getResult("select class,access_month,cart,buy from action_count where train_flag=1");
		
		vector< classifier::labeled_datum > train_data;
/*
		for( result::const_iterator c = result_list->begin(); c != result_list->end(); c++ ){
			train_data.push_back( make_pair( c[0].as(string()), make_datum( c[1].as(int()), c[2].as(int()), c[3].as(int()) ) ) );
		}
*/
		train_data.push_back( classifier::labeled_datum( "Good", make_datum(30,0,0) ) );
		train_data.push_back( classifier::labeled_datum(  "Bad", make_datum(0,0,0) ) );
		jubatus_classifier->train(train_data);

      //recommendeclassifier:classifier:classifier:classifier:classifier::::::r
	   //jubatus_recommender = new jubatus::recommender::client::recommender("localhost",9199,5);

	}else{
		cout<<"selected original evaluate function!"<<endl;
	}
}

datum UserClassifier::make_datum(int access_day, int cart, int buy) {
   datum d;
	d.add_number("access_day", access_day);
	d.add_number("cart", cart);
	d.add_number("buy", buy);
	return d;
}

void UserClassifier::Proc(){
	RED cout<<"UserClassifier::Proc() start!"<<endl; RESET
	result *result_list;
   result_list = getResult("select src_ip,host,access_day from action_count where train_flag=0");
	
	//evaluation
	vector<double> score_list;
	if( this->jubatus_connection ){
		//Mode:Jubatus 
		vector<datum> test_data;
		for( result::const_iterator c = result_list->begin(); c != result_list->end(); c++ ){
			test_data.push_back( make_datum( c[2].as(int()), 0, 0) );
		}

		string sscore;
		int i=0;
      cout <<"send to jubaclassifier!"<<endl;
      vector<vector<estimate_result> > results = jubatus_classifier->classify(test_data);
      cout <<"HOGEHOGEHOGE!"<<endl;
		for( result::const_iterator c = result_list->begin(); c != result_list->end(); c++ ){
			const estimate_result& good = results[i][0];
			//const estimate_result& bad = results[i][1];
         //sscore = good.score +"-"+bad.score;
         sscore = good.score;
         getResult("update action_count set score=" + sscore + " where src_ip='"+  c[0].as(string()) + "' and host='"+ c[1].as(string()) +"'");
			cout<<"update action_count set score=" + sscore + " where src_ip='"+  c[0].as(string()) + "' and host='"+ c[1].as(string()) +"'"<<endl;
			std::cout << std::endl;
         i++;
		}
	}else{
		//Mode:original scoring
		//double score;
		cout << "original scoring!!!"<<endl;
		for( result::const_iterator c = result_list->begin(); c != result_list->end(); c++ ){
			//double score =  c[2].as(int()) + c[3].as(int()) + c[4].as(int());
			//score_list.push_back(score);
			getResult("update action_count set score=" + c[4].as(string()) + " where src_ip='"+  c[0].as(string()) + "' and host='"+ c[1].as(string()) +"'");
			cout<<"update action_count set score=" + c[4].as(string()) + " where src_ip='"+  c[0].as(string()) + "' and host='"+ c[1].as(string()) +"'"<<endl;
		}
	}

	//view	
/*
	for( vector<double>::iterator it = score_list.begin(); it != score_list.end(); it++ ){
		cout << *it <<endl;
		getResult("update action_count set score=" + *it + " where src_ip='"+  c[0].as(string()) + "' and host='"+ c[1].as(string()) +"'");
	}
*/
}
