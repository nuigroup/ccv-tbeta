#ifndef _TEST_APP
#define _TEST_APP

//if standalone mode/non-addon
#define STANDALONE

//main
#include "ofMain.h"
//addon
#include "ofxNCore.h"

class testApp : public ofBaseApp, public TouchListener
{
public:
	testApp() 
	{
		TouchEvents.addListener(this);
	}
	ofxNCoreVision * tbeta;

	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased();

	//Touch Events
	void TouchDown(Blob b);
	void TouchMoved(Blob b);
	void TouchUp(Blob b);
};

#endif

