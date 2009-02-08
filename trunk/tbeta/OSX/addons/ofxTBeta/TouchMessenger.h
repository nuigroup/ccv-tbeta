#ifndef Touch_MESSENGER_H
#define Touch_MESSENGER_H
/*
 *  TouchMessenger.h
 *  tbeta
 *
 *  Created by Artem Titoulenko on 2/1/09.
 *  Copyright 2009 NUI Inc.. All rights reserved.
 *
 */
#include "Poco/FIFOEvent.h"
#include "Poco/Delegate.h"
#include "ofTypes.h"

#include "ofxTBetaCvContourFinder.h"

// message code
//	copied, adapted, practiced from ofMouseEvents.h

class content
	{
	public:
		int id, num;
		float x, y, movx, movy;
		content( int _num, float _x, float _y, float _movx, float _movy){
			id = -1;
			num = _num;
			x = _x;
			y = _y;
			movx = _movx;
			movy = _movy;
		}
		void set(int _num, float _x, float _y, float _movx, float _movy){
			num = _num;
			x = _x;
			y = _y;
			movx = _movx;
			movy = _movy;
		}
	};

class TouchMessage : public ofEventArgs {
public:
	vector<content*> contents;
	vector<int> alive;
};

class TouchListener {
protected:
	
	virtual void TouchDown(ofxTBetaCvBlob message){}
	virtual void TouchUp(ofxTBetaCvBlob message){}
	virtual void TouchMoved(ofxTBetaCvBlob message){}
	
	void TouchDown(const void* sender, ofxTBetaCvBlob& eventArgs){
		printf("being called\n");
		TouchDown(eventArgs);
	}
	void TouchUp(const void* sender, ofxTBetaCvBlob& eventArgs){
		TouchUp(eventArgs);
	}
	void TouchMoved(const void* sender, ofxTBetaCvBlob& eventArgs){
		TouchMoved(eventArgs);
	}
};

class TouchManager
	{
	public:
		//TouchMessage messenger;
		ofxTBetaCvBlob messenger;
		
		void addListener(TouchListener* listener){
			addTouchDownListener(listener);
			addTouchUpListener(listener);
			addTouchMovedListener(listener);
		}
		
		//Listeners -----------
		void addTouchDownListener(TouchListener* listener){ //TouchDown
			TouchDown += Poco::Delegate<TouchListener, ofxTBetaCvBlob>(listener, &TouchListener::TouchDown);
		}
		void addTouchUpListener(TouchListener* listener){ //TouchUp
			TouchUp += Poco::Delegate<TouchListener, ofxTBetaCvBlob>(listener, &TouchListener::TouchUp);
		}
		void addTouchMovedListener(TouchListener* listener){ //TouchMoved
			TouchMoved += Poco::Delegate<TouchListener, ofxTBetaCvBlob>(listener, &TouchListener::TouchMoved);
		}
		
		//message recievers ---
		void notifyTouchDown(void* sender){ //TouchDown
			TouchDown.notify(sender, messenger);
		}
		void notifyTouchUp(void* sender){ //TouchUp
			TouchUp.notify(sender, messenger);
		}
		void notifyTouchMoved(void* sender){ //TouchMoved
			TouchMoved.notify(sender, messenger);
		}
		
		//events -------------- MONGREL
	private:
		Poco::FIFOEvent<ofxTBetaCvBlob> TouchDown;
		Poco::FIFOEvent<ofxTBetaCvBlob> TouchUp;
		Poco::FIFOEvent<ofxTBetaCvBlob> TouchMoved;
		bool becomeSkyNet;
		
	};

extern TouchManager TouchEvents;


#endif