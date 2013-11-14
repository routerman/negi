#include"Uba.H"

void Uba::Proc(){
	if(counter>20){
		RED
		cout << "Routermaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaan!!!!"<<endl;
		counter=0;
		try{
			connection *conn = pgsql->GetConn();
			work T(*conn);
			result R(T.exec("select src_ip,pattern,result from save_result where pattern like 'POST' ") );
			for( result::const_iterator c = R.begin(); c != R.end(); ++c ){
				cout <<"src_ip ="<< c[0].as( string() ) <<": pattern="<< c[1].as( string() ) <<": result ="<< c[2].as( string() ) << endl;
				for( vector<UrlAction>::iterator it = urlaction_list.begin(); it != urlaction_list.end(); it++){
					if( c[2].as(string()) == it->url ){
						//URLに対応するactionが見つかれば、userテーブルの値を更新する
						//c[0].as( string() )のIPのユーザのit->actionをインクリメントする。;
						//for( vector<User>::iterator jt = user_list.begin(); jt != user_list.end(); jt++){
							//対応するソースIPを探し、actionを
						//}	
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
		RESET
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
