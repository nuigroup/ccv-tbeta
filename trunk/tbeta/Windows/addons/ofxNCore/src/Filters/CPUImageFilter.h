/*
*  CPUImageFilter.h
*  
*
*  Created on 2/2/09.
*  Copyright 2009 NUI Group\Inc.. All rights reserved.
*
*/

#ifndef CPUImageFilter_H
#define CPUImageFilter_H

#include "ofxCvGrayscaleImage.h"

class CPUImageFilter : public ofxCvGrayscaleImage {

  public:

    CPUImageFilter(){};

    void operator = ( unsigned char* _pixels );
    void operator = ( ofxCvGrayscaleImage& mom );
    void operator = ( ofxCvColorImage& mom );
    void operator = ( ofxCvFloatImage& mom );

	//amplifies signal
	void amplify( CPUImageFilter& mom, float level );
	//picks out light spots from image
	void highpass(float blur1, float blur2 );
};

#endif

