#ifndef _TUIOOSC
#define _TUIOOSC

#include "../Tracking/ofxTBetaCvContourFinder.h"
#define OF_ADDON_USING_OFXOSC		   // OSC COMMUNICATION
#include "ofAddons.h"

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

	//---------------------------------------FOR NETWORK
	ofxOscSender		TUIOSocket;
	const char*			localHost;
	char				remoteHost[255];
	int					TUIOPort;

	map<int, ofxTBetaCvBlob> blobs;
};

#endif
