#include "ofMain.h"
#include "testApp.h"

#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
#endif 

//========================================================================
int main( ){

//int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine,int nCmdShow){
	
	
	//Linkersettings entrypoint for WIN 32 Applcation witout CONSOLE Window: 
	//mainCRTStartup for main()
	//wWinMainCRTStartup for WinMain()
	// nothing for int APIENTRY _tWinMain
	// but GLUT has a Bug so we must disable the close Button other wise the console-process stays in the system


	//_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );//for Memory Debugging

	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofSetupOpenGL(1024, 768, OF_WINDOW);			// <-------- setup the GL context
	//ofSetupOpenGL(768, 768, OF_FULLSCREEN);	
	
	// this kicks off the running of my app
	ofRunApp(new testApp());
	//ShellExecute(NULL, "open", "cmd", "/k dir c:", NULL, SW_SHOWNORMAL ); 
	//return 0;
	//_CrtDumpMemoryLeaks();
}

