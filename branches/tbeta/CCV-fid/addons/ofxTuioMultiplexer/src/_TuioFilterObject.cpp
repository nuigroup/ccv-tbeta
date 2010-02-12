#include "tuioMultiplexerObject.h"
//using namespace TUIO;

tuioMultiplexerObject::TuioMultiplexer(TUIO::TuioObject * _object){

	cout << "TuioMultiplexer---------" << endl;
	addTuioObject(_object);
}

void tuioMultiplexerObject::addTuioObject( TUIO::TuioObject * _object){

	cout << "addTuioObject()---------:"<<_object->getSymbolID() << endl;
	objectList.push_back(_object);
}

void tuioMultiplexerObject::removeTuioObject( TUIO::TuioObject * _object){

	objectList.remove(_object);
}

