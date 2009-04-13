#ifndef OFX_CV_CPUImageFilter_IMAGE
#define OFX_CV_CPUImageFilter_IMAGE

//#include "ofxCvImage.h"
//#include "ofxCvBlob.h"

// for all the things ofxCvGrayscaleImage can do, check:
// ofxCvImage.h

#include "ofxCvGrayscaleImage.h"


class CPUImageFilter : public ofxCvGrayscaleImage {

  public:

    CPUImageFilter() {};

    void operator = ( unsigned char* _pixels );
    void operator = ( ofxCvGrayscaleImage& mom );
    void operator = ( ofxCvColorImage& mom );
    void operator = ( ofxCvFloatImage& mom );

	//amplifies signal
	void amplify( CPUImageFilter& mom, float level );
	void highpass(float blur1, float blur2 );
};



#endif

