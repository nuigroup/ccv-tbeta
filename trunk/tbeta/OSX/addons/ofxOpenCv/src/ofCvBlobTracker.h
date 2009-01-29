/*
* ofCvBlobTracker.h
* by stefanix
* Thanks to touchlib for the best fit algorithm!
*
* This class tracks blobs between frames.
* Most importantly it assignes persistent id to new blobs, correlates
* them between frames and removes them as blobs dissappear. It also
* compensates for ghost frames in which blobs momentarily dissappear.
*
* Based on the trackning it fires events when blobs come into existence,
* move around, and disappear. The object which receives the callbacks
* can be specified with setListener().
*
*/


#ifndef OF_CV_BLOBTRACKER_H
#define OF_CV_BLOBTRACKER_H

#include <map>
#include <vector>

#include "ofMain.h"
#include "ofxCvMain.h"



class ofCvBlobTracker {


  public:
  
    vector<ofCvTrackedBlob>  blobs;    
    
    
    ofCvBlobTracker();
    void setListener( ofCvBlobListener* _listener );
    void trackBlobs( const vector<ofCvBlob>& blobs );
    int findOrder( int id );  // order by which the present 
                              // blobs came into existence
    ofCvTrackedBlob& getById( int id );  // returns a reference to the
                                         // corresponding blob in blobs vector
    void draw( float x, float y );  // draws all blobs



  protected:
  
    int currentID;
    int extraIDs;
    int numcheck;
    
    ofCvBlobListener* listener;

    int reject_distance_threshold;
    int ghost_frames;
    float minimumDisplacementThreshold;

    vector<vector<int> > matrix;
    vector<int> ids;
    vector<vector<ofCvTrackedBlob> > history;
    
    
    void doBlobOn( const ofCvTrackedBlob& b );    
    void doBlobMoved( const ofCvTrackedBlob& b );    
    void doBlobOff( const ofCvTrackedBlob& b );    
    
    inline void permute( int k );
    inline bool checkValid( int start );
    inline bool checkValidNew( int start );    
};



#endif

