#include"Uba.H"

void Uba::Proc(){
	if(counter>20){
		RED cout<<"Uba::Proc() start"<<endl; RESET
		counter=0;
		try{
			connection *conn = pgsql->GetConn();
			work T(*conn);
			result result_list( T.exec("select src_ip,dst_ip,pattern,result,timestamp from save_result where pattern like 'POST'") );
			//T.commit();
			for( result::const_iterator it = result_list.begin(); it != result_list.end(); ++it ){
				//cout << "src_ip =" << it[0].as( string() ) << ": pattern=" << it[1].as( string() ) << ": result =" << it[2].as( string() ) << endl;
				string host = getHost( it[1].as(string()) ); 
				for( vector<UrlAction>::iterator jt = url_action_list.begin(); jt != url_action_list.end(); jt++){
					//とりあえず、resultにurlが含むかどうか。
					if( it[3].as( string() ).find( jt->url,0) != string::npos){
						RED
						//src_ipのカラムのactionをインクリメント。
						//if( T.exec( "update user_shop_actions set " + jt->action + "_count=" + jt->action + "_count +1 where src_ip='"+it[0].as( string() ) + "'") ){
						//	T.exec( "insert into user_shop_actions(src_ip,access_count,cart_count,buy_count) value('src_ip',0,0,0)" );
						//}
						result list(T.exec("select src_ip from user_shop_actions where src_ip like '"+ it[0].as( string() ) +"'") );
						//cout<< result_url <<endl;
						if(list.size()==0){
							cout << "INSERT!!" + it[0].as( string() ) << endl;
							T.exec( "insert into user_shop_actions(src_ip,access_day,access_month,cart,buy) values('"+ it[0].as( string() ) +"',0,0,0,0)" );
						}
						cout << "UPDATE!!" + it[0].as( string() ) << endl;
						T.exec( "update user_shop_actions set " + jt->action + "=" + jt->action + "+1 where src_ip='"+it[0].as( string() ) + "'");
						RESET
						//T.commit();
						break;
					}
				}
				if( it==result_list.end() ){
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
		if(jubacounter > 10){
			jubacounter=0;
			jubatus_test();
		}else{
			jubacounter++;
		}
		RED cout<<"Uba::Proc() end" <<endl; RESET
	}
}

/* initiation of jubaclassifier */
void Uba::InitJubatus(){
	//初めにjubaclassifierに学習させる。
	//jubatus::classifier::client::classifier client("localhost",9199,1.0);
	//vector<pair<string,datum> > train_datta;
	//train_data.pushback

}

/* jubaclassifier classifies user */
void Uba::jubatus_test(){
	RED
	cout<<"tell me jubatus!"<<endl;
	RESET
	//jubatus::classifier::client::classifier client("localhost",9199,1.0);
	//定期的にuserテーブルにuser情報を問い合わせ、jubatusに分類してもらう。

	//結果をもとにvyattaAPIをたたく。

}

//引数のIPアドレスのHOST名を返す。無ければ、IPアドレスを返す。
string Uba::getHost(string dst_ip){
	for( vector<Record>::iterator it = record_list.begin(); it!=record_list.end(); it++){
		if( it->dst_ip==dst_ip ){
			return it->host;
		}
	}
	return dst_ip;//登録されてない場合はdst_ipをそのまま返すか、dnsに問い合わせるか。
}

//データベースのデータをリストに展開する。それか、DNSにホスト名からIPをといあわせるか？
void Uba::InitRecordList(){
	//Record *tmp;
	//とりあえず、最初は、直打ち
	//tmp = new Record("","nitori"): 
	//record_list.push_back(tmp);
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
	InitRecordList();
	InitUrlActionList();
	InitJubatus();
}
