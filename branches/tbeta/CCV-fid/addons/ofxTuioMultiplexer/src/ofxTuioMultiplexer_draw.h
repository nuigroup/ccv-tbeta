 #ifndef _TUIO_MULTIPLEXER_DRAW
#define _TUIO_MULTIPLEXER_DRAW



	void ofxTuioMultiplexer::drawCursors(){
	
   // std::list<Tuio_Source*> cursorList = client->getTuioCursors();
			std::list<struct Tuio_Source*>::iterator tsrc;
		
		for (tsrc=tuioSources.begin(); tsrc != tuioSources.end(); tsrc++) {
			Tuio_Source *obj = (*tsrc);


			//cout <<  "drawCursors: " << obj->source_id << endl;

				std::list<TuioCursor*>::iterator tit;
				std::list<TuioCursor*> cursorList ;
				lockCursorList();


				cursorList = getTuioCursors(obj->source_id);
				//cout<<"--drawCursors: "<<cursorList.size() <<endl;

				for (tit=cursorList.begin(); tit != cursorList.end(); tit++) {
					TuioCursor * cur = (*tit);
					
					//cout<<"----------"<<endl;										
					glColor3f(0.0,1.0,0.0);
					glColor3f(obj->color.r,obj->color.g,obj->color.b);
					//cout<<"drawCursors:"<< obj->color.r << " : "<< obj->color.g << " : " << obj->color.b<<endl;

					//-------------------------
					ofxVec3f pos;

					localToGlobal(&pos,cur,obj);
					/*
					pos.x = (cur->getX()*obj->screen.width)-(obj->screen.width/2.0);
					pos.y = (cur->getY()*obj->screen.height)-(obj->screen.height/2.0);
					pos.rotate( obj->rotation.z,  ofxVec3f(0,0,1) );

					pos.x += obj->translation.x;
					pos.y += obj->translation.y;
					*/
					ofEllipse(pos.x, pos.y, 10.0, 10.0);

					/*
					glPushMatrix();
						//: 
						glTranslatef(obj->translation.x, obj->translation.y, 0.0);
						//ofEllipse(cur->getX()*ofGetWidth(), cur->getY()*ofGetHeight(), 10.0, 10.0);
						glRotatef(obj->rotation.z, 0, 0, 1);

						glTranslatef(cur->getX()*obj->screen.width, cur->getY()*obj->screen.height, 0.0);
						glTranslatef(-(obj->screen.width/2.0), -(obj->screen.height/2.0),0.0);

						ofEllipse(0.0, 0.0, 10.0, 10.0);
						glColor3f(1.0,1.0,1.0);
						string str = "CursorId: "+ofToString((int)(cur->getCursorID()));
						ofDrawBitmapString(str, -10.0, 25.0);
						str = "SessionId: "+ofToString((int)(cur->getSessionID()));
						ofDrawBitmapString(str, -10.0, 40.0);	
					glPopMatrix();
					*/
					//-------------------------
					
				}

				unlockCursorList();

		}// for tuioSources

		
};
	void ofxTuioMultiplexer::drawCursor(ofxVec3f * pos){
	
	glColor3f(0.0,1.0,0.0);
	ofEllipse(pos->x, pos->y, 5.0, 5.0);
	//cout<<"drawCursors:"<< pos->x << " : "<< pos->y <<endl;

	}

	void ofxTuioMultiplexer::drawCursor(TuioCursor * tcur){
	
		std::list<struct Tuio_Source*>::iterator tsrc;
		//ofNoFill();
		glColor3f(0.0,1.0,0.0);
		//looking for a tuioSource that has the same ID name@adress
		for (tsrc=tuioSources.begin(); tsrc != tuioSources.end(); tsrc++) {

			Tuio_Source *obj = (*tsrc);

			if(obj->source_id == tcur->getTuioSourceID()){
				//-------------------------
				ofxVec3f pos;

				pos.x = (tcur->getX()*obj->screen.width)-(obj->screen.width/2.0);
				pos.y = (tcur->getY()*obj->screen.height)-(obj->screen.height/2.0);
				pos.rotate( obj->rotation.z,  ofxVec3f(0,0,1) );

				pos.x += obj->translation.x;
				pos.y += obj->translation.y;

				ofEllipse(pos.x, pos.y, 10.0, 10.0);

				break;
			}//if


		}//for

		

	};





void ofxTuioMultiplexer::drawObjects(){
	
   // std::list<Tuio_Source*> cursorList = client->getTuioCursors();

	std::list<TuioObject>::iterator it;
	//ofSetRectMode(OF_RECTMODE_CORNER);
	ofSetRectMode(OF_RECTMODE_CENTER);
	ofNoFill();
	glColor3f(0.0,1.0,0.0);

	for (it=transformed_tobj_List.begin(); it != transformed_tobj_List.end(); it++) {
		TuioObject  *tobj = &(*it);

		glPushMatrix();
			glTranslatef(tobj->getX()*main_screen_scaled.width, tobj->getY()*main_screen_scaled.height,0.0);
			float rot = tobj->getAngle()*180/PI;
			glRotatef(rot, 0, 0, 1);

			
			ofRect(0, 0,20/fbo_scale, 20/fbo_scale);
			//ofFill();
			//
			string str = "SymbolId: "+ofToString((int)(tobj->getSymbolID()));
			ofDrawBitmapString(str, 0.0f, 0.0f);
			//str = "SessionId: "+ofToString((int)(obj->getSessionID()));
			//ofDrawBitmapString(str, obj->getX()*ofGetWidth()-10.0, obj->getY()*ofGetHeight()+40.0);

			//tobj->getSymbolID();
			//
		glPopMatrix();


	
	}//for


ofSetRectMode(OF_RECTMODE_CORNER);


	return;
			std::list<struct Tuio_Source*>::iterator tsrc;
		

			ofSetRectMode(OF_RECTMODE_CENTER);

		for (tsrc=tuioSources.begin(); tsrc != tuioSources.end(); tsrc++) {
			Tuio_Source *obj = (*tsrc);

				std::list<TuioObject*>::iterator tit;
				std::list<TuioObject*> objectList ;
				lockObjectList();

				objectList = getTuioObjects(obj->source_id);

				for (tit=objectList.begin(); tit != objectList.end(); tit++) {
					TuioObject * tobj = (*tit);
												
					glColor3f(0.0,1.0,0.0);
					glColor3f(obj->color.r,obj->color.g,obj->color.b);

					//-------------------------
					ofxVec3f pos;

					pos.x = (tobj->getX()*obj->screen.width)-(obj->screen.width/2.0);
					pos.y = (tobj->getY()*obj->screen.height)-(obj->screen.height/2.0);
					pos.rotate( obj->rotation.z,  ofxVec3f(0,0,1) );

					pos.x += obj->translation.x;
					pos.y += obj->translation.y;

					glPushMatrix();
					glTranslatef(pos.x, pos.y,0.0);
					float rot = obj->rotation.z + (tobj->getAngle()*180/PI);
					glRotatef(rot, 0, 0, 1);

					ofNoFill();
					ofRect(0, 0,20, 20);
					//ofFill();
					//
					string str = "SymbolId: "+ofToString((int)(tobj->getSymbolID()));
					//ofDrawBitmapString(str, obj->getX()*ofGetWidth()-10.0, obj->getY()*ofGetHeight()+25.0);
					//str = "SessionId: "+ofToString((int)(obj->getSessionID()));
					//ofDrawBitmapString(str, obj->getX()*ofGetWidth()-10.0, obj->getY()*ofGetHeight()+40.0);
				
					//tobj->getSymbolID();
					//
					glPopMatrix();
						verdana2.drawString(str,pos.x, pos.y);//after rotation otherwise no display
					//ofEllipse(pos.x, pos.y, 20.0, 20.0);

					//-------------------------
					
				}

				unlockObjectList();

		}// for tuioSources

		ofSetRectMode(OF_RECTMODE_CORNER);
};



	//----Screen

	void ofxTuioMultiplexer::drawSourceScreens(){
		ofEnableAlphaBlending();

		//Draw Circle
		ofNoFill();
		glColor4f(1.0,0.0,0.0,1.0);
		//glColor3f(0.0,1.0,0.0);

		if(screenMode== "circle"){
			glPushMatrix();
				glTranslatef(main_screen_scaled.width/2.0f, main_screen_scaled.height/2.0f, 0.0);
				
				ofSetCircleResolution(48) ;
				ofCircle(0.0f,0.0f,main_screen_scaled.width/2.0f);
			glPopMatrix();
		}

		//Draw Screen Frame
		ofFill();	
		ofSetRectMode(OF_RECTMODE_CORNER);
		//ofSetColor(255,0,0,127);   // red, 50% transparent
		//glColor4f(0.0,1.0,0.0,0.2);
		//ofSetColor(54,54,54,54);
		ofSetColor(54,54,54,255);
		ofRect( 0.0f, 0.0f, main_screen_scaled.width,  main_screen_scaled.height);

		//Draw Source Frames
		
		glColor4f(0.0,1.0,0.0,1.0);
		ofSetRectMode(OF_RECTMODE_CENTER);
		std::list<struct Tuio_Source*>::iterator tsrc;


		for (tsrc=tuioSources.begin(); tsrc != tuioSources.end(); tsrc++) {
				Tuio_Source *obj = (*tsrc);
		


			glPushMatrix();
			//we position the rotation point
			//at the location we want it to
			//spin around .
			if(screenMode== "circle"){
				glTranslatef(obj->translation.x, obj->translation.y, 0);
			}else {
				glTranslatef(obj->translation.x, obj->translation.y, 0);
			}
			glRotatef(obj->rotation.z, 0, 0, 1);	

			ofNoFill();
			glColor4f(obj->color.r,obj->color.g,obj->color.b,1.0f);
			ofRect(0.0,0.0,obj->screen.width, obj->screen.height);

			ofFill();
			glColor4f(obj->color.r,obj->color.g,obj->color.b,obj->color.a);
			//ofSetColor(obj->color.r,obj->color.g,obj->color.b,obj->color.a);
			ofRect(0.0,0.0,obj->screen.width, obj->screen.height);

			ofSetColor(0xffffff);
			//ofDrawBitmapString(ofToString(obj->nr), 0,0);
			verdana.drawString(ofToString(obj->nr),0, 0);


			//draw the calibPoints if needed
			if(obj->calibrate){
			
			//TODO wieder zurück translaten!!
				int rows;
				int cols;
				int i =0;
				float xd = obj->screen.width / (float) obj->GRID_X;
				float yd = obj->screen.height / (float) obj->GRID_Y;
				
				glTranslatef(-(obj->screen.width/2), -(obj->screen.height/2), 0); 

				for(rows=0;rows <= obj->GRID_Y;rows++){	
					for(cols=0;cols <= obj->GRID_X;cols++){	
						i=(rows* (obj->GRID_X+1))+cols;
						glPushMatrix();
							glTranslatef(xd* cols, yd* rows, 0);
							if(obj->selectedCalibrationPoint == i){
								ofSetColor(0xff0000);
								ofCircle(0.0f, 0.0f, 4.0f/fbo_scale); 
							}else{
								ofSetColor(0xffffff);
								ofCircle(0.0f, 0.0f, 2.0f/fbo_scale); 
							}					
						glPopMatrix();
					}//cols	
				}//rows
			}//calibrate

			
			glPopMatrix();

		}

		ofFill();	
		ofSetRectMode(OF_RECTMODE_CORNER);
	};


	void ofxTuioMultiplexer::drawDebugInfo(){
		glColor3f(1.0,1.0,1.0);
		/*
	int i = courserToFilterCursor_map.size();
	int yversatz = 25;
	int x = 25;
	
		verdana2.drawString("courserToFilterCursor_map:"+ofToString(i),0, yversatz);//filterCursor..size()
		yversatz+=x;
		i = tuioCourser_source_map.size();
		verdana2.drawString("tuioCourser_source_map:"+ofToString(i),0, yversatz);

		yversatz+=x;
		i = tuioServer->getTuioCursors().size();
		verdana2.drawString("tuioServer->getTuioCursors():"+ofToString(i),0, yversatz);

		yversatz+=x;
		i = server_courserToFilterCursor_map.size();
		verdana2.drawString("server_courserToFilterCursor_map:"+ofToString(i),0, yversatz);

		yversatz+=x;
		i = filterCursorToServer_courser_map.size();
		verdana2.drawString("filterCursorToServer_courser_map:"+ofToString(i),0, yversatz);

		yversatz+=x;
		i = filterCursors.size();
		verdana2.drawString("filterCursors:"+ofToString(i),0, yversatz);
		*/
		
	}
	

#endif