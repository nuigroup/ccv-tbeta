#ifndef OFX_CV_CONSTANTS_H
#define OFX_CV_CONSTANTS_H

#include <iostream>
#include "cv.h"
#include <vector>
#include "ofMain.h"
#include "ofxTBetaCvBlob.h"
//#include "ofxCvTrackedBlob.h"


#define  MAX_NUM_CONTOURS_TO_FIND   500  // alther this if you think you will
                                         // be looking for more then this
                                         // although we use a vector as output
                                         // we currently cap the results to this amount
class ofCvBlobListener {
  public:
  
    virtual void blobDown( ofxTBetaCvBlob b) = 0;
    virtual void blobMoved( ofxTBetaCvBlob b) = 0;    
    virtual void blobUp( ofxTBetaCvBlob b) = 0;
    
};


#endif
