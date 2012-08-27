

#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class CatPictureApp : public AppBasic {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
private:
	float redColor;
	float greenColor;
	float blueColor;
};

void CatPictureApp::setup()
{
	redColor = 0.0f;
	greenColor = 0.0f;
	blueColor = 0.0f;
}

void CatPictureApp::mouseDown( MouseEvent event )
{
}

void CatPictureApp::update()
{
	/// Increments colors at varying rates to produce a wide range of colors
	redColor = redColor + .005f;
	greenColor = greenColor + .01f;
	blueColor = blueColor + .015f;
	
	///Following three if statements prevent the colors from going out of range
	if(redColor > 1.0){
		redColor = 0.0f;
	}

	if(greenColor > 1.0){
		greenColor = 0.0f;
	}

	if(blueColor > 1.0){
		blueColor = 0.0f;
	}

}

void CatPictureApp::draw()
{
	/// Resets screen color to various mixtures of red/green/blue
	gl::clear( Color( redColor, greenColor, blueColor ) ); 
}

CINDER_APP_BASIC( CatPictureApp, RendererGl )
