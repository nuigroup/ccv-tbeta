#ifndef OFXDSVL_H_INCLUDED
#define OFXDSVL_H_INCLUDED

#include "dsvl.h"
#include "stdio.h"

//#define hibyte(x) (unsigned char)((x)>>8)

class ofxDSVL
{
    public:
        ofxDSVL();
		~ofxDSVL();
        void initDSVL();
        void setDeviceID(int id);
		void showSettingsDialog();
		int getNumByes();
        int getDeviceID();
        bool grabFrame();
        int getCamWidth();
        int getCamHeight();
		bool isFrameNew();
		unsigned char * getPixels();

	private:

		//char* xml_config;
		DSVL_VideoSource *dsvl_vs;
		MemoryBufferHandle g_mbHandle;
		LONGLONG g_Timestamp;
		unsigned char *pbuffer;
		unsigned char *buffer;
		LONG	cap_width;
		LONG	cap_height;
		int		camWidth;
		int		camHeight;
		int		fps;
		int		bytes;
		double  cap_fps;
		bool	colour;
		bool	newFrame;
		int		sourceDepth_;
		
};
#endif
