#ifndef _TUIOOSC
#define _TUIOOSC

#include "../Tracking/ofxTBetaCvContourFinder.h"
#define OF_ADDON_USING_OFXOSC		   // OSC COMMUNICATION		
#define OF_ADDON_USING_OFXNETWORK
#include "ofAddons.h"

#include <map>

class TUIOOSC {

private:

	int    frameseq;
	bool   send(string message);
	string partialPrevMsg;

public:
	
	// constructors
	TUIOOSC();
	// destructor
	~TUIOOSC();
	
	// methods
	void setup(const char* host, int port);
	void update();
	void sendOSC();

	//---------------------------------------FOR NETWORK 
	ofxTCPServer 	m_tcpServer;
	ofxOscSender	TUIOSocket; 
	const char*		localHost;
	char			remoteHost[255];
	int				TUIOPort;	
	
	bool 			bHeightWidth;
	bool 			bOSCMode;
	bool			bTCPMode;

	std::map<int, ofxTBetaCvBlob> blobs;
};

#endif