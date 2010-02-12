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

#include "ofxMtObject.h"

ofxMtObject::ofxMtObject(){
}

ofxMtObject::~ofxMtObject(){
}

void ofxMtObject::registerForMultitouchEvents(){
	ofxMultiTouch.addListener(this);
}

void ofxMtObject::touchDown(float _x, float _y, int _touchId, ofxMultiTouchCustomData * _data){
	int x = _x*ofGetWidth();
	int y = _y*ofGetHeight();
	if(verbose) printf("ofxMtObject::touchDown(x: %i, y: %i)\n", x, y);
	if(!enabled) return;
	
	if(hitTest(x, y)){	
		if(!activeTouches.size()){
	    onMultiTouchDown(_x, _y, _touchId, _data);	
		}
		activeTouches.push_back(_touchId);
	}
}

void ofxMtObject::touchMoved(float _x, float _y, int _touchId, ofxMultiTouchCustomData * _data){
	int x = _x*ofGetWidth();
	int y = _y*ofGetHeight();
	if(verbose) printf("ofxMtObject::touchMoved(x: %i, y: %i)\n", x, y);
	if(!enabled) return;
	
	if(hitTest(x, y)){	
		if(activeTouches.size()){
			std::list<int>::iterator fid;
			fid = find(activeTouches.begin(), activeTouches.end(), _touchId);
			if(fid==activeTouches.end()) activeTouches.push_back(_touchId);
			
			onMultiTouchMoved(_x, _y, _touchId, _data);		
		} else {
			onMultiTouchRollOver(_x, _y, _touchId, _data);
			activeTouches.push_back(_touchId);
		}
	} else {
		if(activeTouches.size()){
			if(activeTouches.size()<=1){
				std::list<int>::iterator fid;
				fid = find(activeTouches.begin(), activeTouches.end(), _touchId);
				if(fid!=activeTouches.end()){
					onMultiTouchRollOut(_x, _y, _touchId, _data);
					activeTouches.remove(_touchId);
				}
			} else {
				std::list<int>::iterator fid;
				fid = find(activeTouches.begin(), activeTouches.end(), _touchId);
				if(fid!=activeTouches.end()) activeTouches.remove(_touchId);
			}
		}
	}
}

void ofxMtObject::touchUp(float _x, float _y, int _touchId, ofxMultiTouchCustomData * _data){
	int x = _x*ofGetWidth();
	int y = _y*ofGetHeight();
	if(verbose) printf("ofxMtObject::touchUp(x: %i, y: %i)\n", x, y);
	if(!enabled) return;
	
	if(hitTest(x, y)){
		if(activeTouches.size()<=1){
			onMultiTouchUp(_x, _y, _touchId, _data);
		}
	}
	std::list<int>::iterator fid;
	fid = find(activeTouches.begin(), activeTouches.end(), _touchId);
	if(fid!=activeTouches.end()) activeTouches.remove(_touchId);
}