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

}

void testApp::TouchUp( ofxTBetaCvBlob b)
{

}

void testApp::TouchMoved( ofxTBetaCvBlob b)
{
	
	ofEllipse( b.centroid.x, b.centroid.y, 10, 10);

}


