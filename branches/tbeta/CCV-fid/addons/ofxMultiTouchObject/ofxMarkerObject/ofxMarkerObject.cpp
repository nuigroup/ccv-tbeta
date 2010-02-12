/*
 Multitouch Marker Object for OpenFrameworks 
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

#include "ofxMarkerObject.h"

ofxMarkerObject::ofxMarkerObject(){
}

ofxMarkerObject::~ofxMarkerObject(){
}

void ofxMarkerObject::registerForMarkerEvents(){
	ofxMarker.addListener(this);
}

void ofxMarkerObject::markerDown(int _sid, float _x, float _y, float _ang, ofxMarkerCustomData * _data){
	int x = _x*ofGetWidth();
	int y = _y*ofGetHeight();
	if(verbose) printf("ofxMarkerObject::markerDown(x: %i, y: %i)\n", x, y);
	if(!enabled) return;
	
	if(hitTest(x, y)){	
		if(!activeMarkers.size()){
			onMarkerDown(_sid, _x, _y, _ang, _data);	
		}
		activeMarkers.push_back(_sid);
	}
}

void ofxMarkerObject::markerMoved(int _sid, float _x, float _y, float _ang, ofxMarkerCustomData * _data){
	int x = _x*ofGetWidth();
	int y = _y*ofGetHeight();
	if(verbose) printf("ofxMarkerObject::markerMoved(x: %i, y: %i)\n", x, y);
	if(!enabled) return;
	
	if(hitTest(x, y)){	
		if(activeMarkers.size()){
			std::list<int>::iterator fid;
			fid = find(activeMarkers.begin(), activeMarkers.end(), _sid);
			if(fid==activeMarkers.end()) activeMarkers.push_back(_sid);
			
			onMarkerMoved(_sid, _x, _y, _ang, _data);		
		} else {
			onMarkerRollOver(_sid, _x, _y, _ang, _data);
			activeMarkers.push_back(_sid);
		}
	} else {
		if(activeMarkers.size()){
			if(activeMarkers.size()<=1){
				std::list<int>::iterator fid;
				fid = find(activeMarkers.begin(), activeMarkers.end(), _sid);
				if(fid!=activeMarkers.end()){
					onMarkerRollOut(_sid, _x, _y, _ang, _data);
					activeMarkers.remove(_sid);
				}
			} else {
				std::list<int>::iterator fid;
				fid = find(activeMarkers.begin(), activeMarkers.end(), _sid);
				if(fid!=activeMarkers.end()) activeMarkers.remove(_sid);
			}
		}
	}
	
}

void ofxMarkerObject::markerUp(int _sid, float _x, float _y, float _ang, ofxMarkerCustomData * _data){
	int x = _x*ofGetWidth();
	int y = _y*ofGetHeight();
	if(verbose) printf("ofxMarkerObject::markerUp(x: %i, y: %i)\n", x, y);
	if(!enabled) return;
	
	if(hitTest(x, y)){
		if(activeMarkers.size()<=1){
			onMarkerUp(_sid, _x, _y, _ang, _data);
		}
	}
	std::list<int>::iterator fid;
	fid = find(activeMarkers.begin(), activeMarkers.end(), _sid);
	if(fid!=activeMarkers.end()) activeMarkers.remove(_sid);
}
