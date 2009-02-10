#ifndef _CALIBRATION
#define _CALIBRATION

#define OF_ADDON_USING_OFXXMLSETTINGS  // LOAD CONFIG.XML
#include "ofAddons.h"

//Used other calibration
#include "rect2d.h"		
#include "vector2d.h"


class calibrationB
{
	public:

		calibrationB();										 //constructor
		//~calibrationB();									 //destructor
			

		void loadXMLSettings();							     // Load Settings
		void saveCalibration();								 // Save Settings
		
		//Calibration Methods		
		void setScreenScale(float s);
		void setScreenBBox(rect2df & bbox);

		void setGrid(int x, int y);
		void setCamRes(int camWidth, int camHeight);

		void initTriangles();

		virtual vector2df *getScreenPoints() { return screenPoints; };
		virtual vector2df *getCameraPoints() { return cameraPoints; };

		float getScreenScale();
		rect2df getScreenBBox() { return screenBB; };

		void computeCameraToScreenMap();
		void cameraToScreenPosition(float &x, float &y);
		void cameraToScreenSpace(float &x, float &y);
		void transformDimension(float &width, float &height);

		void initScreenPoints();
		void initCameraPoints(int camWidth, int camHeight);
	
		// returns -1 if none found..
		int findTriangleWithin(vector2df pt);

		bool isPointInTriangle(vector2df p, vector2df a, vector2df b, vector2df c);

		//! starts calibration
		virtual void beginCalibration();

		//! goes to the next step
		virtual void nextCalibrationStep();

		//! return to the last step
		virtual void revertCalibrationStep();

		void calculateBox();

		bool bCalibrating;		
		int calibrationStep;

		vector2df* screenPoints;					// GRID_X * GRID_Y
		vector2df* cameraPoints;					// GRID_X * GRID_Y
		rect2df screenBB;

		int GRID_POINTS;
		int GRID_X;
		int GRID_Y;
		
		float maxBoxX;
		float minBoxX;
		float maxBoxY;
		float minBoxY;


	private:
		//---------------------------------------Calibration

		//set Calibration Points
		int GRID_INDICES;

		int* triangles;								// GRID_X * GRID_Y * 2t * 3i indices for the points
		vector2df* cameraToScreenMap;

		int	_camWidth;
		int _camHeight;

		bool bscreenPoints;
		bool bcameraPoints;

		ofxXmlSettings		calibrationXML;
		string				xmlStructure;
		string				message;

};
#endif
