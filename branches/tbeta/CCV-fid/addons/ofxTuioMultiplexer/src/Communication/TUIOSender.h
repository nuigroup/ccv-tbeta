/*
*  TUIO.h
*  
*
*  Created on 2/2/09.
*  Copyright 2009 NUI Group. All rights reserved.
*
*/

#ifndef TUIO_H
#define TUIO_H

//#include "../Tracking/ContourFinder.h"
#include "ofxOsc.h"
#include "ofxNetwork.h"
//#include "ofxFiducial.h"
#include "ofMain.h"
//#include "ofxTuioMultiplexer.h"
#include "Blob.h"
#include "TuioObject.h"

//#include "Calibration/Calibration.h"

//class ofxTuioMultiplexer;

class TUIOSender {

	public:		

		TUIOSender();
		~TUIOSender();
		
		//methods
		void setup( const char* host, int port, int flashport,int netinterfaceID, string tuio_application_name);
		void setup_tuio_source_String(int id, string applicationName);
		void setNetworkInterface(int id);
		//void sendTUIO(std::map<int, Blob> * blobs);
		void sendTUIO(std::vector<Blob> &blobs, std::list<TUIO::TuioObject> &tobjList);//, std::list <ofxFiducial> *fiducialsList

		//TCP Network 
		ofxTCPServer 	m_tcpServer;
		//OSC Network 
		ofxOscSender	TUIOSocket; 
		const char*		localHost;
		int				TUIOPort;	
		int				TUIOFlashPort;
		bool 			bHeightWidth;
		bool 			bOSCMode;
		bool			bTCPMode;
		bool			bIsConnected;
		//added by Stefan Schlupek
		char*			tuio_source_String;//
		char*			host_name;//ip Adress
		const char*		tuio_source_application;
	
		int				camWidth;//need the dimension from the Camera to Normalize the Fiducial Positions
		int				camHeight;
		
		 
		//------------------

	private:
		int				frameseq;
		bool			send(string message);
		string			partialPrevMsg;
};

#endif