

#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"
#include "Resources.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class CatPictureApp : public AppBasic {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
	void prepareSettings(Settings* settings);

  private:
	Surface* mySurface;

	static const int AppWidth = 800;
	static const int AppHeight = 600;
	static const int TextureSize = 1024;
	Color8u c;

	void setDefaultColor();
	void addRectangle(uint8_t* dataArray, int rectHeight, int rectLength, int centerX, int centerY);
	void addCircle(uint8_t* dataArray, int radius, int xCenter, int yCenter);
	void addLine(int firstX, int firstY, int secondX, int secondY); //used http://www.codekeep.net/snippets/e39b2d9e-0843-4405-8e31-44e212ca1c45.aspx as a template
	void setPixel(int x, int y); //Required by addLine
	void addTriangle(int firstX, int firstY, int secondX, int secondY, int thirdX, int thirdY);
};

void CatPictureApp::prepareSettings(Settings* settings){
	(*settings).setWindowSize(AppWidth, AppHeight);
	(*settings).setResizable(false);
}

void CatPictureApp::setDefaultColor(){
	uint8_t* dataArray = (*mySurface).getData();
	c.g = 255;
	c.r = 255;
	c.b = 255;

	for(int i = 0; i < AppWidth; i++){
		for(int j = 0; j < AppHeight; j++){
			int offset = 3*(i + j*TextureSize);
			dataArray[offset] = c.r;
			dataArray[offset+1] = c.b;
			dataArray[offset+2] = c.g;
				
		}
	}	
}

void CatPictureApp::addRectangle(uint8_t* dataArray, int rectHeight, int rectLength, int centerX, int centerY){
	//A.1
	c.g = 0;
	c.r = 0;
	c.b = 0;

	int staticHeight = rectHeight;
	int staticLength = rectLength;

	for(int i = centerY; i <= centerY + staticHeight; i++){
		if(rectHeight >= 0){
			for(int j = centerX; j <= centerX + staticLength; j++){
				if(rectLength >= 0){
					if(i == centerY || i == centerY + staticHeight || j == centerX || j == centerX + staticLength){
						int offset = 3*(j + i*TextureSize);
						dataArray[offset] = c.r;
						dataArray[offset+1] = c.b;
						dataArray[offset+2] = c.g;
						rectLength--;
					}
				}
			}
			rectLength = staticLength;
			rectHeight--;
		}
	}
}

void CatPictureApp::addCircle(uint8_t* dataArray, int radius, int xCenter, int yCenter){
	//A.2
	c = Color8u(0,0,0);

	for(int i = yCenter-radius; i<=yCenter+radius; i++){
		for(int j = xCenter-radius; j <= xCenter+radius; j++){
			int distance = (int)sqrt((double)((j - xCenter)*(j-xCenter) + (i - yCenter)*(i - yCenter)));
			if(distance <= radius){
				if(distance%radius < 1 && distance > 9){
					int offset = 3*(j + i*TextureSize);
					dataArray[offset] = c.r;
					dataArray[offset+1] = c.g;
					dataArray[offset+2] = c.b;
				}
			}
		}
	}
}

void CatPictureApp::addLine(int firstX, int firstY, int secondX, int secondY){
	int F, x, y;

    if (firstX > secondX)  // Swap points if first point is on the right of second point
    {
        swap(firstX, secondX);
        swap(firstY, secondY);
    }

    // Handle trivial cases separately for algorithm speed up.
    // Trivial case 1: m = +/-INF (Vertical line)
    if (firstX == secondX)
    {
        if (firstY > secondY)  // Swap y-coordinates if p1 is above p2
        {
            swap(firstY, secondY);
        }

        x = firstX;
        y = firstY;
        while (y <= secondY)
        {
            setPixel(x, y);
            y++;
        }
        return;
    }
    // Trivial case 2: m = 0 (Horizontal line)
    else if (firstY == secondY)
    {
        x = firstX;
        y = firstY;

        while (x <= secondX)
        {
            setPixel(x, y);
            x++;
        }
        return;
    }

    int dy            = secondY - firstY;  // y-increment from p1 to p2
    int dx            = secondX - firstX;  // x-increment from p1 to p2
    int dy2           = (dy << 1);  // dy << 1 == 2*dy
    int dx2           = (dx << 1);
    int dy2_minus_dx2 = dy2 - dx2;  // precompute constant for speed up
    int dy2_plus_dx2  = dy2 + dx2;


    if (dy >= 0)    // m >= 0
    {
        // Case 1: 0 <= m <= 1 (Original case)
        if (dy <= dx)   
        {
            F = dy2 - dx;    // initial F

            x = firstX;
            y = firstY;
            while (x <= secondX)
            {
                setPixel(x, y);
                if (F <= 0)
                {
                    F += dy2;
                }
                else
                {
                    y++;
                    F += dy2_minus_dx2;
                }
                x++;
            }
        }
        // Case 2: 1 < m < INF (Mirror about y=x line
        // replace all dy by dx and dx by dy)
        else
        {
            F = dx2 - dy;    // initial F

            y = firstY;
            x = firstX;
            while (y <= secondY)
            {
                setPixel(x, y);
                if (F <= 0)
                {
                    F += dx2;
                }
                else
                {
                    x++;
                    F -= dy2_minus_dx2;
                }
                y++;
            }
        }
    }
    else    // m < 0
    {
        // Case 3: -1 <= m < 0 (Mirror about x-axis, replace all dy by -dy)
        if (dx >= -dy)
        {
            F = -dy2 - dx;    // initial F

            x = firstX;
            y = firstY;
            while (x <= secondX)
            {
                setPixel(x, y);
                if (F <= 0)
                {
                    F -= dy2;
                }
                else
                {
                    y--;
                    F -= dy2_plus_dx2;
                }
                x++;
            }
        }
        // Case 4: -INF < m < -1 (Mirror about x-axis and mirror 
        // about y=x line, replace all dx by -dy and dy by dx)
        else    
        {
            F = dx2 + dy;    // initial F

            y = firstY;
            x = firstX;
            while (y >= secondY)
            {
                setPixel(x, y);
                if (F <= 0)
                {
                    F += dx2;
                }
                else
                {
                    x++;
                    F += dy2_plus_dx2;
                }
                y--;
            }
        }
    }
}

void CatPictureApp::setPixel(int x, int y)
{
	uint8_t* dataArray = (*mySurface).getData();
	c.r = 0;
	c.g = 0;
	c.b = 0;

	for(int i = 0; i < AppWidth; i++){
		if(i == y){
			for(int j = 0; j < AppHeight; j++){
				if(j == x){
					int offset = 3*(j + i*TextureSize);
					dataArray[offset] = c.r;
					dataArray[offset+1] = c.g;
					dataArray[offset+2] = c.b;
				}
			}
		}
	}
}

void CatPictureApp::addTriangle(int firstX, int firstY, int secondX, int secondY, int thirdX, int thirdY){
	addLine(firstX, firstY, secondX, secondY);
	addLine(firstX, firstY, thirdX, thirdY);
	addLine(secondX, secondY, thirdX, thirdY);
}

void CatPictureApp::setup()
{
	
	//put dataArray construct here
	mySurface = new Surface(TextureSize, TextureSize, false);
	setDefaultColor();
}

void CatPictureApp::mouseDown( MouseEvent event )
{

}

void CatPictureApp::update()
{
	uint8_t* dataArray = (*mySurface).getData();
	//addRectangle(dataArray, 20, 30, 300, 300);
	//addRectangle(dataArray, 20, 20, 400, 400);
	//addCircle(dataArray, 10, 200, 200);
	//addCircle(dataArray, 20, 200, 200);
	//addLine(20, 40, 700, 20);
	addTriangle(20,20,400,400,200,400);
}

void CatPictureApp::draw()
{
	gl::draw(*mySurface);
}

CINDER_APP_BASIC( CatPictureApp, RendererGl )
