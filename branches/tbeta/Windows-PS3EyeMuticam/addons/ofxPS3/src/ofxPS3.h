#ifndef OFXPS3_H
#define OFXPS3_H

#include "PS3EyeMulticam.h"

class ofxPS3
{
	// Enumerate the cameras on the bus.
	static unsigned int	camNum;
	int camWidth, camHeight;
	PBYTE pBuffer;
public:
	ofxPS3();
	~ofxPS3();
	void initPS3(int width,int height, int framerate);
	int getCamWidth();
	int getCamHeight();
	void listDevices();
	static int getDeviceCount();
	void showSettings();
	bool isFrameNew();
	PBYTE getPixels();
};
#endif // OFXPS3_H_
