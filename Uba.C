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
		url_action_list.push_back(*tmp);
	}
	tmp = new UrlAction("/gp/product/handle-buy-box/ref","www.amazon.co.jp","cart");
	url_action_list.push_back(*tmp);
	tmp = new UrlAction("/gp/huc/csrf-add.html/ref","www.amazon.co.jp","cart");
	url_action_list.push_back(*tmp);
	//tmp = new UrlAction("/gp/product/handle-buy-box/ref","www.amazon.co.jp","cart");
	//url_action_list.push_back(*tmp);

	except_extension.push_back(".jpg");
	except_extension.push_back(".png");
	except_extension.push_back(".gif");
	except_extension.push_back(".css");
	except_extension.push_back(".js");
	//for( vector<string>::iterator it = except_extension.begin(); it!=except_extension; it++ ){}
}

void Uba::Proc(){
	bool flag;
	RED cout<<"Uba::Proc() start"<<endl; RESET
	cout  << before_timestamp << endl;
	//result *result_list = getResult("select src_ip,dst_ip,pattern,result from save_result where pattern='POST ' and timestamp>'"+ before_timestamp +"'");
	//result *result_list = getResult("select src_ip,dst_ip,pattern,result from save_result where pattern='GET ' and timestamp>'"+ before_timestamp +"'");
	result *result_list = getResult("select src_ip,dst_ip,pattern,result from save_result where timestamp>='"+ before_timestamp +"' and ( pattern='GET ' or pattern='POST ' )");
	//result *result_list = getResult("select src_ip,dst_ip,pattern,result from save_result where timestamp>='"+ before_timestamp +"' and pattern='POST '");
	for( result::const_iterator it = result_list->begin(); it != result_list->end(); ++it ){
		//ホストフィルタ。dst_ipが登録されていない場合はcontinue;
		//if( ( mit = record_map.find( it[1].as( string() ) ) == record_map.end() )continue;//ホントはこう書きたい
		mit = record_map.find( it[1].as( string() ) );
		if( mit == record_map.end() )continue;	
		string host = (*mit).second;

		cout <<"src_ip="<< it[0].as(string())<<":host="<< host <<":result="<< it[2].as(string()) <<":"<<it[3].as(string())<<"---->";
		//拡張子フィルタ。resultに"css","gif"などページファイルが含まれる場合はcontinue;
		flag=false;
		for( unsigned int jt=0; jt<except_extension.size(); jt++){
			if( it[3].as( string() ).find( except_extension[jt],0 ) != string::npos ){
				cout<<"exception!"<< except_extension[jt] << endl;
				flag=true;
				break;
			}
		}
		if(flag)continue;

		//アクセスカウント。ホントはこう書きたい。
		//if( getResult( "update user_shop_actions set access_day=access_day+1 where src_ip='"+it[0].as( string() )+"' and host='"+host+"'") ){
		//	getResult("insert into user_shop_actions(src_ip,host,access_day,access_month,cart,buy) values('"+it[0].as(string())+"','"+host+"',0,0,0,0)" );
		//}

		//アクセスカウント
		result *res = getResult("select src_ip from user_shop_actions where src_ip='"+it[0].as(string())+"' and host='"+host+"'");
		if(res->size()==0){
			RED cout << "INSERT!!" + it[0].as(string()) << " in host="<< host <<endl; RESET
			getResult("insert into user_shop_actions(src_ip,host,access_day,access_month,cart,buy) values('"+it[0].as(string())+"','"+host+"',0,0,0,0)" );
		}
		RED cout << "ACCESS!!" + it[0].as(string())<<" in host=" << host <<endl; RESET
		getResult("update user_shop_actions set access_day=access_day+1 where src_ip='"+it[0].as(string())+"' and host='"+host+"'");

		//該当するホストとアクションを探し、インクリメントする
		for( vector<UrlAction>::iterator jt = url_action_list.begin(); jt != url_action_list.end(); jt++){
			if( jt->host==host && it[2].as(string())=="POST "  && it[3].as( string() ).find( jt->url,0 ) != string::npos ){
				RED cout << jt->action << "!! src_ip=" + it[0].as(string()) +" in host="+host <<endl; RESET
				getResult("update user_shop_actions set " + jt->action + "=" + jt->action + "+1 where src_ip='"+it[0].as( string() ) + "' and host='"+ host +"'");
				break;
			}
		}
	}
	//update timestamp
	result *tmp = getResult("select CURRENT_TIMESTAMP(0) AT TIME ZONE 'JST'" );
	result::const_iterator c = tmp->begin();
	before_timestamp = c[0].as( string() );
	cout << before_timestamp << endl;
	//conn->disconnect();
	//RED cout<<"Uba::Proc() end" <<endl; RESET
}

//user_shop_actionsテーブルに問い合わせ、全てのGoodユーザのソースIPに対して静的ルーティングをする。
void Uba::VyattaProc(){
	RED cout<<"Uba::VyattaConfig() start!"<<endl;	RESET
	result *result_list = getResult("select src_ip,host from user_shop_actions where class='Good' and train_flag=0");
	for( result::const_iterator c = result_list->begin(); c != result_list->end(); ++c ){
		cout <<"src_ip:"<< c[0].as(string()) <<" is Good user for host:"<< c[1].as(string()) <<endl;
		//ここでvyattaAPIをたたき、各GoodユーザのソースIPの次ホップを高品質サーバ行きに
		//cli_config_api("set protocols static route "+ it[0].as( string() )  +" next-hop 0.0.0.0");
		//char *args[] = {"/bin/echo","abc",NULL};
		//execvp(args[0],args);
	}
	//RED cout<<"Uba::VyattaConfig() end" <<endl; RESET
}

