/*
 TUIO Multiplexer for OpenFrameworks 
 Copyright (c) 2010 Stefan Schlupek <info@schlupek.com>
 Based on the TUIO Demo by Martin Kaltenbrunner:
 
 Copyright (c) 2005-2009 Martin Kaltenbrunner <mkalten@iua.upf.edu>

 and the CCV Code from the NUI Group 
 Copyright 2009 NUI Group. All rights reserved.
 
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

 #ifndef _TUIO_MULTIPLEXER
#define _TUIO_MULTIPLEXER

 #include "ofMain.h"
#include "TuioListener.h"
#include "TuioClient.h"
#include "TuioServer.h"
#include "TuioObject.h"
#include "TuioCursor.h"
#include "TuioPoint.h"
#include "TuioBlob.h"

//#include "TuioFilterCursor.h"
//#include "TuioFilterObject.h"

#include "ofxXmlSettings.h"
#include "ofxVectorMath.h"

//#include "ofxTuioClient.h"
#include "ofxNetwork.h"
#include "rect2d.h"

#include "Tracking.h"
#include "ofxCvGrayscaleImage.h"
#include "ContourFinder.h"
#include "TUIOSender.h"
#include "FBORenderer.h"


#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
	#define new DEBUG_NEW 


using namespace TUIO;
//class TuioFilterCursor ;


struct Screen {
		public:
		float		width;
		float		height;
		};

struct Tuio_Source {
		public:
	int				source_id;
	int				nr;
	string		source_name;
	string		source_addr;
	//int				priority;
	ofxVec3f		rotation;
	struct Screen	screen;//ref Screen
	struct Screen	camera_screen;//realscreen,with resize from the calibration boundingbox from CCV
	ofxVec3f translation;
	ofxVec3f camera_translation;
	//ofxVec3f translationNormalized;
	//std::list<int> siblings;
	//std::list<TuioFilterCursor*> cursorList;
	//std::list<TuioObject*> objectList;
	ofColor			color;
	//this Data comes from CCV
	bool			calibrate;
	int				GRID_X;
	int				GRID_Y;
	rect2df			screenBB;
	int				selectedCalibrationPoint;

	};



//public TuioListener,TuioClient,ofxTuioClient
class ofxTuioMultiplexer :  public TuioClient,public TuioListener {

	
	
	
	
public:
	ofxTuioMultiplexer();

	ofxTuioMultiplexer(string serverHost, int serverPort,int clientPort, int flashPort, int netinterfaceID, string tuio_application_name);
	
	~ofxTuioMultiplexer() ;


	void init();
	//void initTuioSenderConnection( );
	void initImage();
	//void drawIntoBlobImage(std::vector<Blob> &vtcur);
	void clearBlobImage();
	void convertToBlobVector(std::map<int, Blob> * blobs, std::vector<Blob> &v);
	void convertTUIOCursorsToBlobVector(std::vector<Blob> &v);
	void getTransformedTUIOObjects(std::list<TuioObject> &transformed_tobj_List);
	// variables
	
	//TuioServer * tuioServer;
	ofxXmlSettings		XML;

	//offset for drawing
	ofPoint offset;
	//--------UDP------------------
	//calibration Connection
	ofxUDPManager		udpConnection;
	int					calibration_udp_port; //port to send the config messages 
	//alive Connection (to send a alive message to the outside world)
	ofxUDPManager		udpConnectionAlive_IN;
	ofxUDPManager		udpConnectionAlive_OUT;
	int					alive_udp_in_port;
	string				alive_udp_out_host;  
	int					alive_udp_out_port;
	
	
	

	void readUDPConnection();
	void readUDPAlive_IN_Connection();//<command id=“getstatus“ ></command>
	void init_UDP_calibrate_connection();//call this after loadXMLSettings !
	void init_UDP_alive_connection();//call this after loadXMLSettings !
	//<command id=“status“ wert=“alive“ ip=“[eigene IP]“ ></command>

	bool isCalibrateConnected ;
	bool isAlive_IN_Connected;
	bool isAlive_OUT_Connected;
	//------------------UDP--------

	string screenMode;
	// for the circle mode
	float  PIXEL_TO_MM;
	float  MM_TO_PIXEL;
	//for the square mode
	float PIXEL_TO_MM_X;
	float PIXEL_TO_MM_Y;
	float  MM_TO_PIXEL_X;
	float  MM_TO_PIXEL_Y;
	float		fbo_scale;//scale factor for the FBO . Higher = better performance/ lower precision
	float		blobSize;// size for the internal Blobs in the FBO Renderer.

	//float distanceThreshold;// threshold for the finger-distance check
	//---------------------------

	//tracking
	//Blob Tracker
	BlobTracker			tracker;
	ofImage				testImage;
	ofxCvColorImage		blobImage;
	ofxCvGrayscaleImage blackImage;
	ofxCvGrayscaleImage blobImageBw;
	//ofxCvColorImage temp;
	
	//Blob Finder
	ContourFinder	contourFinder;

	unsigned char * blackPixels;
	//ofTexture texGray;

	//FBO
	//GLuint output_buffer;//FBO
	FBORenderer	fboRenderer;

	//---------

	//Communication
	TUIOSender			tuioSender;
	string				tmpLocalHost;
    int					tmpPort;
	
	//---------------------------

	struct Screen	main_screen;
	struct Screen	cameraScreen;//area in pixel
	struct Screen	main_screen_scaled;// scaled for FBO

	ofTrueTypeFont	verdana;
	ofTrueTypeFont	verdana2;

	//std::vector< Blob> trackedBlobs;
	std::list< struct Tuio_Source*> tuioSources;
	//std::list< TuioFilterCursor*> filterCursors;

	//std::list<TuioFilterCursor*> tuioFilterCursors;
	std:: map<int,Tuio_Source*> nrToSource_map;// zuordnung der sourceNr zu tuio_source struct

	std:: map<TuioCursor*,Tuio_Source*> tuioCourser_source_map;// damit kann man schnell die Source eines Fingers zuordnen	
	/*
	std:: map<TuioCursor*,Tuio_Source*> server_courser_source_map;// zuordnung der TuioCursor des Servers zu tuioFilterCursor
	std:: map<TuioCursor*,TuioFilterCursor*> courserToFilterCursor_map;// zuordnung der TuioCursor zu tuioFilterCursor
	std:: map<TuioCursor*,TuioFilterCursor*> server_courserToFilterCursor_map;// zuordnung der TuioCursor des Servers zu tuioFilterCursor
	std:: map<TuioFilterCursor*,TuioCursor*> filterCursorToServer_courser_map;//
	*/

	//TUIOObjects
	std:: map<TuioObject*,Tuio_Source*> tuioObject_source_map;// damit kann man schnell die Source eines Objectes zuordnen
	std::list<TuioObject> transformed_tobj_List ;// zUm Versenden mit der entsprechenden Transformation der Source
	//std:: map<int,std::list< TuioObject*> > tuioObjectID_Objects_map;
	/*
	std:: map<TuioObject*,TuioFilterObject*> objectToFilterObject_map;// zuordnung der TuioObjects zu tuioFilterObject
	std:: map<TuioObject*,TuioFilterObject*> server_objectToFilterObject_map;// zuordnung der TuioObjekte des Servers zu tuioFilterObjekt
	std:: map<TuioFilterObject*,TuioObject*> filterObjectToServer_object_map;//
	*/
	//


	//debug Methods
	void drawCursors();
	void drawCursor(TuioCursor * tcur);
	void drawCursor(ofxVec3f * pos);
	void drawObjects();
	void drawSourceScreens();
	void drawDebugInfo();
	//
	//to get a vctor who has the position in World coordintates
	//void localToGlobal(ofxVec3f * vec,TuioCursor * tcur);
	void localToGlobal(ofxVec3f * vec,TuioContainer * TuioContainer);
	void localToGlobal(ofxVec3f * vec,TuioContainer * tcur,Tuio_Source * source);

	
	//update method to trigger the TuioServer
	void update();

	void init_TUIO_connection();//after XMLloading we could initialize the TUIO Connenctions
	//TUIOListener Interface
	void addTuioObject(TuioObject * tobj);
	void updateTuioObject(TuioObject * tobj);
	void removeTuioObject(TuioObject * tobj);
	
	void addTuioCursor(TuioCursor * tcur);
	void updateTuioCursor(TuioCursor * tcur);
	void removeTuioCursor(TuioCursor * tcur);

	
	//void addTuioCursor2(TuioCursor * tcur);

	//TUIO 1.1 Spec---
	void addTuioBlob(TuioBlob *tblb);
	void updateTuioBlob(TuioBlob *tblb);
	void removeTuioBlob(TuioBlob *tblb);
	//-----------
	
	void refresh(TuioTime frameTime);
	//TUIOListener Interface

	void loadXMLSettings(string path);
	void initXMLSettings();
	void setXML(ofxXmlSettings *xml);

	
	//OF POCO EVENTS TO FORWARD TUIO EVENTS TO testApp or any other Class
	ofEvent<TuioCursor> cursorRemoved;
	ofEvent<TuioCursor> cursorAdded;
	ofEvent<TuioCursor> cursorUpdated;
	
	ofEvent<TuioObject> objectAdded;
	ofEvent<TuioObject> objectRemoved;
	ofEvent<TuioObject> objectUpdated;

	ofEvent<TuioBlob> blobAdded;
	ofEvent<TuioBlob> blobRemoved;
	ofEvent<TuioBlob> blobUpdated;

private:
	TuioTime currentTime;//Namespace TUIO

};

#endif