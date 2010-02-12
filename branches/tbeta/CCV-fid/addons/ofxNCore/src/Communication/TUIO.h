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

#include "../Tracking/ContourFinder.h"
#include "ofxOsc.h"
#include "ofxNetwork.h"
#include "ofxFiducial.h"
#include "ofMain.h"

//#include "Calibration/Calibration.h"



class TUIO {

	public:		

		TUIO();
		~TUIO();
		
		//methods
		void setup(const char* host, int port, int flashport, int _camWidth, int _camHeight);
		void setup_tuio_source_String();
		void setNetworkInterface(int id);
		//void sendTUIO(std::map<int, Blob> * blobs);
		void sendTUIO(std::map<int, Blob> * blobs, std::list <ofxFiducial> *fiducialsList);
		void setFiducialModus(bool status);

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
		//char*			tuio_application_name;
		char*			host_name;//ip Adress
		char*			tuio_source_application;
		//char*			tuio_source_address;
		int				camWidth;//need the dimension from the Camera to Normalize the Fiducial Positions
		int				camHeight;
		bool			bTrackFiducials;
		//struct	in_addr		addr;// struct for the IP Adress
		 
		//------------------

	private:
		int				frameseq;
		bool			send(string message);
		string			partialPrevMsg;
};

#endif