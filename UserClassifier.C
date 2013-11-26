#include"UserClassifier.H"

/* initiation of jubaclassifier */
UserClassifier::UserClassifier(){
	RED cout<<"UserClassifier::UserClassifier() start!"<<endl;	RESET
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
		//train_data.push_back(make_pair("Good",make_datum(50,10,5)));
		//train_data.push_back(make_pair("Bad", make_datum(80,4,0)));
		//train_data.push_back(make_pair("Good",make_datum(50,10,10)));
		//train_data.push_back(make_pair("New", make_datum(20,2,0)));
		jubatus_classifier->train("test",train_data);
	}catch(const exception &e){
		cerr << e.what() << endl;
	}catch(...){
		cerr << "routerman >> unhandled error!! :)" << endl;
	}
	RED cout<<"UserClassifier::UserClassifier() start!"<<endl;	RESET
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
	count++;
	if(count>=2){
		RED cout<<"UserClassifier::Proc() staaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaart!"<<endl;	RESET
		count=0;
		try{
			connection *conn = pgsql->GetConn();
			work T(*conn);
			result *result_list;
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
			// 結果をもとにデータベースを更新する。
			//T.exec( "update user_shop_actions set class='Good' where src_ip='" + it[0].as( string() ) + "'");
		}catch(const exception &e){
			cerr << e.what() << endl;
		}catch(...){
			cerr << "routerman >> unhandled error!! :)" << endl;
		}
		RED cout<<"UserClassifier::Proc() end!" <<endl; RESET
	}
}
