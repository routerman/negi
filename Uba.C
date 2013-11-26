#include"Uba.H"

Uba::Uba(){
	RED cout<<"Uba::Uba() start!"<<endl; RESET
	try{
		connection *conn = pgsql->GetConn();
		work T(*conn);
		result res( T.exec("select CURRENT_TIMESTAMP(0) AT TIME ZONE 'JST'") );
		result::const_iterator c = res.begin();
		before_timestamp = c[0].as( string() );
		T.commit();
		cout << before_timestamp << endl;
	}catch(const exception &e){
		cerr << e.what() << endl;
	}catch(...){
		cerr << "routerman >> unhandled error!! :)" << endl;
	}
	count=vyatta_count=0;
	InitRecordList();
	InitUrlActionList();
	RED cout<<"Uba::Uba() stop!"<<endl; RESET
}

//DNS
void Uba::InitRecordList(){
	RED cout<<"Uba::InitRecordList start!"<<endl;	RESET
	try{
		connection *conn = pgsql->GetConn();
		work T(*conn);
		result *result_list;
		result_list = new result( T.exec("select dst_ip,host from dns") );
		T.commit();
		for( result::const_iterator c = result_list->begin(); c != result_list->end(); c++ ){
			record_map.insert( make_pair( c[0].as(string()), c[1].as(string()) ) );
			RED cout<< "dst_ip=" <<c[0].as(string())<<" : host= "<< c[1].as(string()) <<endl; RESET
		}
		//record_map.insert(make_pair("203.216.231.189","www.yahoo.co.jp"));
	}catch(const exception &e){
		cerr << e.what() << endl;
	}catch(...){
		cerr << "routerman >> unhandled error!! :)" << endl;
	}
	RED cout<<"Uba::InitRecordList stop!"<<endl;	RESET
}

//
void Uba::InitUrlActionList(){
	RED cout<<"Uba::InitUrlActionList() start!"<<endl;	RESET
	try{
		connection *conn = pgsql->GetConn();
		work T(*conn);
		result *result_list;
		result_list = new result( T.exec("select url,host,action from url_action") );
		T.commit();

		UrlAction *tmp;
		for( result::const_iterator c = result_list->begin(); c != result_list->end(); c++ ){
			tmp = new UrlAction( c[0].as(string()), c[1].as(string()), c[2].as(string()) );
			//tmp = new UrlAction("/shop/cart/cart.aspx","www.nitori-net.jp","cart");
			url_action_list.push_back(*tmp);
		}
	}catch(const exception &e){
		cerr << e.what() << endl;
	}catch(...){
		cerr << "routerman >> unhandled error!!" << endl;
	}
	RED cout<<"Uba::InitUrlActionList() stop!"<<endl;	RESET
}


void Uba::Proc(){
	if(count>10){
		RED cout<<"Uba::Proc() start"<<endl; RESET
		count=0;
		try{
			connection *conn = pgsql->GetConn();			
			work T(*conn);
			result *result_list;
			cout << before_timestamp << endl;
			result_list = new result( T.exec("select src_ip,dst_ip,pattern,result from save_result where pattern like 'POST' and timestamp>'"+ before_timestamp +"'" ) );
			//result_list = new result( T.exec("select src_ip,dst_ip,pattern,result from save_result where pattern like 'POST'" ) );
			for( result::const_iterator it = result_list->begin(); it != result_list->end(); ++it ){
				//cout << "src_ip =" << it[0].as( string() ) << ": pattern=" << it[1].as( string() ) << ": result =" << it[2].as( string() ) << endl;
				//if( ( mit = record_map.find( it[1].as( string() ) ) == record_map.end() )continue;
				mit = record_map.find( it[1].as( string() ) );
				if( mit == record_map.end() )continue;	//HOSTが登録されてなければ無視
				string host = (*mit).second;
				for( vector<UrlAction>::iterator jt = url_action_list.begin(); jt != url_action_list.end(); jt++){
					//とりあえず、resultにurlが含むかどうか。
					if( jt->host==host ){
						//ACCESS COUNT
						if( it[3].as( string() ).find( jt->url,0 ) != string::npos ){
							RED
							//src_ipのカラムのactionをインクリメント。
							//if( T.exec( "update user_shop_actions set " + jt->action + "_count=" + jt->action + "_count +1 where src_ip='"+it[0].as( string() ) + "'") ){
							//	T.exec( "insert into user_shop_actions(src_ip,access_count,cart_count,buy_count) value('src_ip',0,0,0)" );
							//}
							result list(T.exec("select src_ip from user_shop_actions where src_ip like '"+ it[0].as( string() ) +"' and host= '"+ host +"'") );
							if(list.size()==0){
								cout << "INSERT!!" + it[0].as( string() ) << "in host="<< host <<endl;
								T.exec( "insert into user_shop_actions(src_ip,host,access_day,access_month,cart,buy) values('"+ it[0].as( string() ) +"','"+ host +"',0,0,0,0)" );
							}
							cout << "UPDATE!!" + it[0].as( string() )<<" in host=" << host <<endl;
							T.exec( "update user_shop_actions set " + jt->action + "=" + jt->action + "+1 where src_ip='"+it[0].as( string() ) + "' and host='"+ host +"'");
							RESET
							//T.commit();
							break;
						}
					}
				}
			}
			T.commit();
			work Tmp(*conn);			
			result res( Tmp.exec("select CURRENT_TIMESTAMP(0) AT TIME ZONE 'JST'") );
			result::const_iterator c = res.begin();
			before_timestamp = c[0].as( string() );
			cout << before_timestamp << endl;
			Tmp.commit();
			//conn->disconnect();
		}catch(const exception &e){
			cerr << e.what() << endl;
		}catch(...){
			cerr << "routerman >> unhandled error!! :)" << endl;
		}	
		user_classifier->Proc();
		VyattaProc();
		RED cout<<"Uba::Proc() end" <<endl; RESET
	}
}

//user_shop_actionsテーブルに問い合わせ、全てのGoodユーザのソースIPに対して静的ルーティングをする。
void Uba::VyattaProc(){
	vyatta_count++;
	if(vyatta_count>=1){
		RED cout<<"Uba::VyattaConfig start()!  vyataaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"<<endl;	RESET
		vyatta_count=0;
		try{
			connection *conn = pgsql->GetConn();
			work T(*conn);
			result *result_list;
			result_list = new result( T.exec("select src_ip from user_shop_actions where class='Good'") );
			for( result::const_iterator it = result_list->begin(); it != result_list->end(); ++it ){
				//ここでvyattaAPIをたたき、各GoodユーザのソースIPの次ホップを高品質サーバ行きに
				//cli_config_api("set protocols static route "+ it[0].as( string() )  +" next-hop 0.0.0.0");
			}
			RED
			char *const args[] = {
				"/bin/echo",
				"abc"
			};
			execvp(args[0],args);
			RESET
			T.commit();
		}catch(const exception &e){
			cerr << e.what() << endl;
		}catch(...){
			cerr << "routerman >> unhandled error!! :)" << endl;
		}
		RED cout<<"Uba::VyattaConfig() end" <<endl; RESET
	}
}

