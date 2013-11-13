#include"Uba.H"

void Uba::Proc(){
	if(counter>20){
		RED
			cout << "Routermaaaaaaaaaaaaaaan!!!!"<<endl;
			cout << "Routermaaaaaaaaaaaaaaan!!!!"<<endl;
		RESET
		counter=0;
		try{
			connection *conn = pgsql->GetConn();
			work T(*conn);

			result R(T.exec("select src_ip,pattern,result from save_result"));
			RED
			for( result::const_iterator c = R.begin(); c != R.end(); ++c ){
				cout <<"src_ip ="<< c[0].as( string() ) << endl;
				cout <<"pattern="<< c[1].as( string() ) << endl;
				cout <<"result ="<< c[2].as( string() ) << endl;
			}
			RESET
			T.commit();
			//conn->disconnect();
		}
		catch(const exception &e){
			cerr << e.what() << endl;
		}
		catch(...){
			cerr << "routerman >> unhandled error!! :)" << endl;
		}
		//データベースに問い合わせる
		//Result result;
		for(;false;){
			//result
			//result_list.push_back(result);
		}
	}
}
Uba::Uba(){
	counter=0;
}
