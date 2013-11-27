/*
 * ** Copyright (C) 2012 Shinichi Ishida, All Rights Reserved.
 * **
 * ** Please see the LICENSE file distributed with this source
 * ** code archive for information covering the modification and
 * ** redistribution of this file and binaries built from it.
 * */

/*
 * $Id: Extractor.H,v 5.0 2010-10-13 08:47:15 sin Exp $
*/

#pragma once

#include "Extractor.H"
#include "Packet.H"

class Option{
public:
	bool start;
	bool rule;
	bool encode;
	bool src_ip_port;
	bool dst_ip_port;
	bool result;

	bool query;
	bool end;
	Option(){
		start = end  = rule = encode = src_ip_port = dst_ip_port = result=query = false;
	}
	void getOption(){
		
	}
};


class Extractor
{
	private:

	public:
		Option *option;
		Extractor();
		void Proc(Packet *pkt);
};
