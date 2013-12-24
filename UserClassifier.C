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
	this->jubatus_connection = false;
	RED cout<<"UserClassifier::UserClassifier() start!"<<endl;	RESET

	if(this->jubatus_connection){
		cout<<"selected Jubatus Mode!"<<endl;
      //classifier
		jubatus_classifier = new jubatus::classifier::client::classifier("localhost",9199,1.0);

		result *result_list;
		result_list = getResult("select class,access_month,cart,buy from action_count where train_flag=1");
		
		vector<pair<string, classifier::datum> > train_data;
		for( result::const_iterator c = result_list->begin(); c != result_list->end(); c++ ){
			train_data.push_back( make_pair( c[0].as(string()), make_datum( c[1].as(int()), c[2].as(int()), c[3].as(int()) ) ) );
		}
		jubatus_classifier->train("test",train_data);



      //recommender
	//	jubatus_recommender = new jubatus::recommender::client::recommender("localhost",9199,5);





	}else{
		cout<<"selected original evaluate function!"<<endl;
	}
}

classifier::datum UserClassifier::make_datum(int access_month, int cart, int buy) {
	classifier::datum d;
	d.num_values.push_back(make_pair("access_month", access_month));
	d.num_values.push_back(make_pair("cart", cart));
	d.num_values.push_back(make_pair("buy", buy));
	return d;
}

void UserClassifier::Proc(){
	RED cout<<"UserClassifier::Proc() start!"<<endl;	RESET
	result *result_list;
	result_list = getResult("select src_ip,host,access_day,access_month,cart,buy from action_count where train_flag=0");
	
	//evaluation
	vector<double> score_list;
	if( this->jubatus_connection ){
		//Mode:Jubatus 
		vector<classifier::datum> test_data;
		for( result::const_iterator c = result_list->begin(); c != result_list->end(); c++ ){
			test_data.push_back( make_datum( c[2].as(int()), c[3].as(int()), c[4].as(int()) ) );
		}

		vector<vector<estimate_result> > results = jubatus_classifier->classify("test", test_data);
		for (size_t i = 0; i < results.size(); ++i) {
			for (size_t j = 0; j < results[i].size(); ++j) {
				const estimate_result& r = results[i][j];
				std::cout << r.label << " " << r.score << std::endl;
			}
			std::cout << std::endl;
		}
	}else{
		//Mode:original scoring
		double score;
		cout << "original scoring!!!"<<endl;
		for( result::const_iterator c = result_list->begin(); c != result_list->end(); c++ ){
			score =  c[2].as(int()) + c[3].as(int()) + c[4].as(int());
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
