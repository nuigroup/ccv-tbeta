
#include "CPUImageFilter.h"
#include "ofxCvColorImage.h"
#include "ofxCvFloatImage.h"

//--------------------------------------------------------------------------------
CPUImageFilter::CPUImageFilter( const CPUImageFilter& mom ) {
    cvCopy( mom.getCvImage(), cvImage, 0 );
}


//--------------------------------------------------------------------------------
void CPUImageFilter::amplify ( CPUImageFilter& mom, float level ) {

	float scalef = level / 128.0f;

	cvMul( mom.getCvImage(), mom.getCvImage(), cvImageTemp, scalef );
	swapTemp();
}

void CPUImageFilter::highpass ( float blur1, float blur2 ) {

		//Blur Original Image
		cvSmooth( cvImage, cvImageTemp, CV_BLUR , (blur1 * 2) + 1);

		//Original Image - Blur Image = Highpass Image
		cvSub( cvImage, cvImageTemp, cvImageTemp );

		//Blur Highpass to remove noise
		if(blur2 > 0)
		cvSmooth( cvImageTemp, cvImageTemp, CV_BLUR , (blur2 * 2) + 1);

		swapTemp();
}

//--------------------------------------------------------------------------------
void CPUImageFilter::operator =	( unsigned char* _pixels ) {
    setFromPixels( _pixels, width, height );
}

//--------------------------------------------------------------------------------
void CPUImageFilter::operator =	( ofxCvGrayscaleImage& mom ) {
	if( mom.width == width && mom.height == height ) {
        cvCopy( mom.getCvImage(), cvImage, 0 );
	} else {
        cout << "error in =, images are different sizes" << endl;
	}
}

//--------------------------------------------------------------------------------
void CPUImageFilter::operator =	( ofxCvColorImage& mom ) {
	if( mom.width == width && mom.height == height ) {
		cvCvtColor( mom.getCvImage(), cvImage, CV_RGB2GRAY );
	} else {
        cout << "error in =, images are different sizes" << endl;
	}
}

//--------------------------------------------------------------------------------
void CPUImageFilter::operator =	( ofxCvFloatImage& mom ) {
	if( mom.width == width && mom.height == height ) {
		cvConvert( mom.getCvImage(), cvImage );
	} else {
        cout << "error in =, images are different sizes" << endl;
	}
}


//--------------------------------------------------------------------------------
void CPUImageFilter::operator -= ( ofxCvGrayscaleImage& mom ) {
	if( mom.width == width && mom.height == height ) {
		cvSub( cvImage, mom.getCvImage(), cvImageTemp );
		swapTemp();
	} else {
        cout << "error in -=, images are different sizes" << endl;
	}
}

//--------------------------------------------------------------------------------
void CPUImageFilter::operator += ( ofxCvGrayscaleImage& mom ) {
	if( mom.width == width && mom.height == height ) {
		cvAdd( cvImage, mom.getCvImage(), cvImageTemp );
		swapTemp();
	} else {
        cout << "error in +=, images are different sizes" << endl;
	}
}

//--------------------------------------------------------------------------------
void CPUImageFilter::operator *= ( ofxCvGrayscaleImage& mom ) {
    float scalef = 1.0f / 255.0f;
	if( mom.width == width && mom.height == height ) {
		cvMul( cvImage, mom.getCvImage(), cvImageTemp, scalef );
		swapTemp();
	} else {
        cout << "error in *=, images are different sizes" << endl;
	}
}

//--------------------------------------------------------------------------------
void CPUImageFilter::operator &= ( ofxCvGrayscaleImage& mom ) {
	if( mom.width == width && mom.height == height ) {
		cvAnd( cvImage, mom.getCvImage(), cvImageTemp );
		swapTemp();
	} else {
        cout << "error in &=, images are different sizes" << endl;
	}
}


//-------------------------------------------------------------------------------------
void CPUImageFilter::operator -=	( float scalar ){
	cvSubS(cvImage, cvScalar(scalar), cvImageTemp);
	swapTemp();
}

//-------------------------------------------------------------------------------------
void CPUImageFilter::operator +=	( float scalar ){
	cvAddS(cvImage, cvScalar(scalar), cvImageTemp);
	swapTemp();
}

//--------------------------------------------------------------------------------

