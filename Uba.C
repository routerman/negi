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
	Record *tmp_record;
	result_list = getResult("select dst_ip,host,action_table from dns");
	for( result::const_iterator c = result_list->begin(); c != result_list->end(); c++ ){
		tmp_record = new Record( c[0].as(string()), c[1].as(string()), c[2].as(string()));
		record_list.push_back( *tmp_record );
		//cout << c[0].as(string()) <<":"<< c[1].as(string()) << endl;
	}

	//Initiation UrlActionList
	UrlAction *tmp_urlaction;
	result_list = getResult("select host,url,action from action_shop" );
	for( result::const_iterator c = result_list->begin(); c != result_list->end(); c++ ){
		tmp_urlaction = new UrlAction( c[0].as(string()), c[1].as(string()), c[2].as(string()));
		url_action_list.push_back( *tmp_urlaction );
		//cout<<"url="<< c[0].as(string())<<":host=":<< c[1].as(string()) <<":action"<<  c[2].as(string()) << endl;
	}
	//tmp = new UrlAction("/gp/product/handle-buy-box/ref","www.amazon.co.jp","cart");
	//url_action_list.push_back(*tmp);

	except_extension.push_back(".jpg");
	except_extension.push_back(".png");
	except_extension.push_back(".gif");
	except_extension.push_back(".ico");
	except_extension.push_back(".css");
	except_extension.push_back(".js");
	//for( vector<string>::iterator it = except_extension.begin(); it!=except_extension; it++ ){}
}

//ホストテーブル解決。dst_ipが登録されていない場合はfalse
bool Uba::setHostTable(string dst_ip, Entry *entry){
	for( vector<Record>::iterator it = record_list.begin(); it != record_list.end(); it++ ){
		if( it->dst_ip == dst_ip ){
			entry->host = it->host;
			entry->table = it->table;
			return true;
		}
	}
	return false;
}

//拡張子フィルタ。resultに"css","gif"などページファイルが含まれる場合はtrue
bool Uba::extension_filter( string result ){
	for( unsigned int i=0; i<except_extension.size(); i++){
		if( result.find( except_extension[i],0 ) != string::npos ){
			cout<<"exception!"<< except_extension[i] << endl;
			return true;
		}
	}
	return false;
}

void Uba::Proc(){
	Entry *entry;
	RED cout<<"Uba::Proc() start"<<endl; RESET
	cout  << before_timestamp << endl;
	result *result_list = getResult("select src_ip,dst_ip,pattern,result from save_result where timestamp>='"+ before_timestamp +"' and ( pattern='GET ' or pattern='POST ' )");
	//result *result_list = getResult("select src_ip,dst_ip,pattern,result from save_result where timestamp>='"+ before_timestamp +"' and pattern='POST '");
	for( result::const_iterator c = result_list->begin(); c != result_list->end(); ++c ){
		//src_ip,pattern.result
		entry = new Entry( c[0].as(string()), c[2].as(string()), c[3].as(string()) );
		if( setHostTable( c[1].as(string()), entry) == false ){
			delete entry;
			continue;
		}
		cout <<"src_ip="<< c[0].as(string())<<":host="<< entry->host <<":result="<< c[2].as(string()) <<":"<< c[3].as(string())<<"---->";
		if( extension_filter( c[3].as(string()) ) == true ){
			delete entry;
			continue;
		}
		//Tableごとに行動履歴の処理を切り替える。
		//cout << entry->table;
		if( entry->table == "user_actions_shop" ){
			update_shoptable(entry);
		}else if( entry->table=="playback_history" ){
			//update_shoptable(c);
		}
		delete entry;
	}
	//update timestamp
	result *tmp = getResult("select CURRENT_TIMESTAMP(0) AT TIME ZONE 'JST'" );
	result::const_iterator c = tmp->begin();
	before_timestamp = c[0].as( string() );
	cout << before_timestamp << endl;
	//conn->disconnect();
	//RED cout<<"Uba::Proc() end" <<endl; RESET
}



//void Uba::update_shoptable( pqxx::result::const_iterator c ,string host){
void Uba::update_shoptable( Entry *entry ){
	//RED cout<<"Uba::update_shoptable() start"<<endl; RESET
	//アクセスカウント。ホントはこう書きたい。
	//if( getResult( "update user_shop_actions set access_day=access_day+1 where src_ip='"+it[0].as( string() )+"' and host='"+host+"'") ){
	//	getResult("insert into user_shop_actions(src_ip,host,access_day,access_month,cart,buy) values('"+it[0].as(string())+"','"+host+"',0,0,0,0)" );
	//}
	
	//アクセスカウントi
	result *res = getResult("select src_ip from user_actions_shop where src_ip='"+ entry->src_ip +"' and host='"+ entry->host +"'");
	if(res->size()==0){
		RED cout << "INSERT!!" + entry->src_ip << " in host="<< entry->host <<endl; RESET
		getResult("insert into user_actions_shop(src_ip,host,access_day,access_month,cart,buy) values('"+ entry->src_ip +"','"+ entry->host +"',0,0,0,0)" );
	}
	RED cout << "ACCESS!!" + entry->src_ip + " in host=" << entry->host <<endl; RESET
	getResult("update user_actions_shop set access_day=access_day+1 where src_ip='"+ entry->src_ip +"' and host='"+ entry->host +"'");

	//該当するホストとアクションを探し、インクリメントする
	for( vector<UrlAction>::iterator it = url_action_list.begin(); it != url_action_list.end(); it++){
		if( it->getHost()==entry->host && entry->pattern=="POST "  && entry->result.find( it->getUrl(),0 ) != string::npos ){
			RED cout << it->getAction() << "!! src_ip=" + entry->src_ip +" in host="+ entry->host <<endl; RESET
			getResult("update user_actions_shop set " + it->getAction() + "=" + it->getAction() + "+1 where src_ip='"+ entry->src_ip + "' and host='"+ entry->host +"'");
			break;
		}
	}
}

//user_shop_actionsテーブルに問い合わせ、全てのGoodユーザのソースIPに対して静的ルーティングをする。
void Uba::VyattaProc(){
	RED cout<<"Uba::VyattaConfig() start!"<<endl;	RESET
	result *result_list = getResult("select src_ip,host from user_actions_shop where class='Good' and train_flag=0");
	for( result::const_iterator c = result_list->begin(); c != result_list->end(); ++c ){
		cout <<"src_ip:"<< c[0].as(string()) <<" is Good user for host:"<< c[1].as(string()) <<endl;
		//ここでvyattaAPIをたたき、各GoodユーザのソースIPの次ホップを高品質サーバ行きに
		//cli_config_api("set protocols static route "+ it[0].as( string() )  +" next-hop 0.0.0.0");
		//char *args[] = {"/bin/echo","abc",NULL};
		//execvp(args[0],args);
	}
	//RED cout<<"Uba::VyattaConfig() end" <<endl; RESET
}

