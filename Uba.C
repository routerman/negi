#include"Uba.H"

void Uba::Proc(){
	if(counter>10){
		RED cout<<"Uba::Proc() start"<<endl; RESET
		counter=0;
		try{
			connection *conn = pgsql->GetConn();
			work T(*conn);
			result *result_list;
			result_list = new result( T.exec("select src_ip,dst_ip,pattern,result,timestamp from save_result where pattern like 'POST'") );
			//T.commit();
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
						//cout<< result_url <<endl;
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
				if( it==result_list->end() ){
					before_timestamp = it[4].as( string() );
				}
			}
			T.commit();
			//conn->disconnect();
		}
		catch(const exception &e){
			cerr << e.what() << endl;
		}
		catch(...){
			cerr << "routerman >> unhandled error!! :)" << endl;
		}
		JubatusProc();
		RED cout<<"Uba::Proc() end" <<endl; RESET
	}
}

//データベースのデータをリストに展開する。それか、DNSにホスト名からIPをといあわせるか？
void Uba::InitRecordList(){
	//とりあえず、最初は、直打ち	そのうちデータベース上に。
	//record_map.insert(make_pair("",""));
	record_map.insert(make_pair("54.240.248.0"   ,"www.amazon.co.jp"));
	record_map.insert(make_pair("202.72.50.10"   ,"www.rakuten.co.jp"));
	record_map.insert(make_pair("202.32.114.47"  ,"www.nitori-net.jp"));
	record_map.insert(make_pair("210.129.151.129","kakaku.com"));
	record_map.insert(make_pair("202.247.10.161" ,"www.takashimaya.co.jp"));
	record_map.insert(make_pair("69.171.229.25"  ,"www.facebook.com"));
	record_map.insert(make_pair("203.216.231.189","www.yahoo.co.jp"));
	//record_map.insert(make_pair("",""));
	//record_map.insert(make_pair("",""));
	//record_map.insert(make_pair("",""));
	record_map.insert(make_pair("*","not found!"));
}

void Uba::InitUrlActionList(){
	UrlAction *tmp;
	tmp = new UrlAction("/shop/cart/cart.aspx","www.nitori-net.jp","cart");
	url_action_list.push_back(*tmp);
}

Uba::Uba(){
	//char ctstamp[32];
	//struct timeval tmp_time = ;
	//strftime(ctstamp,32,"%Y-%m-%d %H:%M:%S", (const struct tm *)localtime(&tmp_time.tv_sec) );
	//before_timestamp = ctstamp;
/*
	before_timestamp="";
	connection *conn = pgsql->GetConn();
	work T(*conn);
	result res( T.exec("select CURRENT_TIMESTAMP(0)") );
	result::const_iterator it = res.begin();
	before_timestamp=it;
	T.commit();
*/
	//client = new jubatus::classifier::client::classifier("localhost",9199,1.0);

	before_timestamp="";
	//strptime(,,);
	counter=0;
	jubacounter=0;
	vyatta_counter=0;
	InitRecordList();
	InitUrlActionList();
	InitJubatus();
}

/* initiation of jubaclassifier */
void Uba::InitJubatus(){
	//初めにjubaclassifierに学習させる。
	//jubatus::classifier::client::classifier client("localhost",9199,1.0);
	//vector<pair<string,datum> > train_datta;
	//train_data.pushback

}

/* jubaclassifier classifies user */
void Uba::JubatusProc(){
	//jubatus::classifier::client::classifier client("localhost",9199,1.0);
	//定期的にuserテーブルにuser情報を問い合わせ、jubatusに分類してもらう。
	//結果をもとにvyattaAPIをた
	jubacounter++;
	if(jubacounter>=2){
		RED cout<<"Uba::jubatus_test() start()!"<<endl;	RESET
		jubacounter=0;
		try{
			connection *conn = pgsql->GetConn();
			work T(*conn);
			result *result_list;
			result_list = new result( T.exec("select src_ip,access_day,access_month,cart,buy from user_shop_actions where access_day>=10") );
			//make data
			//send Jubatus
			//receive result
			
			for( result::const_iterator it = result_list->begin(); it != result_list->end(); ++it ){
				//NOP yet..
				//T.exec( "update user_shop_actions set " + jt->action + "=" + jt->action + "+1 where src_ip='10.24.129.200' and host='"+ host +"'");
			}
			T.commit();
		}
		catch(const exception &e){
			cerr << e.what() << endl;
		}
		catch(...){
			cerr << "routerman >> unhandled error!! :)" << endl;
		}
		VyattaProc();
		RED cout<<"Uba::jubatus_test() end" <<endl; RESET
	}
}



void Uba::VyattaProc(){
	//結果をもとにvyattaAPIをた
	vyatta_counter++;
	if(vyatta_counter>=1){
		RED cout<<"Uba::VyattaConfig start()!"<<endl;	RESET
		vyatta_counter=0;
		try{
			connection *conn = pgsql->GetConn();
			work T(*conn);
			result *result_list;
			result_list = new result( T.exec("select src_ip from user_shop_actions where class='Good'") );
			//NOP yet..
			for( result::const_iterator it = result_list->begin(); it != result_list->end(); ++it ){
				//vyattaAPI
				//cli_config_api("set protocols static route "+ it[0].as( string() )  +" next-hop 0.0.0.0");
			}
			T.commit();
		}
		catch(const exception &e){
			cerr << e.what() << endl;
		}
		catch(...){
			cerr << "routerman >> unhandled error!! :)" << endl;
		}
		RED cout<<"Uba::VyattaConfig() end" <<endl; RESET
	}
}


