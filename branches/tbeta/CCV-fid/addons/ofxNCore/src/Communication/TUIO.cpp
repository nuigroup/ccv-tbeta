/*
*  TUIO.h
*  
*
*  Created on 2/2/09.
*  Copyright 2009 NUI Group. All rights reserved.
*
*/

#include "TUIO.h"
#include "Poco/Environment.h"
#include "Poco/Net/NetworkInterface.h"
using Poco::Environment;
using namespace Poco::Net;


TUIO::TUIO() {
	
	 host_name = new char[64];
	 tuio_source_String = new char[256];

	 setNetworkInterface(1);//default
	
	// cout << "NetworkInterface.address:" << NetworkInterface::list()[0].address().toString() << endl;
	//NetworkInterface::list().size()[1].address()!= Null
	// int index = 0;
	//if(NetworkInterface::list().size() >1 ){ index = 1;}
	
	 //strcpy(host_name,NetworkInterface::list()[index].address().toString().c_str( ));//char * strcpy ( char * destination, const char * source );
	 
	// host_name =  NetworkInterface::list()[1].address().toString().c_str( );
	 /*
	 gethostname(host_name,64);
	 struct hostent *phe = gethostbyname(host_name);
	 //struct in_addr addr;
	 memcpy(&addr, phe->h_addr_list[0], sizeof(struct in_addr));
	cout << "IPAddress " << inet_ntoa(addr) << endl;
	*/
}

TUIO::~TUIO() {

	if (tuio_source_String) delete[] tuio_source_String;
	if (host_name) delete[] host_name;
						
	// this could be useful for whenever we get rid of an object
}

void TUIO::setup(const char* host, int port, int flashport,int _camWidth,int _camHeight) {

	localHost = host;
	TUIOPort = port;
	TUIOFlashPort = flashport;
	frameseq = 0;

	//FOR TCP
	bIsConnected = m_tcpServer.setup(TUIOFlashPort);
	//FOR OSC
	TUIOSocket.setup(localHost, TUIOPort);

	camWidth= _camWidth;
	camHeight= _camHeight;
	 
	 
}
void TUIO::setFiducialModus(bool status){
		//----added by Stefan Schlupek
	bTrackFiducials = status;
}

void TUIO::setNetworkInterface(int id){
	 //copy the IPadress from POCO.Net.NetworkInterface to a char*
	int index = 0;

	if(NetworkInterface::list().size() > id ){ 
		index = id;
	}else if(NetworkInterface::list().size() >1 ){
		index = 1;
	}
	
	 strcpy(host_name,NetworkInterface::list()[index].address().toString().c_str( ));
}

void TUIO::setup_tuio_source_String(){
	//cout << "setup_tuio_source_String " << inet_ntoa(addr) << endl;
	// source message from TUIO 1.1 /tuio/[profileName] source [application@address]
	//NetworkInterface::list()[1].address().toString() ;// to get the IP-Adress we could use POCO
	 sprintf(tuio_source_String,"%s@%s",tuio_source_application, host_name);
	
	// sprintf(tuio_source_String,"%s@%s",tuio_source_application,inet_ntoa(addr));
	 cout << "TUIO.SOURCE: "<< tuio_source_String << endl ;
}

void TUIO::sendTUIO(std::map<int, Blob> * blobs, std::list <ofxFiducial> * fiducialsList)
{
	frameseq += 1;

	//if sending OSC (not TCP)
	if(bOSCMode){

		ofxOscBundle b;

		//added by Stefan Schlupek
		// source message from TUIO 1.1 /tuio/[profileName] source [application@address]

		ofxOscMessage source;
			source.setAddress( "/tuio/2Dcur" );
			source.addStringArg( "source" );	
			source.addStringArg(tuio_source_String);

		ofxOscMessage alive;
			alive.setAddress("/tuio/2Dcur");
			alive.addStringArg("alive");


		ofxOscMessage fseq;
			fseq.setAddress( "/tuio/2Dcur" );
			fseq.addStringArg( "fseq" );
			fseq.addIntArg(frameseq);

		if(blobs->size() == 0 )
		{
			//Sends alive message - saying 'Hey, there's no alive blobs'

			//Send fseq message
			

			b.addMessage( source ); //add message to bundle	
			b.addMessage( alive ); //add message to bundle
			b.addMessage( fseq ); //add message to bundle
			//TUIOSocket.sendBundle( b ); //send bundle
		}
		else //actually send the blobs
		{
				b.addMessage( source ); //add message to bundle			
				
				
				//Send alive message of all alive IDs

				std::map<int, Blob>::iterator this_blobID;
				for(this_blobID = blobs->begin(); this_blobID != blobs->end(); this_blobID++)
				{
					alive.addIntArg(this_blobID->second.id); //Get list of ALL active IDs
				}

				b.addMessage( alive ); //add message to bundle


				map<int, Blob>::iterator this_blob;
				for(this_blob = blobs->begin(); this_blob != blobs->end(); this_blob++)
				{
					//Set Message /tuio/2Dcur set s x y X Y m

					ofxOscMessage set;
					set.setAddress("/tuio/2Dcur");
					set.addStringArg("set");
					set.addIntArg(this_blob->second.id); //id
					
					set.addFloatArg(this_blob->second.centroid.x);  // x								
					set.addFloatArg(this_blob->second.centroid.y); // y
					
					set.addFloatArg(this_blob->second.D.x); //dX
					set.addFloatArg(this_blob->second.D.y); //dY
					set.addFloatArg(this_blob->second.maccel); //m
					
					
					/*
					if(bHeightWidth){
						set.addFloatArg(this_blob->second.boundingRect.width); // wd
						set.addFloatArg(this_blob->second.boundingRect.height);// ht
					}
					*/
					b.addMessage( set ); //add message to bundle			

				}//for


			
				b.addMessage( fseq ); //add message to bundle		

			}//if


			TUIOSocket.sendBundle( b ); //send bundle
// BLOB-END----------------

//Fiducial: //----added by Stefan Schlupek
	if(bTrackFiducials){
			ofxOscBundle b_obj;

			ofxOscMessage source_obj;
				source_obj.setAddress( "/tuio/2Dobj" );
				source_obj.addStringArg( "source" );
				source_obj.addStringArg(tuio_source_String);

			ofxOscMessage alive_obj;
				alive_obj.setAddress("/tuio/2Dobj");
				alive_obj.addStringArg("alive");

			ofxOscMessage fseq_obj;
				fseq_obj.setAddress( "/tuio/2Dobj" );
				fseq_obj.addStringArg( "fseq" );
				fseq_obj.addIntArg(frameseq);

			if( fiducialsList->size() == 0){

				//Sends alive message - saying 'Hey, there's no alive blobs'
				b_obj.addMessage( source_obj ); //add message to bundle	
				b_obj.addMessage( alive_obj ); //add message to bundle
				b_obj.addMessage( fseq_obj ); //add message to bundle
			
			}else		
				{				

				//printf("FIDUCIALS->SIZE: %i\n", fiducialsList->size() );
					b_obj.addMessage( source_obj ); //add message to bundle	
				
				list <ofxFiducial>::iterator this_fiducial ;
				for(this_fiducial = fiducialsList->begin();this_fiducial != fiducialsList->end(); this_fiducial++){
					//printf("FIDUCIAL: %f\n", this_fiducial. );
					//Set Message /tuio/2Dobj set s i x y a X Y A m r
									
					ofxOscMessage set;
					set.setAddress("/tuio/2Dobj");
					set.addStringArg("set");
					set.addIntArg(this_fiducial->getId()); //s sessionID????
					set.addIntArg(this_fiducial->getId()); //i	
					set.addFloatArg(this_fiducial->getCameraToScreenX());  // x Normalized
					set.addFloatArg(this_fiducial->getCameraToScreenY()); // y Normalized
					set.addFloatArg(this_fiducial->getAngle()); //a
					set.addFloatArg(this_fiducial->getMSpeedX()); //X
					set.addFloatArg(this_fiducial->getMSpeedY()); //Y
					set.addFloatArg(this_fiducial->getRSpeed()); //A
					set.addFloatArg(this_fiducial->getMAccel()); //m
					set.addFloatArg(this_fiducial->getRAccel()); //r
					

					b_obj.addMessage( set ); //add message to bundle
					//Send alive message of all alive IDs
					alive_obj.addIntArg(this_fiducial->getId());
				}//for

				b_obj.addMessage( alive_obj ); //add message to bundle
				b_obj.addMessage( fseq_obj ); //add message to bundle

				
			}//if				

		TUIOSocket.sendBundle( b_obj ); //send bundle
	}//end if(bTrackFiducials
//--------Fiducial END--------------------------------

//---------End OSC Mode-------------------------------


	}else if(bTCPMode) //else, if TCP (flash) mode
	{
		if(blobs->size() == 0){

			m_tcpServer.sendToAll("<OSCPACKET ADDRESS=\"127.0.0.1\" PORT=\""+ofToString(TUIOPort)+"\" TIME=\""+ofToString(ofGetElapsedTimef())+"\">" +
							 "<MESSAGE NAME=\"/tuio/2Dcur\">"+
							 "<ARGUMENT TYPE=\"s\" VALUE=\"alive\"/>"+
							 "</MESSAGE>"+
							 "<MESSAGE NAME=\"/tuio/2Dcur\">"+
							 "<ARGUMENT TYPE=\"s\" VALUE=\"fseq\"/>"+
							 "<ARGUMENT TYPE=\"i\" VALUE=\""+ofToString(frameseq)+"\"/>" +
							 "</MESSAGE>"+
							 "</OSCPACKET>");
		}
		else
		{
			string setBlobsMsg;

			map<int, Blob>::iterator this_blob;
			for(this_blob = blobs->begin(); this_blob != blobs->end(); this_blob++)
			{
				//if sending height and width
				if(bHeightWidth){
					setBlobsMsg += "<MESSAGE NAME=\"/tuio/2Dcur\"><ARGUMENT TYPE=\"s\" VALUE=\"set\"/><ARGUMENT TYPE=\"i\" VALUE=\""+ofToString(this_blob->second.id)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(this_blob->second.centroid.x)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(this_blob->second.centroid.y)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(this_blob->second.D.x)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(this_blob->second.D.y)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(this_blob->second.maccel)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(this_blob->second.boundingRect.width)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(this_blob->second.boundingRect.height)+"\"/>"+
					"</MESSAGE>";
				}
				else{
					setBlobsMsg += "<MESSAGE NAME=\"/tuio/2Dcur\"><ARGUMENT TYPE=\"s\" VALUE=\"set\"/><ARGUMENT TYPE=\"i\" VALUE=\""+ofToString(this_blob->second.id)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(this_blob->second.centroid.x)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(this_blob->second.centroid.y)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(this_blob->second.D.x)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(this_blob->second.D.y)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(this_blob->second.maccel)+"\"/>"+
					"</MESSAGE>";
				}
			}

			string aliveBeginMsg = "<MESSAGE NAME=\"/tuio/2Dcur\"><ARGUMENT TYPE=\"s\" VALUE=\"alive\"/>";

			string aliveBlobsMsg;
			std::map<int, Blob>::iterator this_blobID;
			for(this_blobID = blobs->begin(); this_blobID != blobs->end(); this_blobID++)
			{
				aliveBlobsMsg += "<ARGUMENT TYPE=\"i\" VALUE=\""+ofToString(this_blobID->second.id)+"\"/>";
			}

			string aliveEndMsg = "</MESSAGE>";

			string fseq = "<MESSAGE NAME=\"/tuio/2Dcur\"><ARGUMENT TYPE=\"s\" VALUE=\"fseq\"/><ARGUMENT TYPE=\"i\" VALUE=\""+ofToString(frameseq)+"\"/></MESSAGE>";

			m_tcpServer.sendToAll("<OSCPACKET ADDRESS=\"127.0.0.1\" PORT=\""+ofToString(TUIOPort)+"\" TIME=\""+ofToString(ofGetElapsedTimef())+"\">" +
									setBlobsMsg + aliveBeginMsg + aliveBlobsMsg + aliveEndMsg + fseq + "</OSCPACKET>");
		}
	}
}
