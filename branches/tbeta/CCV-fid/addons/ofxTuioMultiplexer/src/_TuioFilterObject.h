#ifndef INCLUDED_tuioMultiplexerObject_H
#define INCLUDED_tuioMultiplexerObject_H

//#include "ofxTuioMultiplexer.h"
#include "TuioObject.h"
#include "ofxVectorMath.h"
//using namespace TUIO ;
	class tuioMultiplexerObject {
		
	protected:
		/**
		 * The individual cursor ID number that is assigned to each TuioCursor.
		 */ 
		int object_id;
		
		//struct Tuio_Source * source;
		//struct Screen * screen;
		//struct Tuio_Source* source;
		
	public:
		//using TuioContainer::update;
		//struct Tuio_Source* _source, 
		tuioMultiplexerObject(TUIO::TuioObject * _object);
		//tuioMultiplexerCursor();

		std::list<TUIO::TuioObject *> objectList;

		


		void addTuioObject(TUIO::TuioObject * _object);
		void removeTuioObject( TUIO::TuioObject * _object);
		//TUIO::TuioCursor * gethighestTuioCursor();

		//float getX()const;
		
		//float getY() const;

		 /**
		 * The destructor is doing nothing in particular. 
		 */
		~tuioMultiplexerObject(){};

};
//};

#endif