#ifndef OFXPS3_H
#define OFXPS3_H

#include "IPS3EyeLib.h"

class ofxPS3
{
    public:
        ofxPS3();
		~ofxPS3();
        void initPS3(int width,int height, int framerate);
        void setDeviceID(int id);
        int getDeviceID();
        int getCamWidth();
        int getCamHeight();
        void listDevices();
        static int getDeviceCount();
		bool isFrameNew();
		PBYTE getPixels();

		IPS3EyeLib	*pCam;
		PBYTE		pBuffer;

    private:
        // This acts as a handle to the camera.
		int fcCameraID;
        int camWidth;
        int camHeight;
        // Enumerate the cameras on the bus.
        static unsigned int	camNum;
};
#endif // OFXPS3_H_
