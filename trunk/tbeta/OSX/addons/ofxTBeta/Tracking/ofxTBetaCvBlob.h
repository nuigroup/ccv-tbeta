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

#ifndef _BLOB_H
#define _BLOB_H

#include <vector>

class ofxTBetaCvBlob {

    public:

        float               area;
        float               length;
        ofRectangle         boundingRect;
		ofRectangle         angleBoundingRect;
		ofPoint             centroid, lastCentroid, D;
        bool                hole;

		float				age; //how long the blob has been at war
		float				sitting; //how long hes been sitting in the same place
		float				downTime;

		bool				simulated;

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
			simulated = false;
			age = 0.0f;
			sitting = 0.0f;
        }

        //----------------------------------------
        void draw(float x = 0, float y = 0){
			//draw angled bounding box with cross-hair
            glPushMatrix();
			    ofSetColor(0xFF0099);
                glTranslatef(x + angleBoundingRect.x, y + angleBoundingRect.y, 0.0f);
                glRotatef(-angle, 0.0f, 0.0f, 1.0f);
                glTranslatef(-(x + angleBoundingRect.x), -(y + angleBoundingRect.y), 0.0f);
                ofRect(x + angleBoundingRect.x - angleBoundingRect.width/2, y + angleBoundingRect.y - angleBoundingRect.height/2, angleBoundingRect.width, angleBoundingRect.height);

				ofSetColor(0x0099FF);
                ofRect(x + angleBoundingRect.x, y + angleBoundingRect.y - angleBoundingRect.height, 1, angleBoundingRect.height * 2); //Horizontal Plus
                ofRect(x + angleBoundingRect.x - angleBoundingRect.width, y + angleBoundingRect.y, angleBoundingRect.width * 2, 1); //Horizontal Plus
            glPopMatrix();
        }

        void drawContours(float x = 0, float y = 0, float width = ofGetWidth(), float height = ofGetHeight()) {
            //draw contours
		    ofNoFill();
            ofSetColor(0xFFFFFF);

            /*glPushMatrix();
			    ofSetColor(0xFF0099);
                glTranslatef(x + angleBoundingRect.x/width * ofGetWidth(), y + angleBoundingRect.y/(height) * ofGetHeight(), 0.0f);
                glRotatef(-angle, 0.0f, 0.0f, 1.0f);*/
            ofBeginShape();
            for (int i = 0; i < nPts; i++){
                   ofVertex(x + pts[i].x/width * ofGetWidth(), y + pts[i].y/(height) * ofGetHeight());
            }
            ofEndShape(true);
                /*ofSetColor(0x0099FF);
                ofRect(x + angleBoundingRect.x, y + angleBoundingRect.y - angleBoundingRect.height, 1, angleBoundingRect.height * 2); //Horizontal Plus
                ofRect(x + angleBoundingRect.x - angleBoundingRect.width, y + angleBoundingRect.y, angleBoundingRect.width * 2, 1); //Horizontal Plus
            glPopMatrix();*/
        }
};
#endif


