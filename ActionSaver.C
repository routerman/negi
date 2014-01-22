#include"ActionSaver.H"

pqxx::result* ActionSaver::getResult(string query){
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

ActionSaver::ActionSaver(){
	result *result_list;
	//Initiation timstamp

	//result_list = getResult("select CURRENT_TIMESTAMP(0) AT TIME ZONE 'JST'" );
	//result::const_iterator c = result_list->begin();
	//before_timestamp = c[0].as( string() );
	before_timestamp ="2012-01-01 00:00:00";
	cout << before_timestamp << endl;



	//Initiation RecordList
   /*
	result_list = getResult("select dst_ip,host from record");
	for( result::const_iterator c = result_list->begin(); c != result_list->end(); c++ ){
		record_map.insert( make_pair( c[0].as(string()), c[1].as(string()) ) );
		//cout << c[0].as(string()) <<":"<< c[1].as(string()) << endl;
	}
   */
	record_map.insert( make_pair( "54.240.248.0","www.amazon.co.jp" ) );
	record_map.insert( make_pair( "202.72.50.10","www.rakuten.co.jp" ) );
	record_map.insert( make_pair( "202.32.114.47","www.nitori-net.jp" ) );
	record_map.insert( make_pair( "210.129.151.129","kakaku.com" ) );
	record_map.insert( make_pair( "202.247.10.161","www.takashimaya.co.jp" ) );
	record_map.insert( make_pair( "69.171.229.25","www.facebook.com" ) );
	record_map.insert( make_pair( "203.216.231.189","www.yahoo.co.jp" ) );
	record_map.insert( make_pair( "208.80.154.224","ja.wikipedia.org" ) );
	record_map.insert( make_pair( "210.189.86.12","ejje.weblio.jp" ) );
	record_map.insert( make_pair( "131.113.134.163","www.itc.keio.ac.jp" ) );
	record_map.insert( make_pair("not","found") );

	//Initiation UrlActionList
	UrlAction *tmp_urlaction;
	result_list = getResult("select host,url,action from url_action" );
	for( result::const_iterator c = result_list->begin(); c != result_list->end(); c++ ){
		tmp_urlaction = new UrlAction( c[0].as(string()), c[1].as(string()), c[2].as(string()) );
		url_action_list.push_back( *tmp_urlaction );
		//cout<<"host="<< c[0].as(string()) <<": url="<< c[1].as(string()) <<": action="<<  c[2].as(string()) << endl;
	}
	//tmp = new UrlAction("/gp/product/handle-buy-box/ref","www.amazon.co.jp","cart");
	//url_action_list.push_back(*tmp);

	except_extension.push_back(".jpg");
	except_extension.push_back(".jpeg");
	except_extension.push_back(".png");
	except_extension.push_back(".gif");
	except_extension.push_back(".ico");
	except_extension.push_back(".css");
	except_extension.push_back(".js");
	except_extension.push_back("image");
	except_extension.push_back("plugin");
	//for( vector<string>::iterator it = except_extension.begin(); it!=except_extension; it++ ){}
}

//拡張子フィルタ。resultに"css","gif"などページファイルが含まれる場合はtrue
bool ActionSaver::extension_filter( string result ){
	for( unsigned int i=0; i<except_extension.size(); i++){
		if( result.find( except_extension[i],0 ) != string::npos ){
			//cout<<"exception!"<< except_extension[i] << endl;
			return true;
		}
	}
	return false;
}

void ActionSaver::Proc(){
   bool analyze;
	string host;
   Entry *entry;
	//RED cout<<"ActionSaver::Proc() start"<<endl; RESET
	//cout  << before_timestamp << endl;
	result *result_list = getResult("select timestamp,src_ip,dst_ip,pattern,result from save_result where timestamp>='"+ before_timestamp +"' and ( pattern='GET ' or pattern='POST ' )");
	//result *result_list = getResult("select timestamp,src_ip,dst_ip,pattern,result from save_result where ( pattern='GET ' or pattern='POST ' )");
	//result *result_list = getResult("select src_ip,dst_ip,pattern,result from save_result where timestamp>='"+ before_timestamp +"' and pattern='POST '");
	for( result::const_iterator c = result_list->begin(); c != result_list->end(); ++c ){
		//search host from record_map
		//cout<<"timestamp="<< c[0].as(string())<<": src_ip="<< c[1].as(string()) <<": dst_ip="<< c[2].as(string())<<endl;
      before_timestamp = c[0].as( string() );
		mit = record_map.find( c[2].as(string()) );
		//if ( mit == record_map.end() ) continue;
		if ( mit == record_map.end() ){
			host=c[2].as( string() );
         analyze = false;
         //continue;
		}else{
			host = (*mit).second;
         analyze = true;
		}
		//extension_filter
		if ( extension_filter( c[4].as(string()) ) == true ) continue;

		//cout<<"timestamp="<< c[0].as(string())<<": src_ip="<< c[1].as(string()) <<": host="<< host<<endl;
		//timestamp,src_ip,dst_ip
		entry = new Entry( c[0].as(string()), c[1].as(string()), host);
		//cout<<"timestamp="<< entry->timestamp <<": src_ip="<< entry->src_ip <<": host="<< entry->host<<endl;
		//Analyze
		if(analyze)AnalyzeAction(c, entry);
		//write Log Table
		LogTable(entry);
		//write count table
		CountTable(entry);
		delete entry;
	}
	//update timestamp
	//result *tmp = getResult("select CURRENT_TIMESTAMP(0) AT TIME ZONE 'JST'" );
	//result::const_iterator c = tmp->begin();
	//before_timestamp = c[0].as( string() );
	//cout << before_timestamp << endl;
	//conn->disconnect();
	//RED cout<<"ActionSaver::Proc() end" <<endl; RESET
}

void ActionSaver::Proc( Packet *pkt ){
	RED cout<<"ActionSaver::Proc( PapaResult it) start"<<endl; RESET
	Entry *entry;
	cout  << before_timestamp << endl;
	if(pkt->GetStream() != 0){
		for(list<PapaResult*>::iterator it = pkt->GetStream()->GetPapaResultListFirstIt(); it != pkt->GetStream()->GetPapaResultListLastIt(); it++){
			if((*it)->GetPRule()->GetSaveFlag()){
				/*
				//search host from record_map
				mit = record_map.find( it );
				if ( mit == record_map.end() ) continue;
				string host = (*mit).second;
				//extension_filter
				if ( extension_filter( c[4].as(string()) ) == true ) continue;

				//cout<<"timestamp="<< c[0].as(string())<<": src_ip="<< c[1].as(string()) <<": host="<< host<<endl;
				//timestamp,src_ip,dst_ip
				entry = new Entry( c[0].as(string()), c[1].as(string()), host);
				//cout<<"timestamp="<< entry->timestamp <<": src_ip="<< entry->src_ip <<": host="<< entry->host<<endl;
				//Analyze
				AnalyzeAction(c, entry);
				//write Log Table
				LogTable(entry);
				//write count table
				CountTable(entry);
				delete entry;
				*/
			}
		}
		//update timestamp
		result *tmp = getResult("select CURRENT_TIMESTAMP(0) AT TIME ZONE 'JST'" );
		result::const_iterator c = tmp->begin();
		before_timestamp = c[0].as( string() );
		cout << before_timestamp << endl;
		//conn->disconnect();
	}
}

void ActionSaver::AnalyzeAction( pqxx::result::const_iterator c, Entry *entry){
	//ここでデータベースに挿入できる形までentryを加工する
	//type,actions,url,titile,object
	//cout <<"ActionSaver::AnalyzeActions() start!"<<endl;
	//entry->type 	  = c[0].as( string() );	
	//entry-> = c[0].as( string() );
	string uri = c[4].as(string());
	//cout << uri << endl;
	if( uri.find(" HTTP/",0) != string::npos){
		uri.resize( uri.find(" HTTP/",0) );
		//cout << uri << endl;
	}
	entry->url = "http://" + entry->host + uri;
	for( vector<UrlAction>::iterator it = url_action_list.begin(); it != url_action_list.end(); it++){
		if( it->getHost()==entry->host && c[3].as(string())=="POST "  && uri.find( it->getUrl(),0 ) != string::npos ){
			RED cout << it->getAction() << "!! src_ip=" + entry->src_ip +" in host="+ entry->host <<endl; RESET
			entry->action = it->getAction();
			entry->type = "EC";
			//entry-> = it->getAction();
			break;
		}
	}
}

//Log to action_log table
void ActionSaver::LogTable(Entry *entry){
	//RED cout<<"ActionSaver::LogTable() start!"<<endl; RESET
	//getResult("insert into action_log(timestamp,src_ip,host,service_type,action,url,title,object) values('"+ entry->timestamp +"','"+ entry->src_ip +"','"+ entry->host +"','"+ entry->type +"','"+ entry->action +"','"+ entry->url +"','"+ entry->title +"',"'+ entry->object +'")" );
	//getResult("insert into action_log(timestamp,src_ip,host,action,url) values('"+ entry->timestamp +"','"+ entry->src_ip +"','"+ entry->host +"','"+ entry->action +"',E'"+ escape_binary( entry->url, 30 )+"')");
	getResult("insert into action_log(timestamp,src_ip,host,action,url) values('"+ entry->timestamp +"','"+ entry->src_ip +"','"+ entry->host +"','"+ entry->action +"',E'"+ entry->url+"')");
	//cout<<"insert into action_log(timestamp,src_ip,host,action,url) values('"+ entry->timestamp +"','"+ entry->src_ip +"','"+ entry->host +"','"+ entry->action +"',E'"+ entry->url +"')" <<endl;
}

//void ActionSaver::update_shoptable( pqxx::result::const_iterator c ,string host){
void ActionSaver::CountTable( Entry *entry ){
	//RED cout<<"ActionSaver::CountTable() start!"<<endl; RESET
	//RED cout<<"ActionSaver::update_shoptable() start"<<endl; RESET
	//アクセスカウント。ホントはこう書きたい。
	//if( getResult( "update user_shop_actions set access_day=access_day+1 where src_ip='"+it[0].as( string() )+"' and host='"+host+"'") ){
	//	getResult("insert into user_shop_actions(src_ip,host,access_day,access_month,cart,buy) values('"+it[0].as(string())+"','"+host+"',0,0,0,0)" );
	//}
	result *res = getResult("select src_ip from action_count where src_ip='"+ entry->src_ip +"' and host='"+ entry->host +"'");
	if(res->size()==0){
		//RED cout << "INSERT!!" + entry->src_ip << " in host="<< entry->host <<endl; RESET
		getResult("insert into action_count(src_ip,host,access_day,access_month,cart,buy,train_flag) values('"+ entry->src_ip +"','"+ entry->host +"',0,0,0,0,0)" );
	}
	//RED cout << "ACCESS!!" + entry->src_ip + " in host=" << entry->host <<endl; RESET
	//getResult("update action_count set access_day=access_day+1 where src_ip='"+ entry->src_ip +"' and host='"+ entry->host +"'");
	if( entry->action == "access" )entry->action += "_day";
	if( entry->action.empty() == false ){
		//RED cout << entry->action << "!! src_ip=" + entry->src_ip +" in host="+ entry->host <<endl; RESET
		getResult("update action_count set " + entry->action + "=" + entry->action + "+1 where src_ip='"+ entry->src_ip + "' and host='"+ entry->host +"'");
	}
}

//user_shop_actionsテーブルに問い合わせ、全てのGoodユーザのソースIPに対して静的ルーティングをする。
void ActionSaver::VyattaProc(){
	RED cout<<"ActionSaver::VyattaConfig() start!"<<endl;	RESET
	result *result_list = getResult("select src_ip,host from action_count where class='Good' and train_flag=0");
	for( result::const_iterator c = result_list->begin(); c != result_list->end(); ++c ){
		cout <<"src_ip:"<< c[0].as(string()) <<" is Good user for host:"<< c[1].as(string()) <<endl;
		//ここでvyattaAPIをたたき、各GoodユーザのソースIPの次ホップを高品質サーバ行きに
		//cli_config_api("set protocols static route "+ it[0].as( string() )  +" next-hop 0.0.0.0");
		//char *args[] = {"/bin/echo","abc",NULL};
		//execvp(args[0],args);
	}
	//RED cout<<"ActionSaver::VyattaConfig() end" <<endl; RESET
}

