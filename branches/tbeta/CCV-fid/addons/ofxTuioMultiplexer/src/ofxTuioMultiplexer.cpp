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

#include "ofxTuioMultiplexer.h"

#include "ofxTuioMultiplexer_draw.h" 
#include "ofxTuioMultiplexer_loadXML.h" 
#include "ofxTuioMultiplexer_udp_connection.h" 


ofxTuioMultiplexer::ofxTuioMultiplexer() {  
	//here we connect the client per default with port 3333
	 init();
}

//call contstrucor from SuperClass 
ofxTuioMultiplexer::ofxTuioMultiplexer(
	string serverHost, 
	int serverPort,
	int clientPort, 
	int flashPort, 
	int netinterfaceID, 
	string tuio_application_name) : TuioClient(clientPort) {

	cout << "clientPort::" << clientPort << endl;
	cout << "serverPort::" << serverPort << endl; 

	init();
	//tuioSender = new TUIOSender();
	tuioSender.setup(serverHost.c_str(), serverPort, flashPort, netinterfaceID, tuio_application_name);

   this->addTuioListener(this);
 

  // tuioServer = new TuioServer(serverHost.c_str(), serverPort);//3334
	//initTuioConnection(serverHost.c_str(), serverPort,flasPort,tuio_application_name,);
}
 ofxTuioMultiplexer::~ofxTuioMultiplexer() {
		disconnect();
		//delete tuioSender;
	}


void ofxTuioMultiplexer::init(){
	 isCalibrateConnected= false;
	 isAlive_IN_Connected = false;
	 isAlive_OUT_Connected = false;
// now load another font, but with extended parameters:
	// font name, size, anti-aliased, full character set
	verdana.loadFont("verdana.ttf",24, false, true);
	verdana.setLineHeight(20.0f);

	verdana2.loadFont("verdana.ttf",11, false, true);
	verdana2.setLineHeight(10.0f);

	
}



void ofxTuioMultiplexer::initImage(){
	
	main_screen_scaled.width = (int)floor(main_screen.width/fbo_scale);
	main_screen_scaled.height = (int)floor( main_screen.height/fbo_scale);

	blobImage.allocate(main_screen_scaled.width, main_screen_scaled.height); //main Image that'll be processed.
	blackImage.allocate(main_screen_scaled.width, main_screen_scaled.height);
	blobImageBw.allocate(main_screen_scaled.width, main_screen_scaled.height);
	testImage.allocate(main_screen_scaled.width, main_screen_scaled.height,OF_IMAGE_GRAYSCALE);
	
	 blackPixels			= new unsigned char [main_screen_scaled.width* main_screen_scaled.height];

	// black pixels
	for (int i = 0; i < main_screen_scaled.width*main_screen_scaled.height; i++){
		blackPixels[i] = (unsigned char)(0);
		
	}
	
	blackImage.setFromPixels(blackPixels,main_screen_scaled.width, main_screen_scaled.height);
	//blobImage.setUseTexture(false);	
	cout << "initImage::" <<  endl;

	fboRenderer.initFBO( main_screen_scaled.width, main_screen_scaled.height,blobSize,fbo_scale);
}



void ofxTuioMultiplexer::addTuioCursor(TuioCursor * tcur){

	lockCursorList();
		//cout << "TuioSourceFilter::addTuioCursor:" << tcur->getTuioSourceName()<< "@" << tcur->getTuioSourceAddress()<< endl;
		bool trueSource = false;

		//cursor ID = name@adress
		char cursor_source_buffer [256];
		sprintf(cursor_source_buffer,"%s@%s",tcur->getTuioSourceName(), tcur->getTuioSourceAddress());

		Tuio_Source *tsrc;// current Source;

		char tuio_source_buffer [256];

		std::list<struct Tuio_Source*>::iterator tsrc_it;
		
		//looking for a tuioSource that has the same ID name@adress
		for (tsrc_it=tuioSources.begin(); tsrc_it != tuioSources.end(); tsrc_it++) {
			tsrc = (*tsrc_it);
			//source ID = name@adress	
			//tuio_source_buffer = "";
			memset(tuio_source_buffer, 0, sizeof(tuio_source_buffer));//clear 
			sprintf(tuio_source_buffer,"%s@%s",tsrc->source_name.c_str( ), tsrc->source_addr.c_str( ));// Strings to const char* convert ! 
	
			//std::cout << "Buffer:" << tuio_source_buffer << " cursorbuffer: "<< cursor_source_buffer  << std::endl;
			// now we have the right source for the Cursor
			if(strcmp (cursor_source_buffer,tuio_source_buffer) == 0 ){
				trueSource = true;
				/*Problem: die TuioSourceID im TUIClient werden anders vergeben als die source_id im Tuio_source struct.
				also beim ersten neuen FingerAdd muss die zuweisung kommen!
				*/
				if(tsrc->source_id==INT_MAX){
					tsrc->source_id = tcur->getTuioSourceID();

					//tuioCourser_source_map[tcur] = tsrc;
					nrToSource_map[tsrc->nr] = tsrc;
					cout << "-----------NEW SOURCE---------" << endl;					
				}

				break;
				
			}// if
		}//for

		if(trueSource!= false){
				tuioCourser_source_map[tcur] = tsrc;
				//nrToSource_map[tsrc->nr] = tsrc;
				//cout << "1-----------NEW 2---------"  << endl;		
		}else{
			cout<< "SOURCE ADRESS IS WRONG!" << cursor_source_buffer<<endl;
		}
	
	unlockCursorList();
	//_CrtDumpMemoryLeaks();
}


	void ofxTuioMultiplexer::updateTuioCursor(TuioCursor * tcur){
		
		lockCursorList();	
		cout << "TuioSourceFilter::updateTuioCursor:" << tcur->getX()<< ":" << tcur->getY()<< ":"<< tcur->getXSpeed()<<":" <<tcur->getYSpeed()<< ":"<< tcur->getMotionAccel()<<endl;
		unlockCursorList();
		
	//_CrtDumpMemoryLeaks();
	};
	

	void ofxTuioMultiplexer::removeTuioCursor(TuioCursor * tcur){
		
		lockCursorList();	
		tuioCourser_source_map.erase(tcur);
		unlockCursorList();
		
	//_CrtDumpMemoryLeaks();
	};



	//TUIOObject-------------------
	void ofxTuioMultiplexer::addTuioObject(TuioObject * tobj){
		lockObjectList();

		//cout << "TuioSourceFilter::addTuioObject" << tobj << endl;

		cout << "TuioSourceFilter::addTuioObject:" << tobj->getTuioSourceName()<< "@" << tobj->getTuioSourceAddress()<<":" << tobj->getX()<<"."<< tobj->getY()<<endl;

		bool trueSource = false;

		//cursor ID = name@adress
		char object_source_buffer [256];
		sprintf(object_source_buffer,"%s@%s",tobj->getTuioSourceName(), tobj->getTuioSourceAddress());

		Tuio_Source *tsrc;// current Source;

		char tuio_source_buffer [256];

		std::list<struct Tuio_Source*>::iterator tsrc_it;
		
		//looking for a tuioSource that has the same ID name@adress
		for (tsrc_it=tuioSources.begin(); tsrc_it != tuioSources.end(); tsrc_it++) {
			tsrc = (*tsrc_it);
			//source ID = name@adress	
			//tuio_source_buffer = "";
			memset(tuio_source_buffer, 0, sizeof(tuio_source_buffer));//clear 
			sprintf(tuio_source_buffer,"%s@%s",tsrc->source_name.c_str( ), tsrc->source_addr.c_str( ));// Strings to const char* convert ! 
	
			//std::cout << "Buffer:" << tuio_source_buffer << " cursorbuffer: "<< cursor_source_buffer  << std::endl;
			// now we have the right source for the Cursor
			if(strcmp (object_source_buffer,tuio_source_buffer) == 0 ){
				trueSource = true;
				/*Problem: die TuioSourceID im TUIClient werden anders vergeben als die source_id im Tuio_source struct.
				also beim ersten neuen FingerAdd muss die zuweisung kommen!
				*/
				if(tsrc->source_id==INT_MAX){
					tsrc->source_id = tobj->getTuioSourceID();
					
					nrToSource_map[tsrc->nr] = tsrc;
					cout << "-----------NEW SOURCE---------" << endl;					
				}

				break;
				
			}// if
		}//for

		if(trueSource!= false){
				tuioObject_source_map[tobj] = tsrc;
				//nrToSource_map[tsrc->nr] = tsrc;
				//cout << "1-----------NEW 2---------"  << endl;

		}else{
			cout<< "SOURCE ADRESS IS WRONG!" << endl;
		}

	unlockObjectList();
	};


	void ofxTuioMultiplexer::updateTuioObject(TuioObject * tobj){
		
		lockObjectList();
		cout << "TuioSourceFilter::updateTuioObject:" << tobj <<":"<< tobj->getX()<<"."<< tobj->getY()<< endl;//->getTuioSourceAddress()
		printf("TuioSourceFilter::updateTuioObject" );
		unlockObjectList();
		
	};


void ofxTuioMultiplexer::removeTuioObject(TuioObject * tobj){
	
	lockObjectList();
	//cout << "TuioSourceFilter::removeTuioObject:" << tobj->getTuioSourceName()<< "@" << tobj->getTuioSourceAddress()<< endl;		
	tuioObject_source_map.erase(tobj);
	unlockObjectList();
	
	};
	
	//------------------------


	//TUIO 1.1 Spec---
	void ofxTuioMultiplexer::addTuioBlob(TuioBlob *tblb){};
	void ofxTuioMultiplexer::updateTuioBlob(TuioBlob *tblb){};
	void ofxTuioMultiplexer::removeTuioBlob(TuioBlob *tblb){};
	
	void ofxTuioMultiplexer::refresh(TuioTime frameTime){};
	//-----------------------




	void ofxTuioMultiplexer::localToGlobal(ofxVec3f * vec,TuioContainer * tcur,Tuio_Source * source){

		vec->x = (tcur->getX()*source->screen.width)-(source->screen.width/2.0);
		vec->y = (tcur->getY()*source->screen.height)-(source->screen.height/2.0);
		vec->rotate( source->rotation.z,  ofxVec3f(0,0,1) );

		vec->x += source->translation.x;
		vec->y += source->translation.y;
	}

	void ofxTuioMultiplexer::localToGlobal(ofxVec3f * vec,TuioContainer * tcur){

			std::list<struct Tuio_Source*>::iterator tsrc;
			//looking for a tuioSource that has the same ID name@adress
			for (tsrc=tuioSources.begin(); tsrc != tuioSources.end(); tsrc++) {
				Tuio_Source *obj = (*tsrc);
				if(obj->source_id == tcur->getTuioSourceID()){

					vec->x = (tcur->getX()*obj->screen.width)-(obj->screen.width/2.0);
					vec->y = (tcur->getY()*obj->screen.height)-(obj->screen.height/2.0);
					vec->rotate( obj->rotation.z,  ofxVec3f(0,0,1) );

					vec->x += obj->translation.x;
					vec->y += obj->translation.y;
					break;
				}
			
			}


	};


	
	void ofxTuioMultiplexer::update(){
	
			//call this only if we have triggert the connection via init_UDP_calibrate_connection();
			if(isCalibrateConnected){
				readUDPConnection();
			}
			//only check UDP Connection if we have in and out connection to send an alive message
			if(isAlive_IN_Connected && isAlive_OUT_Connected){
				readUDPAlive_IN_Connection();
			}
			

		//1. create a Image from the current Fingers
		//2. contourfinder 
		//3. take the blobs from the contourfinder to the Blobtracker
		//4. Send TUIO Messages

			
			clearBlobImage();

			//fboRenderer.grabImageData(temp.getPixels());
			std::vector<Blob> vtcur;
			convertTUIOCursorsToBlobVector(vtcur);//std::list<TuioCursor*> curList = getTuioCursors(),std::vector<Blob> v;

			//FBO version
			fboRenderer.drawBlobs(blobImage,vtcur);
			blobImageBw = blobImage;//convert to grayscale for contourfinder;

			//pure FrameBuffer version //bug with scaling!
			//fboRenderer.drawBlobs(testImage,vtcur);
			//blobImageBw.setFromPixels(testImage.getPixels(), main_screen_scaled.width, main_screen_scaled.height);
			
			//drawIntoBlobImage(vtcur);		

		
			int MIN_BLOB_SIZE = 0;
			int MAX_BLOB_SIZE = 100;
			int maxBlobs = 256;
		
			
			int nBlobs = contourFinder.findContours(blobImageBw,  (MIN_BLOB_SIZE * 2) + 1, ((main_screen_scaled.width* main_screen_scaled.height) * .4) * (MAX_BLOB_SIZE * .001), maxBlobs, false);

			//Track found contours/blobs
			//cout <<"contourFinder BLOBS:"<<contourFinder.blobs.size()<<":"<<nBlobs<<endl;
			for  (vector <Blob>	::iterator it = contourFinder.blobs.begin();it !=  contourFinder.blobs.end();it++){
			
			cout <<"contourFinder BLOB:"<<(*it).centroid.x<<":"<<(*it).centroid.y<<endl;
			}
			
			tracker.track(&contourFinder);
			//cout <<"TRACKER BLOBS:"<<&tracker.getTrackedBlobs()<<endl;
			std::vector<Blob> vBlob;
			convertToBlobVector(&(tracker.getTrackedBlobs()),vBlob);
			


			transformed_tobj_List.clear(); //the list is filled in the next step  ;
			getTransformedTUIOObjects(transformed_tobj_List);
			
			tuioSender.sendTUIO(vBlob,transformed_tobj_List);//tracker.getTrackedBlobs(), &fidfinder.fiducialsList
			
			//_CrtDumpMemoryLeaks();
			//vtcur.clear();
			//vBlob.clear();
	}






	void ofxTuioMultiplexer::clearBlobImage(){
		// cout << "clearBlobImage::1" <<  endl;	
		blobImage = blackImage;
		//temp = blackImage;

	}

	// Here we convert the datastructure from countourtracker to a vector for easy parsing and Normalizing the position.
	void ofxTuioMultiplexer::convertToBlobVector(std::map<int, Blob> * blobs, std::vector<Blob> &v){
		//cout<<"convertToBlobVector:"<<blobs <<endl;
				map<int, Blob>::iterator this_blob;
				for(this_blob = blobs->begin(); this_blob != blobs->end(); this_blob++)
				{
					//cout<<"BLOB_:"<< this_blob<<endl;
					//cout<<"BLOB_:"<< this_blob->second.centroid.x <<endl;
					v.push_back (this_blob->second);
					
				}
				std::vector<Blob>::iterator it;

				for(it= v.begin();it != v.end();it++){
				
					
					//cout<<"BLOB:"<< (*it).centroid.x <<endl;
					(*it).centroid.x /= main_screen_scaled.width;
					(*it).centroid.y /= main_screen_scaled.height;
					
					//(*it).centroid = ofPoint(b.centroid.x /= main_screen.width,b.centroid.y /= main_screen.height);
					//cout<<"BLOB_norm:"<< (*it).centroid.x <<endl;
				}
				
	}


	//Here we convert the current TUIOCursor List to a vector and project the position in World coordinates (Normalized)
	//Cursor who  have not a source-adress from the config.xml are skipped
	void ofxTuioMultiplexer::convertTUIOCursorsToBlobVector(std::vector<Blob> &v){
		lockCursorList();
		std::list<TuioCursor*> curList = getTuioCursors();
		//cout<<"--------------------------" <<endl;
		for(std::list<TuioCursor*>::iterator it = curList.begin();it != curList.end();it++)
    {
		TuioCursor *tcur = (*it);
		//cout<<"*tcur:"<< (*it)->getX() <<":"<< (*it)->->getY() <<endl;
		ofxVec3f pos_tcur;
		Tuio_Source* tsrc = tuioCourser_source_map[tcur];
		//cout<<"tsrc:---" << tsrc->source_name <<endl;
		if(tsrc != NULL){
			localToGlobal(&pos_tcur,tcur,tsrc);
			Blob b;
			b.centroid.x = pos_tcur.x/main_screen_scaled.width;
			b.centroid.y = pos_tcur.y/main_screen_scaled.height;
			v.push_back(b);
			}
		}
	//	cout<<"--------------------------" <<endl;
		unlockCursorList();
	
	}

	void ofxTuioMultiplexer::getTransformedTUIOObjects(std::list<TuioObject> &transformed_tobj_List){
		lockObjectList();
		std::list<TuioObject*> tobjList	= getTuioObjects() ;

		//1. Durch alle Objekte Parsen
		//2. Es gibt nieh zwei gleiche 
		//3.
		for(std::list<TuioObject*>::iterator it = tobjList.begin();it != tobjList.end();it++)
			{

			TuioObject *tobj = (*it);
			ofxVec3f pos_tobj;
			Tuio_Source* tsrc = tuioObject_source_map[tobj];
			if(tsrc ==NULL)continue;//nasty Hack but there are some cases were the map is empty at the key
			localToGlobal(&pos_tobj,tobj,tsrc);

			//TuioObject tobj2 = (*it);
			//TuioObject t_wc_obj = tobj;
			
			transformed_tobj_List.push_back(tobj);
			//int index = visible_tobjList.size()-1;
			//index = index < 0 ? 0 : index ;
			TuioObject  t_wc_obj = transformed_tobj_List.back();

			
		/*
		 * @param	ttime	the TuioTime to assign
		 * @param	xp	the X coordinate to assign
		 * @param	yp	the Y coordinate to assign
		 * @param	a	the angle coordinate to assign
		 * @param	xs	the X velocity to assign
		 * @param	ys	the Y velocity to assign
		 * @param	rs	the rotation velocity to assign
		 * @param	ma	the motion acceleration to assign
		 * @param	ra	the rotation acceleration to assign
		 */
		//		 update (float xp, float yp, float a, float xs, float ys, float rs, float ma, float ra)
		
		float rot = (tuioObject_source_map[tobj]->rotation.z /180*PI)+tobj->getAngle();
		//serverTobj->update(TuioTime::getSessionTime(),pos_Normalized.x,pos_Normalized.y,rot,0,0,0,0,0);
		ofxVec3f pos_Normalized = ofxVec3f(pos_tobj.x/main_screen_scaled.width,pos_tobj.y/main_screen_scaled.height);//Normalize the Values
		
			
			transformed_tobj_List.back().update(TuioTime::getSessionTime(),pos_Normalized.x,pos_Normalized.y,rot,0,0,0,0,0);

		}


		//visible_tobjList;
		unlockObjectList();

	} //

	/*
	void ofxTuioMultiplexer::drawIntoBlobImage(std::vector<Blob> &vtcur){
	//	cout << "drawIntoBlobImage::1" <<  endl;
		
		unsigned char * pixels = blobImage.getPixels();

		for(std::vector<Blob>::iterator it = vtcur.begin();it != vtcur.end();it++)
	{
		//Here we draw the blobs 
		//ofCircle( (*it).centroid.x * width, height-((*it).centroid.y*height), blobSize/scaleFactor);
		float pos = floor( ((*it).centroid.x * main_screen_scaled.width)+ (main_screen_scaled.height-((*it).centroid.y*main_screen_scaled.height)) );
		pos = pos <= 0 ? 0 : pos;
		pos = pos > (main_screen_scaled.width* main_screen_scaled.height)-1 ? (main_screen_scaled.width* main_screen_scaled.height)-1 : pos;
		pixels[(int)(pos)]= 255;
		pixels[400 +1]= 255;
		pixels[400 +2]= 255;
		pixels[400+3]= 255;
		pixels[400 +4]= 255;
		cout << "drawIntoBlobImage::pos" << pos << ":" <<endl;
	}
		
		
		blobImageBw.setFromPixels(pixels,main_screen_scaled.width, main_screen_scaled.height);
		
		//cout << "drawIntoBlobImage::2" <<  endl;
}
*/
			