 #ifndef _TUIO_MULTIPLEXER_LOADXML
#define _TUIO_MULTIPLEXER_LOADXML

	/****************************************************************
	*	Load/Save config.xml file Settings
	****************************************************************/

 void ofxTuioMultiplexer::setXML(ofxXmlSettings *xml)
 {
	XML = *xml;
 }

 void ofxTuioMultiplexer::loadXMLSettings(string path)
 {
		string message;

		if( XML.loadFile(path) ){
			message = "mySettings.xml loaded!";
		}else{
			message = "unable to load mySettings.xml check data/ folder";
		}
		std::cout << "<<<<" << message<<":"<<path << std::endl;
 }

 //should only sbe called after setXML or loadXMLSettings 
	 void ofxTuioMultiplexer::initXMLSettings()
	{
		
	
	XML.pushTag("CONFIG"  , 0);

		fbo_scale = XML.getValue("scalefactor",1.0f, 0);
		blobSize = XML.getValue("blobSize",1.0f, 0);
		screenMode =XML.getAttribute("screen","mode","square", 0);
		

		ofxPoint2f offset;
		float radius;


		XML.pushTag("screen"  , 0);
			if(screenMode== "circle"){
				main_screen.width = XML.getValue("circle:diameter",0.0f, 0);//XML.getValue("width",0.0f, 0);
				main_screen.height =XML.getValue("circle:diameter",0.0f, 0) ;//XML.getValue("height",0.0f, 0);
			}else {
				//square!!!!!!!!!!!!!
				main_screen.width=XML.getValue("square:width",0.0f, 0);
				main_screen.height=XML.getValue("square:height",0.0f, 0);
			}
		XML.popTag();

		
		XML.pushTag("screen", 0);
			if(screenMode== "circle"){
				// helper variables to translate the screens
				radius = XML.getValue("circle:diameter",0.0f, 0)/fbo_scale  ;
				offset = ofxPoint2f( XML.getValue("circle:diameter",0.0f, 0)/2.0, XML.getValue("circle:diameter",0.0f, 0)/2.0 ) /fbo_scale;

				cout  << "radius:" << radius << endl;
				cout  << "offset:" << offset.x <<":" <<offset.y <<endl;
			}else{
				//square!!!!!!!!!!!!!
				offset =ofxPoint2f(0.0,0.0);
			
			}
		XML.popTag();
	

		//calib
		XML.pushTag("calibrate", 0);
			calibration_udp_port = XML.getValue("CALIBRATION_UDP_PORT",0, 0);
		XML.popTag();
		//Alive for monitoring the Application
		XML.pushTag("alive", 0);
			alive_udp_in_port = XML.getValue("UDP_IN_PORT", 10100, 0);cout  << "ALIVE_UDP_IN_PORT:" << alive_udp_in_port << endl;
			alive_udp_out_port = XML.getValue("UDP_OUT_PORT", 10101 ,0);cout  << "ALIVE_UDP_OUT_PORT:" << alive_udp_out_port << endl;
			alive_udp_out_host = XML.getValue("OUT_HOST","127.0.0.1", 0);cout  << "ALIVE_UDP_OUT_HOST:" << alive_udp_out_host << endl;
		XML.popTag();



		XML.pushTag("tuio_sources", 0);
	
			int numSourceTags = XML.getNumTags("source");
			
			for (int i =0;i< numSourceTags;i++){
				
				Tuio_Source * p_ts = new Tuio_Source ;
				p_ts->source_id = INT_MAX;// this value should be high enough that nobody in the world imagine to take this ID in the config.xml
				
				
				p_ts->nr = XML.getAttribute("source","nr",0, i);
				nrToSource_map[p_ts->nr] = p_ts;

				p_ts->source_name = XML.getAttribute("source","name","NOT_FOUND", i);
				p_ts->source_addr = XML.getAttribute("source","address","NOT_FOUND", i);
				

				XML.pushTag("source"  , i);
				
					float trans_X;
					float trans_Y;	
					float radius_source;
					float radius_offset; 

					p_ts->screen.width = XML.getValue("width",0.0f, 0) / fbo_scale;
					p_ts->screen.height = XML.getValue("height",0.0f, 0) / fbo_scale;
				
					if(screenMode== "circle"){						

							p_ts->rotation.set(0.0f,0.0f,XML.getAttribute("rotation","deg",0.0f, 0));;// first data for Translation calculation

							radius_offset = XML.getValue("radius_offset",0.0f, 0)/fbo_scale;
							radius_source= (radius - p_ts->screen.height) / 2.0;			
							radius_source += radius_offset;

							trans_X =	offset.x +(radius_source * cos((double)(p_ts->rotation.z * PI/180.0))); 
							trans_Y=	offset.y +(radius_source * sin((double)(p_ts->rotation.z * PI/180.0)));

							cout  << "translation:" << trans_X <<":" <<trans_Y <<endl;
							cout  << "translation:" << p_ts->rotation.z <<endl;
						
							p_ts->rotation.z -=90;//second data to get the right direction of the Screens
							p_ts->translation.set(trans_X,trans_Y,0.0f);
						}else{
						
							//square!!!!!!!!!!!!!
							//we have to translate the middle of the square as we have there the registrationpoint							 
							cout  << "translation:" << XML.getAttribute("translation","x",0.0f, 0) <<":" <<XML.getAttribute("translation","y",0.0f, 0) <<endl;
							trans_X = (XML.getAttribute("translation","x",0.0f, 0)/ fbo_scale )+(p_ts->screen.width/2);
							trans_Y = (XML.getAttribute("translation","y",0.0f, 0)/ fbo_scale) +(p_ts->screen.height/2);

							p_ts->translation.set(trans_X,trans_Y,0.0f);

					}
									
					p_ts->color.r =  XML.getAttribute("color","r",0.f, 0) / 255;
					p_ts->color.g =  XML.getAttribute("color","g",0.f, 0) /255;
					p_ts->color.b =  XML.getAttribute("color","b",0.f, 0) /255;
					p_ts->color.a = 0.025;

					p_ts->calibrate = false; //default

					
				XML.popTag();//source
				
				tuioSources.push_back(p_ts);//fill the datastructure!!!!

			} //sources

		XML.popTag();//tuio_sources

	XML.popTag();//CONFIG


};



#endif