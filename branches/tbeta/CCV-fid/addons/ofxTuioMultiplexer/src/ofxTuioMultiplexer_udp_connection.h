#ifndef _TUIO_MULTIPLEXER_UDPCONNECTION
#define _TUIO_MULTIPLEXER_UDPCONNECTION


//connections------------------------
void ofxTuioMultiplexer::init_TUIO_connection(){

}
void ofxTuioMultiplexer::init_UDP_calibrate_connection(){
	//UDP Connection to receive the CCV Messages
	udpConnection.Create();
	isCalibrateConnected = udpConnection.Bind(calibration_udp_port);
	udpConnection.SetNonBlocking(true);
}

void ofxTuioMultiplexer::init_UDP_alive_connection(){
	//isAliveConnected = true;
	udpConnectionAlive_IN.Create();
	udpConnectionAlive_OUT.Create();
	isAlive_IN_Connected = udpConnectionAlive_IN.Bind(alive_udp_in_port);
	isAlive_OUT_Connected = udpConnectionAlive_OUT.Connect(alive_udp_out_host.c_str(),alive_udp_out_port);

	cout << "isAlive_IN_Connected: " << isAlive_IN_Connected << endl;
	cout << " isAlive_OUT_Connected: " << isAlive_OUT_Connected << endl;

	udpConnectionAlive_IN.SetNonBlocking(true);
	udpConnectionAlive_OUT.SetNonBlocking(true);

}
void ofxTuioMultiplexer::readUDPAlive_IN_Connection(){

	char udpMessage[256];
	udpConnectionAlive_IN.Receive(udpMessage,256);
	string message=udpMessage;
	
	if(message=="<command id=\"getstatus\" ></command>" ||  message=="<command id=\"getstatus\"></command>"){
		cout << "UDP_alive_in:"<<message <<endl;
		char udpOutMessage[256];
		sprintf(udpOutMessage,"<command id=\"status\" wert=\"alive\" ip=\"%s\" ></command>",tuioSender.host_name);// Strings to const char* convert ! 
		//<command id=“status“ wert=“alive“ ip=“[your IP]“ ></command>
		int sent = udpConnectionAlive_OUT.Send(udpOutMessage,sizeof(udpOutMessage));
	}

}
void ofxTuioMultiplexer::readUDPConnection(){

	char udpMessage[1024];
	udpConnection.Receive(udpMessage,1024);
	string message=udpMessage;

	Tuio_Source *curSource;

	if(message!=""){

		vector<string> strPoints = ofSplitString(message,"[/p]");

		for(int i=0;i<strPoints.size();i++){
			
			//std::cout << "UDP.Message:"<< strPoints[i].c_str() << std::endl;

			//split of the UDP  Data for paring

			vector<string> udpInfoChunk = ofSplitString(strPoints[i],"|");


		//looking for a tuioSource that has the same ID name@adress------------
			char source_buffer [256];
			sprintf(source_buffer,"%s",udpInfoChunk[2].c_str());

			std::list<struct Tuio_Source*>::iterator tsrc;
					
			for (tsrc=tuioSources.begin(); tsrc != tuioSources.end(); tsrc++) {
				Tuio_Source *obj = (*tsrc);
				//source ID = name@adress
				char tuio_source_buffer [256];
			
				sprintf(tuio_source_buffer,"%s@%s",obj->source_name.c_str( ), obj->source_addr.c_str( ));// Strings to const char* convert ! 
		
				//std::cout << "tuio_source_buffer:" << tuio_source_buffer << " source_buffer: "<< source_buffer  << std::endl;
				// now we have the right source for the Cursor
				if(strcmp (source_buffer,tuio_source_buffer) == 0 ){
					//std::cout << "UDP.YEAH!:" << std::endl;
					curSource = obj;//copy for later use
				break;
				}
			}//for
		//-------------------------

			/*
			for(int i=0;i<udpInfoChunk.size();i++){
				std::cout << i<< "#:"<< udpInfoChunk[i] << std::endl;
			}
			*/
			
	//TODO: alert if the source is not in the XML config file!!!!!!!
			//Read the UDP Message.
			//The Protokoll is "calibrate|OFF|TUIOSourceAdress|xGridPoints|yGridPoints"
			if(udpInfoChunk[0]=="calibrate" && curSource != NULL){
				std::cout << "#calibrate:"<<udpInfoChunk[1]<< std::endl;
				
				if(udpInfoChunk[1]=="ON"){
					curSource->calibrate= true;
					curSource->GRID_X = atoi(udpInfoChunk[3].c_str());
					curSource->GRID_Y = atoi(udpInfoChunk[4].c_str());
					std::cout << "#curSource->GRID_X:"<<curSource->GRID_X<< ":"<<curSource->GRID_Y<< std::endl;
					//now we have to map the new source screen position
				}
				else if(udpInfoChunk[1]=="OFF"){
					curSource->calibrate= false;
				}
				else if(udpInfoChunk[1]=="POINT"){
					curSource->selectedCalibrationPoint= atoi(udpInfoChunk[3].c_str());
				}
			
			}//if calibrate


		}//for
		
	}//if



} 


#endif