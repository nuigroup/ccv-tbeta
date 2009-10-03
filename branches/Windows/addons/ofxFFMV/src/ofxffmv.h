#ifndef OFXFFMV_H_INCLUDED
#define OFXFFMV_H_INCLUDED

#include "pgrflycapture.h"
#include "stdio.h"
//
// The maximum number of cameras on the bus.
//
#define _MAX_CAMS       10
class ofxffmv
{
    public:
        ofxffmv();
		~ofxffmv();
        void initFFMV(int wid,int hei,int startX=0,int startY=0);
        void setDeviceID(int id);
        int getDeviceID();
        void grabFrame();
        int getCamWidth();
        int getCamHeight();
        void listDevices();
        static int getDeviceCount();
        FlyCaptureImage fcImage[_MAX_CAMS];


    private:
        // This acts as a handle to the camera.
		FlyCaptureContext context[_MAX_CAMS];
		int fcCameraID;
        int camWidth;
        int camHeight;
        // Enumerate the cameras on the bus.
        FlyCaptureInfoEx  arInfo[ _MAX_CAMS ];
        unsigned int	  camNum ;
};
#endif // OFXFFMV_H_INCLUDED
