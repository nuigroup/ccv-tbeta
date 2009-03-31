#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){	 

}

//--------------------------------------------------------------
void testApp::update(){
	
}

//--------------------------------------------------------------
void testApp::draw(){
	
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
void testApp::TouchDown( ofxTBetaCvBlob b)
{
	ofFill();
	ofSetColor(0,255,0);
	ofEllipse(b.centroid.x*ofGetWidth(), b.centroid.y*ofGetHeight(), 10,10);
}

void testApp::TouchUp( ofxTBetaCvBlob b)
{

}

void testApp::TouchMoved( ofxTBetaCvBlob b)
{
	ofFill();
	ofSetColor(255,255,255);
	ofEllipse(b.centroid.x*ofGetWidth(), b.centroid.y*ofGetHeight(), 10, 10);

}


