#include "testApp.h"
#include "stdio.h"
#include "ofUtils.h"
//--------------------------------------------------------------
void testApp::setup(){

	tbeta = new ofxNCoreVision();
}


//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){

	//ofSetColor(0, 255, 255);
	//ofCircle( 200, 200, 100);

	//draw blobs
/*	std::vector<int, Blob>	blobs;
    blobs = tbeta->getBlobs();//get blobs
    for(int i=0; i < blobs.size(); i++)
    {
		Blob drawBlob;
		drawBlob = blobs[i].second;
		ofSetColor(0xFF0000);
		ofFill();
		ofEllipse( (drawBlob.centroid.x - drawBlob.boundingRect.width/2) * ofGetWidth() , 
				   (drawBlob.centroid.y - drawBlob.boundingRect.height/2) * ofGetHeight(),
				   drawBlob.boundingRect.width * ofGetWidth(), drawBlob.boundingRect.height * ofGetHeight());
	}
*/
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){

}

//--------------------------------------------------------------
void testApp::keyReleased  (int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mouseReleased(){

}

/*****************************************************************************
 *	TOUCH EVENTS
 *****************************************************************************/
void testApp::TouchDown( Blob b)
{
    ofSetColor(0xFF0000);
    ofFill();
	ofEllipse( (b.centroid.x - b.boundingRect.width/2) * ofGetWidth() , (b.centroid.y - b.boundingRect.height/2) * ofGetHeight(),
		       b.boundingRect.width * ofGetWidth(), b.boundingRect.height * ofGetHeight());
}

void testApp::TouchUp( Blob b)
{
    ofSetColor(0x0FFFFF);
    ofFill();
	ofEllipse( (b.centroid.x - b.boundingRect.width/2) * ofGetWidth() , (b.centroid.y - b.boundingRect.height/2) * ofGetHeight(),
		       b.boundingRect.width * ofGetWidth(), b.boundingRect.height * ofGetHeight());
}

void testApp::TouchMoved( Blob b)
{
	ofSetColor(0xFFFFFF);
    ofFill();
	ofEllipse( (b.centroid.x - b.boundingRect.width/2) * ofGetWidth() , (b.centroid.y - b.boundingRect.height/2) * ofGetHeight(),
		       b.boundingRect.width * ofGetWidth(), b.boundingRect.height * ofGetHeight());
}


