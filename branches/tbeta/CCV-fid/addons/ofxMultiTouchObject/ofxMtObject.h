/*
 MultitouchObject for OpenFrameworks 
 Copyright (c) 2009 Matthias Dörfelt <info@mokafolio.de>
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

//Use this as a base class for your buttons if you only need a callback function
//inside the button object

#ifndef _OFXMTOBJECT
#define _OFXMTOBJECT

#include "ofxMSAInteractiveObject.h"
#include "ofxMultiTouch.h"

class ofxMtObject : public ofxMSAInteractiveObject, public ofxMultiTouchListener{
	
public:
	ofxMtObject();
	virtual ~ofxMtObject();
	void registerForMultitouchEvents();
	
	//flashStyle Callbacks for Multitouch Events
	virtual void onMultiTouchDown(float x, float y, int touchId, ofxMultiTouchCustomData *data = NULL){};
	virtual void onMultiTouchMoved(float x, float y, int touchId, ofxMultiTouchCustomData *data = NULL){};
	virtual void onMultiTouchRollOut(float x, float y, int touchId, ofxMultiTouchCustomData *data = NULL){};
	virtual void onMultiTouchRollOver(float x, float y, int touchId, ofxMultiTouchCustomData *data = NULL){};
	virtual void onMultiTouchUp(float x, float y, int touchId, ofxMultiTouchCustomData *data = NULL){};
	//not implemented yet
	virtual void onMultiTouchDoubleTap(float x, float y, int touchId, ofxMultiTouchCustomData *data = NULL){};

protected:
	//global Callbacks, don't modify anthing unless you know what you are doing!
	void touchDown(float x, float y, int touchId, ofxMultiTouchCustomData *data = NULL);
	void touchMoved(float x, float y, int touchId, ofxMultiTouchCustomData *data = NULL);
	void touchUp(float x, float y, int touchId, ofxMultiTouchCustomData *data = NULL);
	//not implemented yet
	void touchDoubleTap(float x, float y, int touchId, ofxMultiTouchCustomData *data = NULL){};
	
	//keeps track of all touches active on the object
	std::list<int> activeTouches;
};

#endif
