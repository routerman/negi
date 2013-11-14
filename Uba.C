#include"Uba.H"

void Uba::Proc(){
	if(counter>20){
		counter=0;
		try{
			connection *conn = pgsql->GetConn();
			work T(*conn);
			result action_list(T.exec("select src_ip,pattern,result from save_result where pattern like 'POST' ") );
			//T.commit();
			for( result::const_iterator it = action_list.begin(); it != action_list.end(); ++it ){
				//cout << "src_ip =" << it[0].as( string() ) << ": pattern=" << it[1].as( string() ) << ": result =" << it[2].as( string() ) << endl;
				for( vector<UrlAction>::iterator jt = urlaction_list.begin(); jt != urlaction_list.end(); jt++){
					//urlに対応するactionが見つかれば、
					//とりあえず、含むかどうか。
					string result_url=it[2].as(string());
					if( result_url.find( jt->url,0) != string::npos){
						//RED
						//cout<<"wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww you accessed "<< jt->url <<endl;
						//RESET
						//src_ipのカラムのactionをインクリメント。
						//if( T.exec( "update user_actions set " + jt->action + "_count=" + jt->action + "_count +1 where src_ip='"+it[0].as( string() ) + "'") ){
						//	T.exec( "insert into user_actions(src_ip,access_count,cart_count,buy_count) value('src_ip',0,0,0)" );
						//}
						result list(T.exec("select src_ip from user_actions where src_ip like '"+ it[0].as( string() ) +"'") );
						RED
						cout<< result_url <<endl;
						if(list.size()==0){
							cout<< "INSERT!!" + it[0].as( string() );
							T.exec( "insert into user_actions(src_ip,access_day,access_month,cart,buy) values('"+ it[0].as( string() ) +"',0,0,0,0)" );
						}
						cout<< "UPDATE!!" <<endl;
						T.exec( "update user_actions set " + jt->action + "=" + jt->action + "+1 where src_ip='"+it[0].as( string() ) + "'");
						RESET
						//T.commit();
						break;
					}
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
	}
}

/* initiation of jubaclassifier */
void Uba::jubatus_train(){
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
	//定期的にuserテーブルにuser情報を問い合わせ、jubatusに分類してもらう。

	//結果をもとにvyattaAPIをたたく。

}

Uba::Uba(){
	counter=0;
	jubacounter=0;
	jubatus_train();
	UrlAction *tmp;
	tmp = new UrlAction("/shop/cart/cart.aspx","www.nitori-net.jp","cart");
	urlaction_list.push_back(*tmp);
	
}
