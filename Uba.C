#include"Uba.H"

pqxx::result* Uba::getResult(string query){
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

Uba::Uba(){
	result *result_list;
	//Initiation timstamp
	result_list = getResult("select CURRENT_TIMESTAMP(0) AT TIME ZONE 'JST'" );
	result::const_iterator c = result_list->begin();
	before_timestamp = c[0].as( string() );
	cout << before_timestamp << endl;

	//Initiation RecordList
	result_list = getResult("select dst_ip,host from dns");
	for( result::const_iterator c = result_list->begin(); c != result_list->end(); c++ ){
		record_map.insert( make_pair( c[0].as(string()), c[1].as(string()) ) );
		//cout << c[0].as(string()) <<":"<< c[1].as(string()) << endl;
	}

	//Initiation UrlActionList
	result_list = getResult("select url,host,action from url_action" );
	UrlAction *tmp;
	for( result::const_iterator c = result_list->begin(); c != result_list->end(); c++ ){
		tmp = new UrlAction( c[0].as(string()), c[1].as(string()), c[2].as(string()) );
		//cout<<"url="<< c[0].as(string())<<":host="<< c[1].as(string()) <<":action"<<  c[2].as(string()) << endl;
		//tmp = new UrlAction("/shop/cart/cart.aspx","www.nitori-net.jp","cart");
		url_action_list.push_back(*tmp);
	}
}

void Uba::Proc(){
	RED cout<<"Uba::Proc() start"<<endl; RESET
	cout  << before_timestamp << endl;
	result *result_list = getResult("select src_ip,dst_ip,pattern,result from save_result where pattern='POST ' and timestamp>'"+ before_timestamp +"'");
	//result *result_list = getResult("select src_ip,dst_ip,pattern,result from save_result where timestamp>='"+ before_timestamp +"' and ( pattern='GET ' or pattern='POST ' )");
	//result *result_list = getResult("select src_ip,dst_ip,pattern,result from save_result where timestamp>='"+ before_timestamp +"' and pattern='POST '");
	for( result::const_iterator it = result_list->begin(); it != result_list->end(); ++it ){
		//if( ( mit = record_map.find( it[1].as( string() ) ) == record_map.end() )continue;
		mit = record_map.find( it[1].as( string() ) );
		if( mit == record_map.end() )continue;	//HOSTが登録されてなければ無視
		string host = (*mit).second;
		cout <<"src_ip="<< it[0].as(string())<<":host="<< host <<":result="<< it[2].as(string()) <<":"<<it[3].as(string()) << endl;
		for( vector<UrlAction>::iterator jt = url_action_list.begin(); jt != url_action_list.end(); jt++){
			if( jt->host==host ){
				//ACCESS COUNT
				if( it[3].as( string() ).find( jt->url,0 ) != string::npos ){
					RED
					//src_ipのカラムのactionをインクリメント。
					//if( T.exec( "update user_shop_actions set " + jt->action + "_count=" + jt->action + "_count +1 where src_ip='"+it[0].as( string() ) + "'") ){
					//	T.exec( "insert into user_shop_actions(src_ip,access_count,cart_count,buy_count) value('src_ip',0,0,0)" );
					//}
					result *res = getResult("select src_ip from user_shop_actions where src_ip like '"+ it[0].as( string() ) +"' and host= '"+ host +"'");
					if(res->size()==0){
						cout << "INSERT!!" + it[0].as( string() ) << " in host="<< host <<endl;
						getResult("insert into user_shop_actions(src_ip,host,access_day,access_month,cart,buy) values('"+ it[0].as( string() ) +"','"+ host +"',0,0,0,0)" );
					}
					cout << "UPDATE!!" + it[0].as( string() )<<" in host=" << host <<endl;
					getResult("update user_shop_actions set " + jt->action + "=" + jt->action + "+1 where src_ip='"+it[0].as( string() ) + "' and host='"+ host +"'");
					RESET
					break;
				}
			}
		}
	}
	//update timestamp
	result *tmp = getResult("select CURRENT_TIMESTAMP(0) AT TIME ZONE 'JST'" );
	result::const_iterator c = tmp->begin();
	before_timestamp = c[0].as( string() );
	cout << before_timestamp << endl;
	//conn->disconnect();
	RED cout<<"Uba::Proc() end" <<endl; RESET
}

//user_shop_actionsテーブルに問い合わせ、全てのGoodユーザのソースIPに対して静的ルーティングをする。
void Uba::VyattaProc(){
	RED cout<<"Uba::VyattaConfig() start!"<<endl;	RESET
	result *result_list = getResult("select src_ip from user_shop_actions where class='Good'");
	for( result::const_iterator it = result_list->begin(); it != result_list->end(); ++it ){
		//ここでvyattaAPIをたたき、各GoodユーザのソースIPの次ホップを高品質サーバ行きに
		//cli_config_api("set protocols static route "+ it[0].as( string() )  +" next-hop 0.0.0.0");
	}
	RED
	//char *args[] = {"/bin/echo","abc",NULL};
	//execvp(args[0],args);
	RESET
	RED cout<<"Uba::VyattaConfig() end" <<endl; RESET
}

