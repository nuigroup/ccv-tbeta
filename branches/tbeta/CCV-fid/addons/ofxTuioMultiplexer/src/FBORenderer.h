#ifndef FBO_RENDERER_H
#define FBO_RENDERER_H
#include "ofMain.h"
//#include "ofTexture.h"
//#include "ofGraphics.h"
#include "ofImage.h"
//#include "ofxCvGrayscaleImage.h"
#include "ofxOpenCv.h"
#include "Blob.h"
//#include "GLee.h"
//#include <GL/gl.h>


class FBORenderer {

public:
	FBORenderer();
	~FBORenderer(){};

	void initFBO(int _width,int _height,float _blobSize, float _scaleFactor);
	void grabImageData(void *pixels);
	void drawBlobs(ofxCvColorImage &gpuReadBackImageGS, std::vector<Blob> &vtcur);
	void drawBlobs(ofImage &gpuReadBackImageGS, std::vector<Blob> &vtcur);
	GLuint * getTexture();

private:
GLuint fbo_buffer;//FBO
GLuint output_texture;
GLuint render_buffer;
unsigned char  * gpuReadBackBuffer;
int width;
int height;
float scaleFactor;//for the blob size
float blobSize;
	

//private:
	
};
#endif