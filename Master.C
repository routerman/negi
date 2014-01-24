/*
 * ** Copyright (C) 2012 Shinichi Ishida, All Rights Reserved.
 * **
 * ** Please see the LICENSE file distributed with this source
 * ** code archive for information covering the modification and
 * ** redistribution of this file and binaries built from it.
 * */

/*
 * $Id: Master.C,v 5.41 2012-08-22 08:42:52 sin Exp $
*/
#include "Master.H"
#include "Global.H"
#include "SSS.H"

Master::Master(){

	observer_packet_counter = 0;
	all_packet_counter = 0;

	max_stream_size = 0;
	start_time = time(NULL);
   gettimeofday(&start[0], NULL);
	
	return;
}

Master::~Master(){
	return;
}
int pkt_cnt = 0;
void Master::Proc(Packet *pkt){
//	pkt_cnt++;
//	cout << pkt_cnt << endl;
//	pkt->Show();
	MASTER_DEBUG(MSG("Master Proc!"));
	stream_rebuild->Proc(pkt);

	if(pkt->GetError() == 0){
		//ip_filter.Proc(pkt);
	MASTER_DEBUG(MSG("http_decode"));
		http_decoder->Proc(pkt);

	MASTER_DEBUG(MSG("match_pre"));
		match_pre_filter->Proc(pkt);

	//MASTER_DEBUG(MSG("uba"));
	//	uba->Proc(pkt);

	MASTER_DEBUG(MSG("extract"));
		extractor->Proc(pkt);

	}

	OBSERVER_DEBUG(observer->ShowMem(pkt->GetTimestamp());); 

	static time_t prev_time;
	static time_t now_time;
	if(observer_packet_counter > 1000000){
		now_time = time(NULL);
		if(now_time > prev_time){
			//cout << now_time - start_time << "	" ;
			//observer->ShowMem(pkt->GetTimestamp()); 
	//		MSG("Packet Count: " << all_packet_counter);
			observer_packet_counter = 0;
			prev_time = now_time;
         gettimeofday(&end[0], NULL);
         
			MASTER_DEBUG(MSG("uba"));
         gettimeofday(&start[1], NULL);
			   action_saver->Proc();
         gettimeofday(&end[1], NULL);

			MASTER_DEBUG(MSG("user_classifier"));
         gettimeofday(&start[2], NULL);
		      user_classifier->Proc();
         gettimeofday(&end[2], NULL);

		   MASTER_DEBUG(MSG("user_recommender"));
         gettimeofday(&start[3], NULL);
			   user_recommender->Proc();
         gettimeofday(&end[3], NULL);

         printf("%6d,", (int)(end[0].tv_sec - start[0].tv_sec) * 1000 + (int)(end[0].tv_usec - start[0].tv_usec) / 1000);
         printf("%6d,", (int)(end[1].tv_sec - start[1].tv_sec) * 1000 + (int)(end[1].tv_usec - start[1].tv_usec) / 1000);
         printf("%6d,", (int)(end[2].tv_sec - start[2].tv_sec) * 1000 + (int)(end[2].tv_usec - start[2].tv_usec) / 1000);
         printf("%6d\n", (int)(end[3].tv_sec - start[3].tv_sec) * 1000 + (int)(end[3].tv_usec - start[3].tv_usec) / 1000);
         gettimeofday(&start[0], NULL);
         //NEGI()
		}
	}else{
		observer_packet_counter++;
//		all_packet_counter++;
	}
//	river_gate->ProcPacket(pkt);
	
/*
	if(pkt->GetProtocol() == IPPROTO_TCP && !(pkt->GetAck()) && (pkt->GetSyn() || pkt->GetFin())){
		pthread_mutex_lock(&sss_cond_mut);
		sss_send_value = new Packet(pkt->GetPacketContainer());
		pthread_cond_signal(&sss_cond);
		pthread_mutex_unlock(&sss_cond_mut);
	//	pkt->Show();
	}
*/


	if(!end_stream_list.empty()){
		for(list<Stream *>::iterator it=end_stream_list.begin(); it != end_stream_list.end(); it++){
			if((*it)->GetSaveFlag()){
#ifdef USE_POSTGRES
				pgsql_saver->Proc(*it);
#endif
			}else if(atoi(config->get("save_all").c_str())){
#ifdef USE_POSTGRES
				pgsql_saver->Proc(*it);
#endif
			}

		}
	}
//	pgsql_saver.ProcPacket(pkt);
	packet_clean->Proc(pkt);
//	MASTER_DEBUG(observer.Show());
}


