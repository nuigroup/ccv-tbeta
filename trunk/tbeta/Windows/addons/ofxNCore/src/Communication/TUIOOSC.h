#ifndef _TUIOOSC
#define _TUIOOSC

#include "../Tracking/ofxTBetaCvContourFinder.h"
#define OF_ADDON_USING_OFXOSC		   // OSC COMMUNICATION		
#define OF_ADDON_USING_OFXNETWORK
#include "ofAddons.h"

#include <map>

class TUIOOSC {

private:

	int		   frameseq;

public:
	
	// constructors
	TUIOOSC();
	// destructor
	~TUIOOSC();
	
	// methods
	void setup(const char* host, int port);
	void update();
	void sendOSC();

	ofxTCPServer 	m_tcpServer;

	//---------------------------------------FOR NETWORK 
//	ofxOscSender		TUIOSocket; 
	const char*			localHost;
	char				remoteHost[255];
	int					TUIOPort;	

	std::map<int, ofxTBetaCvBlob> blobs;
};

#endif