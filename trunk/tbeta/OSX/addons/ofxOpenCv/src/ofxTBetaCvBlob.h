/*
* ofxTBetaCvBlob.h
* openFrameworks
*
* A blob is a homogenous patch represented by a polygonal contour.
* Typically a blob tracker uses the contour to figure out the blob's
* persistence and "upgrades" it with ids and other temporal
* information.
*
*/


#ifndef OFX_CV_BLOB_H
#define OFX_CV_BLOB_H


#include "ofxCvConstants.h"


class ofxTBetaCvBlob {

    public:

        float               area;
        float               length;
        ofRectangle         boundingRect;
		ofPoint             centroid, lastCentroid, D;
        bool                hole;
	
		float				maccel;

        vector <ofPoint>    pts;    // the contour of the blob
        int                 nPts;   // number of pts;
		int					id;

		float               angle;

        //----------------------------------------
        ofxTBetaCvBlob() {
            area 		= 0.0f;
            length 		= 0.0f;
            hole 		= false;
            nPts        = 0;
        }

        //----------------------------------------
        void draw(float x = 0, float y = 0){
            ofNoFill();
            ofSetColor(0xFF00FF);
            ofBeginShape();
            for (int i = 0; i < nPts; i++){
                   ofVertex(x + pts[i].x, y + pts[i].y);
            }
            ofEndShape(true);
            ofSetColor(0xFFFFFF);
		    //ofLine(x + boundingRect.x+boundingRect.width/2,y + boundingRect.y+boundingRect.height/2,x+boundingRect.width+100,y+boundingRect.height+200); //hands
			ofFill();
			ofNoFill();
			ofSetColor(0xFF0099);
			//ofCircle(x + boundingRect.x+boundingRect.width/2,y + boundingRect.y+boundingRect.height/2,area/100);
			//ofRect(x + boundingRect.x+boundingRect.width/4, y + boundingRect.y+boundingRect.height/4, boundingRect.width/2, boundingRect.height/2);

            //ofRect(x + boundingRect.x, y + boundingRect.y, boundingRect.width, boundingRect.height);


            glPushMatrix();
                glTranslatef(x + boundingRect.x, y + boundingRect.y, 0.0f);
                glRotatef(-angle, 0.0f, 0.0f, 1.0f);
                glTranslatef(-(x + boundingRect.x), -(y + boundingRect.y), 0.0f);
                //glut(boundingRect.x - boundingRect.width, boundingRect.y - boundingRect.height, boundingRect.x + boundingRect.height,boundingRect.y + boundingRect.width, 1.0f, 0.0f, 0.0f);
                ofRect(x + boundingRect.x - boundingRect.width/2, y + boundingRect.y - boundingRect.height/2, boundingRect.width, boundingRect.height);

                ofSetColor(0x0099FF);
                ofRect(x + boundingRect.x, y + boundingRect.y - boundingRect.height, 1, boundingRect.height * 2); //Horizontal Plus
                ofRect(x + boundingRect.x - boundingRect.width, y + boundingRect.y, boundingRect.width * 2, 1); //Horizontal Plus
            glPopMatrix();



        }
};


#endif


