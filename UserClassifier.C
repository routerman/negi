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
		jubatus_classifier = new jubatus::classifier::client::classifier("localhost",9199,1.0);
		try{
			connection *conn = pgsql->GetConn();
			work T(*conn);
			result *result_list;
			result_list = new result( T.exec("select class,access_month,cart,buy from user_shop_actions where train_flag=1") );
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
	}
	RED cout<<"UserClassifier::UserClassifier() end"<<endl;	RESET
}

datum UserClassifier::make_datum(int access_month, int cart, int buy) {
	datum d;
	d.num_values.push_back(make_pair("access_month", access_month));
	d.num_values.push_back(make_pair("cart", cart));
	d.num_values.push_back(make_pair("buy", buy));
	return d;
}


void UserClassifier::Proc(){
	//定期的にuserテーブルにuser情報を問い合わせ、jubatusに分類して、スコアを基にuser_shop_actionsのユーザタイプを更新する。
	RED cout<<"UserClassifier::Proc() start!"<<endl;	RESET
	try{
		connection *conn = pgsql->GetConn();
		work T(*conn);
		result *result_list;
		if( this->jubatus_connection ){
			//Mode:Jubatus 
			result_list = new result( T.exec("select src_ip,access_day,access_month,cart,buy from user_shop_actions where access_day>=10 and train_flag=0") );
			vector<datum> test_data;
			for( result::const_iterator c = result_list->begin(); c != result_list->end(); c++ ){
				test_data.push_back( make_datum( c[0].as(int()), c[1].as(int()), c[2].as(int()) ) );
			}
			T.commit();
			test_data.push_back(make_datum(200,2,0));
			test_data.push_back(make_datum(30,15,10));

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
			result_list = new result( T.exec("select src_ip,access_day,access_month,cart,buy from user_shop_actions where access_day>=10 and train_flag=0") );
			vector<double> score_list;
			double score;
			for( result::const_iterator c = result_list->begin(); c != result_list->end(); c++ ){
				score =  c[0].as(int()) + c[1].as(int()) + c[2].as(int());
				score_list.push_back(score);
			}
		}
		// 結果をもとにデータベースを更新する。
		//T.exec( "update user_shop_actions set class='Good' where src_ip='" + it[0].as( string() ) + "'");
	}catch(const exception &e){
		cerr << e.what() << endl;
	}catch(...){
		cerr << "routerman >> unhandled error!! :)" << endl;
	}
	RED cout<<"UserClassifier::Proc() end!" <<endl; RESET
}



