#ifndef _TEST_APP
#define _TEST_APP

#include <iostream>
#include <fstream>
#include "ofMain.h"



//#include "ofxTuio.h"
#include "ofxTuioClient.h"
#include "ofxTuioMultiplexer.h"
#include "ofxXmlSettings.h"
#include "ofxKeyMap.h"

#include <psapi.h>







//--------------------------------------------------------
class testApp : public ofSimpleApp{

public:

	void setup();
	void update();
	void draw();

	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased();

	void objectAdded(TuioObject & tuioObject);
	void objectRemoved(TuioObject & tuioObject);
	void objectUpdated(TuioObject & tuioObject);

	void tuioAdded(TuioCursor & tuioCursor);
	void tuioRemoved(TuioCursor & tuioCursor);
	void tuioUpdated(TuioCursor & tuioCursor);

	void loadXMLSettings(string path);

	void setScreenData();
	void toggleVisibility();
	void checkForMemoryLeak();
	
	
private:
	
	ofxTuioMultiplexer * tuioMultiplexer;
	string tuioServerHost;
	int tuioServerPort;
	int tuioClientPort;
	int tuioFlashPort;
	int netinterfaceID;
	string tuio_application_name;
	ofxKeyMap keyMapper;
	ofstream filestr;


	ofxXmlSettings		XML;
	bool				fullScreen;
	bool				visible;
	bool				printToFile;
	ofxPoint2f			position;

	char				fileName [80];
	time_t				rawtime;
	struct tm *			timeinfo;

	//memory management
	PROCESS_MEMORY_COUNTERS pmc;
	HANDLE hProcess;
	long maxMemoryAllocation;
		
	

	
};

#endif
