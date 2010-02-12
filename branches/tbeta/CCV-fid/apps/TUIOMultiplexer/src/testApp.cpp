#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

	ofSetFrameRate(60);

	ofSetWindowTitle(" TUIO Multiplexer ");

	hProcess = GetCurrentProcess();// process Handle for MemoryCheck

	//return;
	 //removes the 'x' button on windows which causes a crash due to a GLUT bug
	
	#ifdef TARGET_WIN32
		//Get rid of 'x' button
		HWND hwndConsole = FindWindowA(NULL, " TUIO Multiplexer ");
		HMENU hMnu = ::GetSystemMenu(hwndConsole, FALSE);
		RemoveMenu(hMnu, SC_CLOSE, MF_BYCOMMAND);
	#endif


		

	//problem with the initialization of the tuioClient: constructor get called at initialization.no way to set the port afterward
	// so we have to load the XML.config first for the data. 
	//printToFile=false;
	loadXMLSettings("data/config.xml");

	//cout<<"PRINTTOFILE:"<<printToFile<<endl;
	if(printToFile) {
		/*****************************************************************************************************
		* LOGGING
		******************************************************************************************************/
		/* alright first we need to get time and date so our logs can be ordered */
	
		time ( &rawtime );
		timeinfo = localtime ( &rawtime );
		strftime (fileName,80,"../logs/log_%B_%d_%y_%H_%M_%S.txt",timeinfo);
		/*
		FILE *stream ;
		sprintf(fileName, ofToDataPath(fileName).c_str());
		if((stream = freopen(fileName, "w", stdout)) == NULL){}
		*/
	
		
		sprintf(fileName, ofToDataPath(fileName).c_str());
		filestr.open (fileName,ios::out | ios::app );

		cout.rdbuf(filestr.rdbuf());
		
		//filestr.close();

		/******************************************************************************************************/
	}
	
	tuioMultiplexer = new ofxTuioMultiplexer(tuioServerHost, tuioServerPort, tuioClientPort, tuioFlashPort, netinterfaceID, tuio_application_name);	
	
	
	ofAddListener(tuioMultiplexer->objectAdded,this,&testApp::objectAdded);
	ofAddListener(tuioMultiplexer->objectRemoved,this,&testApp::objectRemoved);
	ofAddListener(tuioMultiplexer->objectUpdated,this,&testApp::objectUpdated);

	ofAddListener(tuioMultiplexer->cursorAdded,this,&testApp::tuioAdded);
	ofAddListener(tuioMultiplexer->cursorRemoved,this,&testApp::tuioRemoved);
	ofAddListener(tuioMultiplexer->cursorUpdated,this,&testApp::tuioUpdated);
	
	//the XML is already loaded so we give a pointer  to it
	tuioMultiplexer->setXML(&XML);
	tuioMultiplexer->initXMLSettings();

	tuioMultiplexer->initImage();


	visible = !visible;//we toggle this back in the next method call
	toggleVisibility();
	ofSetWindowPosition(position.x , position.y);


	tuioMultiplexer->init_UDP_calibrate_connection();
	tuioMultiplexer->init_UDP_alive_connection();
	
	tuioMultiplexer->connect();

	fullScreen = (ofGetWindowMode() == OF_FULLSCREEN) ? true : false ;
	setScreenData();
}


void testApp::setScreenData(){
		
		if(fullScreen){
			cout  << "OF_FULLSCREEN:"  << endl;
		tuioMultiplexer->offset.set((ofGetWidth()/2)-(tuioMultiplexer->main_screen_scaled.width /2),(ofGetHeight()/2)-(tuioMultiplexer->main_screen_scaled.height /2));
		}else{
			cout  << "OF_WINDOW:"  << endl;
			
			tuioMultiplexer->offset.set(0.0f,0.0f);
		}
			

}

void testApp::toggleVisibility(){
visible = !visible;
if(visible){
		ofSetWindowShape(tuioMultiplexer->main_screen_scaled.width , tuioMultiplexer->main_screen_scaled.height);//reshape the window to the size from the XML.config
	}else {
		ofSetWindowShape(110 , 1);
	}
}
//--------------------------------------------------------------
void testApp::update(){
	//tuio.getMessage();
	if(tuioMultiplexer == NULL)return;
	tuioMultiplexer->update();//trigger the TuioServer
	
	checkForMemoryLeak();
	
}


//--------------------------------------------------------------
void testApp::draw(){
	if(tuioMultiplexer == NULL)return;
	ofBackground(0,0,0);
	
	glPushMatrix();
	glTranslatef(tuioMultiplexer->offset.x,tuioMultiplexer->offset.y, 0.0);
	
	//tuioMultiplexer->blobImage.draw(0,0);
	
	if(visible){
	
		//tuioMultiplexer->drawCursors();


		tuioMultiplexer->drawSourceScreens();
		tuioMultiplexer->drawObjects();

		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		tuioMultiplexer->blobImageBw.draw(0,0);// Here we see the Blobs as visual feedback so we didn't have to call tuioMultiplexer->drawCursors() explicit

	
	}
	
	
		
	//tuioMultiplexer->drawDebugInfo();
	glPopMatrix();

}


//--------------------------------------------------------------
void testApp::keyPressed  (int key){
	
	cout << "KeY:"<< key << keyMapper.isShiftDown()<< endl;

	int delta = keyMapper.isShiftDown() ? 10 : 1 ;
switch(key){
	case ' ':
		ofToggleFullscreen();
		break;
		case 356:
		//LEFT
			
		ofSetWindowPosition(ofGetWindowPositionX()-delta , ofGetWindowPositionY());
		break;
	case 357:
		//UP
		ofSetWindowPosition(ofGetWindowPositionX() , ofGetWindowPositionY()-delta);
		break;
	case 358:
		//RIGHT
		ofSetWindowPosition(ofGetWindowPositionX()+delta , ofGetWindowPositionY());
		break;
	case 359:
		//DOWN
		ofSetWindowPosition(ofGetWindowPositionX() , ofGetWindowPositionY()+delta);
		break;
	case 'h':
			toggleVisibility();

		break;
	case 'q':
		break;


		
	}

}
//--------------------------------------------------------------
void testApp::keyReleased(int key){

	cout  << "keyReleased:" << key << ":"<< (char)key<<endl;
	int w ;
	switch(key){
	case ' ':
		// we need to catch this on Key up as on Keydown we get not the right WindowMode after the ofToggleFullscreen() call
		fullScreen = (ofGetWindowMode() == OF_FULLSCREEN) ? true : false ;
		if(fullScreen){
			cout  << "OF_FULLSCREEN:"  << endl;	
		}else{
			cout  << "OF_WINDOW:"  << endl;
			//ofSetWindowShape(100 , 50);
			ofSetWindowShape(tuioMultiplexer->main_screen_scaled.width , tuioMultiplexer->main_screen_scaled.height);
		}

		setScreenData();
		break;
	case 'x':
		cout  << "x:" << key << endl;
		break;
	
	}
	

}


//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mouseReleased(){


}


void testApp::objectAdded(TuioObject & tuioObject){
	cout << " new object: " + ofToString(tuioObject.getSymbolID())+
		" X: "+ofToString(tuioObject.getX())+
		" Y: "+ofToString(tuioObject.getY())+
		" angle: "+ofToString(tuioObject.getAngleDegrees())
	<< endl;
	
}
void testApp::objectRemoved(TuioObject & tuioObject){
	cout << " object removed: " + ofToString(tuioObject.getSymbolID())+
		" X: "+ofToString(tuioObject.getX())+
		" Y: "+ofToString(tuioObject.getY())+
		" angle: "+ofToString(tuioObject.getAngleDegrees())
	<< endl;
}
void testApp::objectUpdated(TuioObject & tuioObject){
	cout << " object updated: " + ofToString(tuioObject.getSymbolID())+
		" X: "+ofToString(tuioObject.getX())+
		" Y: "+ofToString(tuioObject.getY())+
		" angle: "+ofToString(tuioObject.getAngleDegrees())
	<< endl;
}

void testApp::tuioAdded(TuioCursor & tuioCursor){
	/*
	cout << " new cursor: " + ofToString(tuioCursor.getCursorID())+
		" X: "+ofToString(tuioCursor.getX())+
		" Y: "+ofToString(tuioCursor.getY())
	<< endl;
	*/
	ofxVec3f vec;		
	tuioMultiplexer->localToGlobal(&vec,&tuioCursor,tuioMultiplexer->tuioCourser_source_map[&tuioCursor]);
	cout << "global Pos:" 
	<< " X: "<< vec.x
	<< " Y: "<< vec.y
	<< endl;
	//cout << "getTuioCursors(0)" << (int) tuio.client->getTuioCursors(0).size() << endl;
	//cout << "getTuioCursors(1)" << (int) tuio.client->getTuioCursors(1).size() << endl;
}
void testApp::tuioRemoved(TuioCursor & tuioCursor){
	/*
	cout << " cursor removed: " + ofToString(tuioCursor.getCursorID())+
		" X: "+ofToString(tuioCursor.getX())+
		" Y: "+ofToString(tuioCursor.getY())
	<< endl;
	*/
}
void testApp::tuioUpdated(TuioCursor & tuioCursor){
	/*
	cout << " cursor updated: " + ofToString(tuioCursor.getCursorID())+
		" X: "+ofToString(tuioCursor.getX())+
		" Y: "+ofToString(tuioCursor.getY())
	<< endl;
	*/
}

 void testApp::loadXMLSettings(string path)
	{
		string message;

		if( XML.loadFile(path) ){
		message = "testapp.xml loaded!";
	}else{
		message = "unable to load mySettings.xml check data/ folder";
	}
		std::cout << "<<<<" << message<<":"<<path << std::endl;
	
		XML.pushTag("CONFIG"  , 0);
		printToFile = XML.getValue("printToFile",0, 0);
			XML.pushTag("tuioServer"  , 0);
				tuioServerHost = XML.getValue("host","127.0.0.1", 0);
				tuioServerPort =XML.getValue("port",3334, 0);
				 tuioFlashPort=XML.getValue("flashport_out",3000, 0);
				tuio_application_name=XML.getValue("tuio_source_application","TUIOSOURCEFILTER", 0);
				cout  << "tuioServerHost:" << tuioServerHost << endl;
				cout  << "tuioServerPort:" << tuioServerPort << endl;
			XML.popTag();

			netinterfaceID = XML.getValue("network_interface_nr",1, 0);

			XML.pushTag("tuioClient"  , 0);
				tuioClientPort = XML.getValue("port",3333, 0);
				cout  << "tuioClientPort:" << tuioClientPort << endl;
			XML.popTag();

			visible = XML.getValue("visible",1,0);
			//int		getAttribute(const string& tag, const string& attribute, int defaultValue, int which = 0);
			position.x = XML.getAttribute("position","x",0,0);
			position.y = XML.getAttribute("position","y",0,0);
			maxMemoryAllocation = XML.getValue("maxMemoryAllocation",50,0)*1024;//in MB
			

		XML.popTag();
 }

 void testApp::checkForMemoryLeak(){


	
	//info about memory usage of the current Process: http://msdn.microsoft.com/en-us/library/ms682050%28VS.85%29.aspx 
	//	or http://www.openframeworks.cc/forum/viewtopic.php?f=9&t=372&hilit=memory+usage 
		//Linke Settings: additional dependencies : Psapi.Lib
		//and don't forget : psapi.dll to distribute (to find in Windows/system32/)

		 /*
      The working set is the amount of memory physically mapped to
      the process context at a given time. Memory in the paged pool
      is system memory that can be transferred to the paging file on
      disk (paged) when it is not being used. Memory in the nonpaged
      pool is system memory that cannot be paged to disk as long as
      the corresponding objects are allocated. The pagefile usage
      represents how much memory is set aside for the process in the
      system paging file. When memory usage is too high, the virtual
      memory manager pages selected memory to disk. When a thread
      needs a page that is not in memory, the memory manager reloads
      it from the paging file.
   */

	 /*
		PROCESS_MEMORY_COUNTERS pmc;
		HANDLE hProcess;
		hProcess = GetCurrentProcess();
		*/


	  if ( GetProcessMemoryInfo( hProcess, &pmc, sizeof(pmc)) )
    {
		/*
        cout<< "\tPageFaultCount: "<< pmc.PageFaultCount << endl;
        cout<<  "\tPeakWorkingSetSize: "<< pmc.PeakWorkingSetSize << endl;
        cout<<  "\tWorkingSetSize: "<< pmc.WorkingSetSize << endl;
        cout<<  "\tQuotaPeakPagedPoolUsage: "<< pmc.QuotaPeakPagedPoolUsage <<endl;
        cout<<  "\tQuotaPagedPoolUsage: "<< pmc.QuotaPagedPoolUsage << endl;
        cout<<  "\tQuotaPeakNonPagedPoolUsage: "<< pmc.QuotaPeakNonPagedPoolUsage << endl;
        cout<<  "\tQuotaNonPagedPoolUsage: "<< pmc.QuotaNonPagedPoolUsage<< endl;
        cout<<  "\tPagefileUsage: "<< pmc.PagefileUsage<< endl;
        cout<<  "\tPeakPagefileUsage: "<< pmc.PeakPagefileUsage<< endl;
		*/
    }

		//cout << GetProcessMemoryInfo(GetCurrentProcess(),&pmc,sizeof(pmc) )<< endl;
		
		if(pmc.WorkingSetSize/1024 > maxMemoryAllocation){
		CloseHandle( hProcess );
		delete tuioMultiplexer;//clean up the connection, otherwise the follow Process will have no UDP connection! 
		//Sleep(2000); 
		//system("start CalibrationGUI.exe");//With a short console Popup :-(
		_execl("TUIO Multiplexer.exe","_execl",NULL);//Terminates the running process and starts a new Instance
		//_spawnl(_P_OVERLAY,"CalibrationGUI.exe","_spawnl",NULL); // same as _execl
		//OF_EXIT_APP(0);
		}
		
 
 
 
 }
