/*
*  TUIO.h
*  
*
*  Created on 2/2/09.
*  Copyright 2009 NUI Group\Inc.. All rights reserved.
*
*/

#ifndef TUIO_H
#define TUIO_H

#include "../Tracking/ContourFinder.h"
#include "ofxOsc.h"
#include "ofxNetwork.h"

#include <map>

class TUIO {

	public:		

		TUIO();
		~TUIO();
		
		//methods
		void setup(const char* host, int port);
		void sendTUIO();

		//TCP Network 
		ofxTCPServer 	m_tcpServer;
		//OSC Network 
		ofxOscSender	TUIOSocket; 
		const char*		localHost;
		char			remoteHost[255];
		int				TUIOPort;			
		bool 			bHeightWidth;
		bool 			bOSCMode;
		bool			bTCPMode;

		std::map<int, Blob> blobs;

	private:

		int    frameseq;
		bool   send(string message);
		string partialPrevMsg;
};

#endif