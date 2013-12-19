#include"UserClassifier.H"

/* initiation of jubaclassifier */
UserClassifier::UserClassifier(){
	this->jubatus_connection = false;
	RED cout<<"UserClassifier::UserClassifier() start!"<<endl;	RESET

/*
	char *args[] = {"jubaclassifier","-f","/opt/jubatus/share/jubatus/example/config/classifier/pa.json","&",NULL};
	if( execvp(args[0],args) == -1 ){
		jubatus_connection = false;
		cerr <<"can't execute juaclassifier!!"<<endl;
		return;
	}
*/

	if(this->jubatus_connection){
		cout<<"selected Jubatus Mode!"<<endl;
		jubatus_classifier = new jubatus::classifier::client::classifier("localhost",9199,1.0);
		try{
			connection *conn = pgsql->GetConn();
			work T(*conn);
			result *result_list;
			result_list = new result( T.exec("select class,access_month,cart,buy from actions_count where train_flag=1") );
			T.commit();
			vector<pair<string, datum> > train_data;
			for( result::const_iterator c = result_list->begin(); c != result_list->end(); c++ ){
				train_data.push_back( make_pair( c[0].as(string()), make_datum( c[1].as(int()), c[2].as(int()), c[3].as(int()) ) ) );
			}
			jubatus_classifier->train("test",train_data);
		}catch(const exception &e){
			cerr << e.what() << endl;
		}catch(...){
			cerr << "routerman >> unhandled error!! :)" << endl;
		}
	}else{
		cout<<"selected original evaluate function!"<<endl;
	}
}

datum UserClassifier::make_datum(int access_month, int cart, int buy) {
	datum d;
	d.num_values.push_back(make_pair("access_month", access_month));
	d.num_values.push_back(make_pair("cart", cart));
	d.num_values.push_back(make_pair("buy", buy));
	return d;
}

void UserClassifier::Proc(){
	RED cout<<"UserClassifier::Proc() start!"<<endl;	RESET
	result *result_list;
	try{
		connection *conn = pgsql->GetConn();
		work T(*conn);
		result_list = new result( T.exec("select src_ip,access_day,access_month,cart,buy from action_count where train_flag=1") );
		T.commit();
	}catch(const exception &e){
		cerr << e.what() << endl;
	}catch(...){
		cerr << "routerman >> unhandled error!! :)" << endl;
	}
	//evaluation
	vector<double> score_list;
	if( this->jubatus_connection ){
		//Mode:Jubatus 
		vector<datum> test_data;
		for( result::const_iterator c = result_list->begin(); c != result_list->end(); c++ ){
			test_data.push_back( make_datum( c[0].as(int()), c[1].as(int()), c[2].as(int()) ) );
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
		for( result::const_iterator c = result_list->begin(); c != result_list->end(); c++ ){
			score =  c[0].as(int()) + c[1].as(int()) + c[2].as(int());
			score_list.push_back(score);
			//cout << score << endl;
		}
	}
	//view
}



