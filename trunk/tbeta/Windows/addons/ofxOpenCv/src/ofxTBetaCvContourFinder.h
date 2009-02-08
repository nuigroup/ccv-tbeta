/*
* ofxTBetaCvContourFinder.h
*
* Finds white blobs in binary images and identifies
* centroid, bounding box, area, length and polygonal contour
* The result is placed in a vector of ofxTBetaCvBlob objects.
*
*/

#ifndef OFX_CV_CONTOUR_FINDER
#define OFX_CV_CONTOUR_FINDER



#include "ofxCvConstants.h"
#include "ofxTBetaCvBlob.h"
#include "ofxCvGrayscaleImage.h"

class ofxTBetaCvContourFinder {

  public:

    ofxTBetaCvContourFinder();
    ~ofxTBetaCvContourFinder();
    int findContours( ofxCvGrayscaleImage& input,
                       int minArea, int maxArea,
                       int nConsidered, bool bFindHoles,
                       bool bUseApproximation = true);

                       // approximation = don't do points for all points of the contour, if the contour runs
                       // along a straight line, for example...


    void                    draw() { draw (0,0); }; // just draw at 0,0
    void                    draw( float x, float y );

    int                     	nBlobs;     // how many did we find
    vector <ofxTBetaCvBlob>       	blobs;      // the blobs, in a std::vector...
    ofxTBetaCvBlob                	getBlob(int num);

  protected:

    // this is stuff, not for general public to touch -- we need
    // this to do the blob detection, etc.

    ofxCvGrayscaleImage      inputCopy;
    CvMemStorage*           contour_storage;
    CvMemStorage*           storage;
    CvMoments*              myMoments;
    // internally, we find cvSeqs, they will become blobs.
    CvSeq*                  cvSeqBlobs[MAX_NUM_CONTOURS_TO_FIND];
    int                     nCvSeqsFound;
    // imporant!!
    void                    reset();

};



#endif
