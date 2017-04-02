//dapengli final project
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef WIN32
#include <windows.h>
#pragma warning(disable:4996)
#include <GL/glew.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "BmpToTexture.cpp"


//	This is a sample OpenGL / GLUT program
//
//	The objective is to draw a 3d object and change the color of the axes
//		with a glut menu
//
//	The left mouse button does rotation
//	The middle mouse button does scaling
//	The user interface allows:
//		1. The axes to be turned on and off
//		2. The color of the axes to be changed
//		3. Debugging to be turned on and off
//		4. Depth cueing to be turned on and off
//		5. The projection to be changed
//		6. The transformations to be reset
//		7. The program to quit
//
//	Author:			Joe Graphics

// NOTE: There are a lot of good reasons to use const variables instead
// of #define's.  However, Visual C++ does not allow a const variable
// to be used as an array size or as the case in a switch( ) statement.  So in
// the following, all constants are const variables except those which need to
// be array sizes or cases in switch( ) statements.  Those are #defines.


// title of these windows:

const char *WINDOWTITLE = { "OpenGL / GLUT Sample -- Dapeng Li" };
const char *GLUITITLE   = { "User Interface Window" };


// what the glui package defines as true and false:

const int GLUITRUE  = { true  };
const int GLUIFALSE = { false };


// the escape key:

#define ESCAPE		0x1b


// initial window size:

const int INIT_WINDOW_SIZE = { 600 };


// size of the box:

const float BOXSIZE = { 2.f };



// multiplication factors for input interaction:
//  (these are known from previous experience)

const float ANGFACT = { 1. };
const float SCLFACT = { 0.005f };


// minimum allowable scale factor:

const float MINSCALE = { 0.05f };


// active mouse buttons (or them together):

const int LEFT   = { 4 };
const int MIDDLE = { 2 };
const int RIGHT  = { 1 };


// which projection:

enum Projections
{
	ORTHO,
	PERSP
};

// which button:

enum ButtonVals
{
	RESET,
	QUIT
};


// window background color (rgba):

const GLfloat BACKCOLOR[ ] = { 0., 0., 0., 1. };


// line width for the axes:

const GLfloat AXES_WIDTH   = { 3. };


// the color numbers:
// this order must match the radio button order

enum Colors
{
	RED,
	YELLOW,
	GREEN,
	CYAN,
	BLUE,
	MAGENTA,
	WHITE,
	BLACK
};

char * ColorNames[ ] =
{
	"Red",
	"Yellow",
	"Green",
	"Cyan",
	"Blue",
	"Magenta",
	"White",
	"Black"
};


// the color definitions:
// this order must match the menu order

const GLfloat Colors[ ][3] = 
{
	{ 1., 0., 0. },		// red
	{ 1., 1., 0. },		// yellow
	{ 0., 1., 0. },		// green
	{ 0., 1., 1. },		// cyan
	{ 0., 0., 1. },		// blue
	{ 1., 0., 1. },		// magenta
	{ 1., 1., 1. },		// white
	{ 0., 0., 0. },		// black
};


// fog parameters:

const GLfloat FOGCOLOR[4] = { .0, .0, .0, 1. };
const GLenum  FOGMODE     = { GL_LINEAR };
const GLfloat FOGDENSITY  = { 0.30f };
const GLfloat FOGSTART    = { 1.5 };
const GLfloat FOGEND      = { 4. };
//added how many milliseconds are in the animation cycle
const int MS_PER_CYCLE = 10000;



// non-constant global variables:

int		ActiveButton;			// current button that is down
GLuint	AxesList;				// list to hold the axes
int		AxesOn;					// != 0 means to draw the axes
int     TextureOn;              //!=0 means enalbe texture
int		DebugOn;				// != 0 means to print debugging info
int		DepthCueOn;				// != 0 means to use intensity depth cueing
GLuint	BoxList;				// object display list
int		MainWindow;				// window id for main graphics window
float	Scale;					// scaling factor
int		WhichColor;				// index into Colors[ ]
int		WhichProjection;		// ORTHO or PERSP
int		Xmouse, Ymouse;			// mouse values
float	Xrot, Yrot;				// rotation angles in degrees


//added
float Time;
bool	Distort;		// global -- true means to distort the texture
unsigned char * Texture;
int sun, mercury, venus, earth, mars, jupiter, saturn, uranus, neptune, pluto;
float rotate_angle[10];
int width, height;
int level, ncomps, border;
float plantradius[10] = { 696, 2.439, 6.052, 6.371, 3.4, 69.9, 58.2, 25.3, 24.6, 1.186 };
float plantdisfromsun[10] = { 0, 0.39, 0.72, 1.00, 1.52, 5.20, 9.58, 19.20, 30.05, 49.3 };//average distance au
//rotate by itself
//float plantrotate[10] = { 25, 59, 243, 1, 1, 0.41, 0.5, 0.72, 0.67, 0.3 }; //day
const float PI = 3.1415926;
float White[] = { 1., 1., 1., 1. };//added slides p21
int Frozen;
int LightOn = 0; // lightoff

struct point {
	float x, y, z;		// coordinates
	float nx, ny, nz;	// surface normal
	float s, t;		// texture coords
};

int		NumLngs, NumLats;
struct point *	Pts;

struct point *
	PtsPointer(int lat, int lng)
{
	if (lat < 0)	lat += (NumLats - 1);
	if (lng < 0)	lng += (NumLngs - 1);
	if (lat > NumLats - 1)	lat -= (NumLats - 1);
	if (lng > NumLngs - 1)	lng -= (NumLngs - 1);
	return &Pts[NumLngs*lat + lng];
}


// function prototypes:

void	Animate( );
void	Display( );
void	DoAxesMenu( int );
void	DoColorMenu( int );
void	DoDepthMenu( int );
void	DoDebugMenu( int );
void	DoMainMenu( int );
void	DoProjectMenu( int );
void	DoRasterString( float, float, float, char * );
void	DoStrokeString( float, float, float, float, char * );
float	ElapsedSeconds( );
void	InitGraphics( );
void	InitLists( );
void	InitMenus( );
void	Keyboard( unsigned char, int, int );
void	MouseButton( int, int, int, int );
void	MouseMotion( int, int );
void	Reset( );
void	Resize( int, int );
void	Visibility( int );

void	Axes( float );
void	HsvRgb( float[3], float [3] );
//added
char* picture[13] = { "sun.bmp", "mercury.bmp", "venus.bmp", "earth.bmp", "mars.bmp", "jupiter.bmp", "saturn.bmp", "uranus.bmp", "neptune.bmp", "pluto.bmp", "stars.bmp", "saturnring.bmp" , "moon.bmp"};
GLuint id[13];

//added from slides P21
// utility to create an array from 3 separate values:
float *
Array3(float a, float b, float c)
{
	static float array[4];
	array[0] = a;
	array[1] = b;
	array[2] = c;
	array[3] = 1.;
	return array;
}

float *
Array3_pa(float a, float b, float c)
{
	static float array[4];
	array[0] = a;
	array[1] = b;
	array[2] = c;
	array[3] = 0.1;
	return array;
}
// utility to create an array from a multiplier and an array:
float *
MulArray3(float factor, float array0[3])
{
	static float array[4];
	array[0] = factor * array0[0];
	array[1] = factor * array0[1];
	array[2] = factor * array0[2];
	array[3] = 1.;
	return array;
}


//added function
//0: sun 1:mercury  2:venus   3:earth  4:mars   5:jupiter   6:saturn   7:uranus   8: neptune  9: pluto 10:starsky 11:saturnmoonlet 12:moon
void gentexobj(char* picture[13], GLuint id[13]) {
	for (int i = 0; i < 13; i++) {
		Texture = BmpToTexture(picture[i], &width, &height);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGenTextures(1, &id[i]);
		glBindTexture(GL_TEXTURE_2D, id[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//Texture wrapping
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//Texture Filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		level = 0;//used with mip-mapping from slides
		ncomps = 3;//number of componenets in this texture
		border = 0;//width of the texture border in pixels;

				   //transfer the array of texels from the cpu to gpu and makes it the current active texture
		glTexImage2D(GL_TEXTURE_2D, level, ncomps, width, height, border, GL_RGB, GL_UNSIGNED_BYTE, Texture);
	}
}
//slides page19 draw circle

void drawcircle(float radius) {
	glColor3f(1.0, 1.0, 1.0);
	float dang = 2.*M_PI / 99.0;
	float ang = 0;
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 100; i++) {

		glVertex3f(radius*cos(ang), 0., radius*sin(ang));
		glNormal3f(-cos(ang), 0, -sin(ang));
		ang += dang;
	}
	glEnd();
}

void drawmoonlet(float radius1, float radius2) {
	float dang = 2.*M_PI / 49.0;
	float ang = 0;
	
	glBegin(GL_QUADS);
	for (int i = 0; i < 50; i++) {
		glColor3f(1., 1., 1.);
		glNormal3f(0., 1., 0.);
		glTexCoord2f(1, 0);
		glVertex3f(radius1*cos(ang), 0., radius1*sin(ang));
		glNormal3f(0., 1., 0.);
		glTexCoord2f(0, 0);
		glVertex3f(radius2*cos(ang), 0., radius2*sin(ang));
		glNormal3f(0., 1., 0.);
		glTexCoord2f(0, 1);
		glVertex3f(radius2*cos(ang+dang), 0., radius2*sin(ang+dang));
		glNormal3f(0., 1., 0.);
		glTexCoord2f(1, 1);
		glVertex3f(radius1*cos(ang + dang), 0., radius1*sin(ang + dang));

		ang += dang;
	}
	glEnd();
}
float calT(float t) {
	return 500 * sqrt(t*t*t);
}



// create the object:
void creatskybox() {
	int dx = 150, dy = 150, dz = 150;
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, id[10]);
	
	
	

	glBegin(GL_QUADS);

	glColor3f(1.,1., 1.);
	glNormal3f(0., 0., 1.);
	glTexCoord2f(0., 0.);
	glVertex3f(-dx, -dy, dz);
	glTexCoord2f(0., 1.);
	glVertex3f(dx, -dy, dz);
	glTexCoord2f(1., 1.);
	glVertex3f(dx, dy, dz);
	glTexCoord2f(1., 0.);
	glVertex3f(-dx, dy, dz);

	glNormal3f(0., 0., -1.);
	glTexCoord2f(0., 0.);
	glVertex3f(-dx, -dy, -dz);
	glTexCoord2f(0., 1.);
	glVertex3f(-dx, dy, -dz);
	glTexCoord2f(1., 1.);
	glVertex3f(dx, dy, -dz);
	glTexCoord2f(1., 0.);
	glVertex3f(dx, -dy, -dz);

	glColor3f(1., 1., 1.);
	glNormal3f(1., 0., 0.);
	glTexCoord2f(0., 0.);
	glVertex3f(dx, -dy, dz);
	glTexCoord2f(0., 1.);
	glVertex3f(dx, -dy, -dz);
	glTexCoord2f(1., 1.);
	glVertex3f(dx, dy, -dz);
	glTexCoord2f(1., 0.);
	glVertex3f(dx, dy, dz);

	glNormal3f(-1., 0., 0.);
	glTexCoord2f(0., 0.);
	glVertex3f(-dx, -dy, dz);
	glTexCoord2f(0., 1.);
	glVertex3f(-dx, dy, dz);
	glTexCoord2f(1., 1.);
	glVertex3f(-dx, dy, -dz);
	glTexCoord2f(1., 0.);
	glVertex3f(-dx, -dy, -dz);

	glColor3f(1., 1., 1.);
	glNormal3f(0., 1., 0.);
	glTexCoord2f(0., 0.);
	glVertex3f(-dx, dy, dz);
	glTexCoord2f(0., 1.);
	glVertex3f(dx, dy, dz);
	glTexCoord2f(1., 1.);
	glVertex3f(dx, dy, -dz);
	glTexCoord2f(1., 0.);
	glVertex3f(-dx, dy, -dz);

	glNormal3f(0., -1., 0.);
	glTexCoord2f(0., 0.);
	glVertex3f(-dx, -dy, dz);
	glTexCoord2f(0., 1.);
	glVertex3f(-dx, -dy, -dz);
	glTexCoord2f(1., 1.);
	glVertex3f(dx, -dy, -dz);
	glTexCoord2f(1., 0.);
	glVertex3f(dx, -dy, dz);

	glEnd();
	glDisable(GL_TEXTURE_2D);
	
}
//added from slides p29
void
SetMaterial(float r, float g, float b, float shininess)
{
	glMaterialfv(GL_BACK, GL_EMISSION, Array3(0., 0., 0.));
	glMaterialfv(GL_BACK, GL_AMBIENT, MulArray3(.4f, White));
	glMaterialfv(GL_BACK, GL_DIFFUSE, MulArray3(1., White));
	glMaterialfv(GL_BACK, GL_SPECULAR, Array3(0., 0., 0.));
	glMaterialf(GL_BACK, GL_SHININESS, 2.f);
	glMaterialfv(GL_FRONT, GL_EMISSION, Array3(0., 0., 0.));
	glMaterialfv(GL_FRONT, GL_AMBIENT, Array3(r, g, b));
	glMaterialfv(GL_FRONT, GL_DIFFUSE, Array3(r, g, b));
	glMaterialfv(GL_FRONT, GL_SPECULAR, MulArray3(.8f, White));
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}


//added from slides p30
void
SetParaellLight(int ilight, float x, float y, float z, float r, float g, float b)
{
	glLightfv(ilight, GL_POSITION, Array3_pa(x, y, z));
	glLightfv(ilight, GL_AMBIENT, Array3(1., 1., 1.));
	glLightfv(ilight, GL_DIFFUSE, Array3(r, g, b));
	glLightfv(ilight, GL_SPECULAR, Array3(r, g, b));
	glLightf(ilight, GL_CONSTANT_ATTENUATION, 1.);
	glLightf(ilight, GL_LINEAR_ATTENUATION, 0.);
	glLightf(ilight, GL_QUADRATIC_ATTENUATION, 0.);
	glEnable(ilight);
}

//added from slides p30
void
SetPointLight(int ilight, float x, float y, float z, float r, float g, float b)
{
	glLightfv(ilight, GL_POSITION, Array3(x, y, z));
	glLightfv(ilight, GL_AMBIENT, Array3(0, 0, 0));
	glLightfv(ilight, GL_DIFFUSE, Array3(r, g, b));
	glLightfv(ilight, GL_SPECULAR, Array3(r, g, b));
	glLightf(ilight, GL_CONSTANT_ATTENUATION, 0.1);
	glLightf(ilight, GL_LINEAR_ATTENUATION, 0.);
	glLightf(ilight, GL_QUADRATIC_ATTENUATION, 0.);
	glEnable(ilight);
}

void
SetSpotLight(int ilight, float x, float y, float z, float xdir, float ydir, float zdir, float r, float g, float b)
{
	glLightfv(ilight, GL_POSITION, Array3(x, y, z));
	glLightfv(ilight, GL_SPOT_DIRECTION, Array3(xdir, ydir, zdir));
	glLightf(ilight, GL_SPOT_EXPONENT, 1.);
	glLightf(ilight, GL_SPOT_CUTOFF, 45.);
	glLightfv(ilight, GL_AMBIENT, Array3(0., 0., 0.));
	glLightfv(ilight, GL_DIFFUSE, Array3(r, g, b));
	glLightfv(ilight, GL_SPECULAR, Array3(r, g, b));
	glLightf(ilight, GL_CONSTANT_ATTENUATION, 1.);
	glLightf(ilight, GL_LINEAR_ATTENUATION, 0.);
	glLightf(ilight, GL_QUADRATIC_ATTENUATION, 0.);
	glEnable(ilight);
}
// main program:

int
main( int argc, char *argv[ ] )
{
	// turn on the glut package:
	// (do this before checking argc and argv since it might
	// pull some command line arguments out)

	glutInit( &argc, argv );


	// setup all the graphics stuff:

	InitGraphics( );


	// create the display structures that will not change:

	InitLists( );


	// init all the global variables used by Display( ):
	// this will also post a redisplay

	Reset( );


	// setup all the user interface stuff:

	InitMenus( );


	// draw the scene once and wait for some interaction:
	// (this will never return)

	glutSetWindow( MainWindow );
	glutMainLoop( );


	// this is here to make the compiler happy:

	return 0;
}


// this is where one would put code that is to be called
// everytime the glut main loop has nothing to do
//
// this is typically where animation parameters are set
//
// do not call Display( ) from here -- let glutMainLoop( ) do it

//added
void
DrawPoint(struct point *p)
{
	glNormal3f(p->nx, p->ny, p->nz);
	glTexCoord2f(p->s, p->t);
	glVertex3f(p->x, p->y, p->z);
}

void
MjbSphere(float radius, int slices, int stacks)
{
	struct point top, bot;		// top, bottom points
	struct point *p;

	// set the globals:

	NumLngs = slices;
	NumLats = stacks;

	if (NumLngs < 3)
		NumLngs = 3;

	if (NumLats < 3)
		NumLats = 3;


	// allocate the point data structure:

	Pts = new struct point[NumLngs * NumLats];


	// fill the Pts structure:

	for (int ilat = 0; ilat < NumLats; ilat++)
	{
		float lat = -M_PI / 2. + M_PI * (float)ilat / (float)(NumLats - 1);
		float xz = cos(lat);
		float y = sin(lat);
		for (int ilng = 0; ilng < NumLngs; ilng++)
		{
			float lng = -M_PI + 2. * M_PI * (float)ilng / (float)(NumLngs - 1);
			float x = xz * cos(lng);
			float z = -xz * sin(lng);
			p = PtsPointer(ilat, ilng);
			p->x = radius * x;
			p->y = radius * y;
			p->z = radius * z;
			p->nx = x;
			p->ny = y;
			p->nz = z;
			if (Distort)
			{
				if (TextureOn == 0) {
					fprintf(stderr, "Texture isn't choosen, can't do distortion\n");
				}
				else {
					//glRotatef(360.*Time, 0., 1., 0.);
					p->s = (lng + M_PI) / (2.*M_PI)* (0.5+lat / M_PI) ;  
																      
					p->t = p->t = (lat + M_PI / 2.) / M_PI;
					
				}
			}
			else
			{
				p->s = (lng + M_PI) / (2.*M_PI);
				p->t = (lat + M_PI / 2.) / M_PI;
			}
		}
	}

	top.x = 0.;		top.y = radius;	top.z = 0.;
	top.nx = 0.;		top.ny = 1.;		top.nz = 0.;
	top.s = 0.;		top.t = 1.;

	bot.x = 0.;		bot.y = -radius;	bot.z = 0.;
	bot.nx = 0.;		bot.ny = -1.;		bot.nz = 0.;
	bot.s = 0.;		bot.t = 0.;


	// connect the north pole to the latitude NumLats-2:

	glBegin(GL_QUADS);
	for (int ilng = 0; ilng < NumLngs - 1; ilng++)
	{
		p = PtsPointer(NumLats - 1, ilng);
		DrawPoint(p);

		p = PtsPointer(NumLats - 2, ilng);
		DrawPoint(p);

		p = PtsPointer(NumLats - 2, ilng + 1);
		DrawPoint(p);

		p = PtsPointer(NumLats - 1, ilng + 1);
		DrawPoint(p);
	}
	glEnd();

	// connect the south pole to the latitude 1:

	glBegin(GL_QUADS);
	for (int ilng = 0; ilng < NumLngs - 1; ilng++)
	{
		p = PtsPointer(0, ilng);
		DrawPoint(p);

		p = PtsPointer(0, ilng + 1);
		DrawPoint(p);

		p = PtsPointer(1, ilng + 1);
		DrawPoint(p);

		p = PtsPointer(1, ilng);
		DrawPoint(p);
	}
	glEnd();


	// connect the other 4-sided polygons:

	glBegin(GL_QUADS);
	for (int ilat = 2; ilat < NumLats - 1; ilat++)
	{
		for (int ilng = 0; ilng < NumLngs - 1; ilng++)
		{
			p = PtsPointer(ilat - 1, ilng);
			DrawPoint(p);

			p = PtsPointer(ilat - 1, ilng + 1);
			DrawPoint(p);

			p = PtsPointer(ilat, ilng + 1);
			DrawPoint(p);

			p = PtsPointer(ilat, ilng);
			DrawPoint(p);
		}
	}
	glEnd();

	delete[] Pts;
	Pts = NULL;
}

void
Animate( )
{
	// put animation stuff in here -- change some global variables
	// for Display( ) to find:

	// force a call to Display( ) next time it is convenient:
	//added from the website
	int ms = glutGet(GLUT_ELAPSED_TIME);
	ms %= MS_PER_CYCLE;
	Time = (float)ms / (float)MS_PER_CYCLE;		// [0.,1.)

	glutSetWindow( MainWindow );
	glutPostRedisplay( );

}


// draw the complete scene:

void
Display( )
{
	if( DebugOn != 0 )
	{
		fprintf( stderr, "Display\n" );
	}


	// set which window we want to do the graphics into:

	glutSetWindow( MainWindow );


	// erase the background:

	glDrawBuffer( GL_BACK );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glEnable( GL_DEPTH_TEST );


	// specify shading to be flat:

	glShadeModel( GL_FLAT );


	// set the viewport to a square centered in the window:

	GLsizei vx = glutGet( GLUT_WINDOW_WIDTH );
	GLsizei vy = glutGet( GLUT_WINDOW_HEIGHT );
	GLsizei v = vx < vy ? vx : vy;			// minimum dimension
	GLint xl = ( vx - v ) / 2;
	GLint yb = ( vy - v ) / 2;
	glViewport( xl, yb,  v, v );


	// set the viewing volume:
	// remember that the Z clipping  values are actually
	// given as DISTANCES IN FRONT OF THE EYE
	// USE gluOrtho2D( ) IF YOU ARE DOING 2D !

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	if( WhichProjection == ORTHO )
		glOrtho( -3., 3.,     -3., 3.,     0.1, 1000. );
	else
		gluPerspective( 90., 1.,	0.1, 1000. );


	// place the objects into the scene:

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );


	// set the eye position, look-at position, and up-vector:

	gluLookAt( 0., 0., 70.,     0., 0., 0.,     0., 1., 0. );


	// rotate the scene:

	glRotatef( (GLfloat)Yrot, 0., 1., 0. );
	glRotatef( (GLfloat)Xrot, 1., 0., 0. );
	
	// uniformly scale the scene:

	//printf("scale = %.2f\n", Scale);
	if (Scale < MINSCALE)
		Scale = MINSCALE;
	glScalef((GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale);



	creatskybox();

	int ms = glutGet(GLUT_ELAPSED_TIME); // can not use Time
	
	if (LightOn) {
		glEnable(GL_LIGHTING);
		SetPointLight(GL_LIGHT0, 0, 0, 0, 1, 1, 1);
		SetParaellLight(GL_LIGHT0, 1, 1, 1, 1, 1, 1);
		SetParaellLight(GL_LIGHT0, 1, -1, 1, 1, 1, 1);
		SetParaellLight(GL_LIGHT0, 1, 1, -1, 1, 1, 1);
		

	}
	else {
		glEnable(GL_LIGHTING);
		SetPointLight(GL_LIGHT0, 0, 0, 0, 1, 1, 1);
	}
	//glDisable(GL_LIGHTING);
	
	//SetParaellLight(GL_LIGHT0, 1, 1, 1, 1, 1, 1);

	//sun
	glDisable(GL_LIGHTING);
	glPushMatrix();
	glShadeModel(GL_SMOOTH);
	SetMaterial(1.0, 1.0, 1.0, 20.0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, id[0]);
	int sun_rotation_period = 25379; //25.3799 days
	glRotatef(360*(float(ms % sun_rotation_period) / sun_rotation_period), 0., 1., 0.);//rotate by itself
	MjbSphere(4.0, 50, 50); // small #s of slices/stacks MjbSphere( float radius, int slices, int stacks ) 696
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	glEnable(GL_LIGHTING);
	//mercury
	drawcircle(0.39+5.0); // orbit
	int mercury_orbit_period = int (calT(5.39));

	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glRotatef(360 * (float(ms % mercury_orbit_period) / mercury_orbit_period), 0., 1., 0.);//
	//printf("%.3f %.3f\n", Time, 360 * Time / calT(5.39));

	glTranslatef(0.39+5.0, 0., 0.);
	int mercury_rotation_period = 58646; // 58.646 days
	glRotatef(360 * (float(ms % mercury_rotation_period) / mercury_rotation_period), 0., 1., 0.);//rotate by itself
	glShadeModel(GL_SMOOTH);
	SetMaterial(1.0, 1.0, 1.0, 20.0);
	glBindTexture(GL_TEXTURE_2D, id[1]);
	MjbSphere(0.5, 20, 20);  //MjbSphere( float radius, int slices, int stacks )
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);

	//venus
	drawcircle(0.72+7.0);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	int venus_orbit_period = int(calT(0.72+7.0));
	glRotatef(360 * (float(ms % venus_orbit_period) / venus_orbit_period), 0., 1., 0.); // orbit
	glTranslatef(0.72+7.0, 0., 0.);
	glShadeModel(GL_SMOOTH);
	SetMaterial(1.0, 1.0, 1.0, 20.0);
	int venus_rotatoin_period = 243018; // -243.0187 days
	glRotatef(-360 * (float(ms % venus_rotatoin_period) / venus_rotatoin_period), 0., 1., 0.);
	glBindTexture(GL_TEXTURE_2D, id[2]);
	MjbSphere(0.95, 50, 50);  //MjbSphere( float radius, int slices, int stacks )

	glPopMatrix();
	glDisable(GL_TEXTURE_2D);

	//earth+moon
	drawcircle(1.0 + 8.0);

	glEnable(GL_TEXTURE_2D);

	glPushMatrix();
	int earth_orbit_period = int(calT(1.+8.));
	glRotatef(360 * (float(ms % earth_orbit_period) / earth_orbit_period), 0., 1., 0.); // orbit
	glTranslatef(1.0 + 8., 0., 0.);

	glShadeModel(GL_SMOOTH);
	SetMaterial(1.0, 1.0, 1.0, 20.0);
	glBindTexture(GL_TEXTURE_2D, id[3]);

	glRotatef(-23, 1, 0, 0); // axis tilt; must be before self rotation
	int earth_rotation_period = 997; // should be int(calT(1. + 8.)/365.); // 0.997268 days
	glRotatef(360 * (float(ms % earth_rotation_period) / earth_rotation_period), 0., 1., 0.);// self rotation
	MjbSphere(1.0, 100, 100);  //MjbSphere( float radius, int slices, int stacks )
	
	//moon
	drawcircle(1.2); // moon orbit
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	int moon_orbit_period = int(calT(1.2));
	int moon_rotation_period = 27321;//27.321day
	glRotatef(360 * (float(ms % moon_orbit_period) / moon_orbit_period), 0., 1., 0.);
	glTranslatef(1.2, 0., 0.);
	glRotatef(360 * (float(ms % moon_rotation_period) / moon_rotation_period), 0., 1., 0.);// self rotation
	
	glShadeModel(GL_SMOOTH);
	SetMaterial(1.0, 1.0, 1.0, 20.0);
	glBindTexture(GL_TEXTURE_2D, id[13]);
	MjbSphere(0.2, 100, 100);  //MjbSphere( float radius, int slices, int stacks )
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	
	//moon ends
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	
	//mars
	drawcircle(1.52+9.0);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	int mars_orbit_period = int(calT(1.52+9.));
	glRotatef(360 * (float(ms % mars_orbit_period) / mars_orbit_period), 0., 1., 0.);

	glTranslatef(1.52+9.0, 0., 0.);
	glShadeModel(GL_SMOOTH);
	SetMaterial(1.0, 1.0, 1.0, 20.0);
	glBindTexture(GL_TEXTURE_2D, id[4]);
	int mars_rotation_period = 1026; //1.0259 days
	glRotatef(-25, 1, 0, 0); // axis tilt
	glRotatef(360 * (float(ms % mars_rotation_period) / mars_rotation_period), 0., 1., 0.);
	MjbSphere(0.6, 50, 50);  //MjbSphere( float radius, int slices, int stacks )
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);

	//jupiter
	drawcircle(5.20+10.);  //orbit
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	int jupiter_orbit_period = int(calT(5.20+10));
	glRotatef(360 * (float(ms % jupiter_orbit_period) / jupiter_orbit_period), 0., 1., 0.);

	glTranslatef(5.20 + 10, 0., 0.);

	glBindTexture(GL_TEXTURE_2D, id[5]);
	int jupiter_rotation_period = 413.53; // 0.41353 days
	glRotatef(3, 1, 0, 0); // axis tilt
	glRotatef(360 * (float(ms % jupiter_rotation_period) / jupiter_rotation_period), 0., 1., 0.); // self-rotate
	glShadeModel(GL_SMOOTH);
	SetMaterial(1.0, 1.0, 1.0, 20.0);
	MjbSphere(2.0, 50, 50);  //MjbSphere( float radius, int slices, int stacks )
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);

	//saturn
	drawcircle(9.58+11);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();

	int saturn_orbit_period = int(calT(9.58+11));
	glRotatef(360 * (float(ms % saturn_orbit_period) / saturn_orbit_period), 0., 1., 0.); // orbit
	glTranslatef(9.58+11, 0., 0.);
	glShadeModel(GL_SMOOTH);
	SetMaterial(1.0, 1.0, 1.0, 20.0);
	glBindTexture(GL_TEXTURE_2D, id[6]);
	
	glRotatef(-27, 1, 0, 0); // axis tilt
	int saturn_rotation_period = 444.03; // 0.44403 days
	glRotatef(360 * (float(ms % saturn_rotation_period) / saturn_rotation_period), 0, 1, 0); //self-rotate
	MjbSphere(1.9, 50, 50);  //MjbSphere( float radius, int slices, int stacks )
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);

	//drawmoonlet (band)
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glRotatef(360 * (float(ms % saturn_orbit_period) / saturn_orbit_period), 0., 1., 0.); //orbit
	glTranslatef(9.58 + 11, 0., 0.);
	glRotatef(-27, 1, 0, 0); // axis tilt
	glRotatef(360 * (float(ms % saturn_rotation_period) / saturn_rotation_period), 0, 1, 0); //self-rotat

	glShadeModel(GL_SMOOTH);
	SetMaterial(1.0, 1.0, 1.0, 20.0);
	glBindTexture(GL_TEXTURE_2D, id[11]);
	//glRotatef(360 * sin(PI*Time*0.1/0.5), 0., 1., 0.);
	drawmoonlet(2.0, 3.0);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);


	//uranus
	drawcircle(19.20+12);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();

	int uranus_orbit_period = int(calT(19.20 + 12));
	glRotatef(360 * (float(ms % uranus_orbit_period) / uranus_orbit_period), 0., 1., 0.);

	glTranslatef(19.20+12, 0., 0.);
	glShadeModel(GL_SMOOTH);
	SetMaterial(1.0, 1.0, 1.0, 20.0);
	glBindTexture(GL_TEXTURE_2D, id[7]);

	glRotatef(-98, 1, 0, 0); // axis tilt
	int uranus_rotation_period = 718.33; // -0.718 days
	glRotatef(-360 * (float(ms % uranus_rotation_period) / uranus_rotation_period), 0., 1., 0.); //self-rotate
	MjbSphere(1.5, 50, 50);  //MjbSphere( float radius, int slices, int stacks )
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	
	//neptune
	drawcircle(30.05+13);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();

	int neptune_orbit_period = int(calT(30.05+13));
	glRotatef(360 * (float(ms % neptune_orbit_period) / neptune_orbit_period), 0., 1., 0.); // orbit

	glTranslatef(30.05+13, 0., 0.);
	glShadeModel(GL_SMOOTH);
	SetMaterial(1.0, 1.0, 1.0, 20.0);
	glBindTexture(GL_TEXTURE_2D, id[8]);
	glRotatef(-28, 1, 0, 0); //axis tilt

	int neptune_rotation_period = 671.25; // 0.67125 days
	glRotatef(360 * (float(ms % neptune_rotation_period) / neptune_rotation_period), 0., 1., 0.); // self-rotate
	MjbSphere(1.4, 50, 50);  //MjbSphere( float radius, int slices, int stacks )
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);

	//pluto
	drawcircle(49.3+14);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();

	int pluto_orbit_period = int(calT(49.3 + 14));
	glRotatef(360 * (float(ms % pluto_orbit_period) / pluto_orbit_period), 0., 1., 0.);

	glTranslatef(49.3+14, 0., 0.);
	glShadeModel(GL_SMOOTH);
	SetMaterial(1.0, 1.0, 1.0, 20.0);
	glBindTexture(GL_TEXTURE_2D, id[9]);
//	glRotatef(360 * sin(PI*Time*0.1/0.3), 0., 1., 0.);
	MjbSphere(0.4, 50, 50);  //MjbSphere( float radius, int slices, int stacks )
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);

	

	// set the fog parameters:

	if( DepthCueOn != 0 )
	{
		glFogi( GL_FOG_MODE, FOGMODE );
		glFogfv( GL_FOG_COLOR, FOGCOLOR );
		glFogf( GL_FOG_DENSITY, FOGDENSITY );
		glFogf( GL_FOG_START, FOGSTART );
		glFogf( GL_FOG_END, FOGEND );
		glEnable( GL_FOG );
	}
	else
	{
		glDisable( GL_FOG );
	}


	// possibly draw the axes:

	if( AxesOn != 0 )
	{
		glColor3fv( &Colors[WhichColor][0] );
		glCallList( AxesList );
	}


	// since we are using glScalef( ), be sure normals get unitized:

	glEnable( GL_NORMALIZE );


	// draw the current object:

	glCallList( BoxList );


	// draw some gratuitous text that just rotates on top of the scene:

	glDisable( GL_DEPTH_TEST );
	glColor3f( 0., 1., 1. );
	//DoRasterString( 0., 1., 0., "world" );


	// draw some gratuitous text that is fixed on the screen:
	//
	// the projection matrix is reset to define a scene whose
	// world coordinate system goes from 0-100 in each axis
	//
	// this is called "percent units", and is just a convenience
	//
	// the modelview matrix is reset to identity as we don't
	// want to transform these coordinates

	glDisable( GL_DEPTH_TEST );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	gluOrtho2D( 0., 100.,     0., 100. );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );
	glColor3f( 1., 1., 1. );
	//DoRasterString( 5., 5., 0., "world" );


	// swap the double-buffered framebuffers:

	glutSwapBuffers( );


	// be sure the graphics buffer has been sent:
	// note: be sure to use glFlush( ) here, not glFinish( ) !

	glFlush( );
}


void
DoAxesMenu( int id )
{
	AxesOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoColorMenu( int id )
{
	WhichColor = id - RED;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDebugMenu( int id )
{
	DebugOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthMenu( int id )
{
	DepthCueOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// main menu callback:

void
DoMainMenu( int id )
{
	switch( id )
	{
		case RESET:
			Reset( );
			break;

		case QUIT:
			// gracefully close out the graphics:
			// gracefully close the graphics window:
			// gracefully exit the program:
			glutSetWindow( MainWindow );
			glFinish( );
			glutDestroyWindow( MainWindow );
			exit( 0 );
			break;

		default:
			fprintf( stderr, "Don't know what to do with Main Menu ID %d\n", id );
	}

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoProjectMenu( int id )
{
	WhichProjection = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}

void
DoTextureMenu(int id)
{
	TextureOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}

void
DoDistortion(int id)
{
	Distort = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}

// use glut to display a string of characters using a raster font:

void
DoRasterString( float x, float y, float z, char *s )
{
	glRasterPos3f( (GLfloat)x, (GLfloat)y, (GLfloat)z );

	char c;			// one character to print
	for( ; ( c = *s ) != '\0'; s++ )
	{
		glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, c );
	}
}


// use glut to display a string of characters using a stroke font:

void
DoStrokeString( float x, float y, float z, float ht, char *s )
{
	glPushMatrix( );
		glTranslatef( (GLfloat)x, (GLfloat)y, (GLfloat)z );
		float sf = ht / ( 119.05f + 33.33f );
		glScalef( (GLfloat)sf, (GLfloat)sf, (GLfloat)sf );
		char c;			// one character to print
		for( ; ( c = *s ) != '\0'; s++ )
		{
			glutStrokeCharacter( GLUT_STROKE_ROMAN, c );
		}
	glPopMatrix( );
}


// return the number of seconds since the start of the program:

float
ElapsedSeconds( )
{
	// get # of milliseconds since the start of the program:

	int ms = glutGet( GLUT_ELAPSED_TIME );

	// convert it to seconds:

	return (float)ms / 1000.f;
}


// initialize the glui window:

void
InitMenus( )
{
	glutSetWindow( MainWindow );

	int numColors = sizeof( Colors ) / ( 3*sizeof(int) );
	int colormenu = glutCreateMenu( DoColorMenu );
	for( int i = 0; i < numColors; i++ )
	{
		glutAddMenuEntry( ColorNames[i], i );
	}

	int axesmenu = glutCreateMenu( DoAxesMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthcuemenu = glutCreateMenu( DoDepthMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int debugmenu = glutCreateMenu( DoDebugMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int projmenu = glutCreateMenu( DoProjectMenu );
	glutAddMenuEntry( "Orthographic",  ORTHO );
	glutAddMenuEntry( "Perspective",   PERSP );

	int texturemenu = glutCreateMenu(DoTextureMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("World", 1);
	glutAddMenuEntry("Sky", 2);

	int distortmenu = glutCreateMenu(DoDistortion);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);


	int mainmenu = glutCreateMenu( DoMainMenu );
	glutAddSubMenu(   "Axes",          axesmenu);
	glutAddSubMenu(   "Colors",        colormenu);
	glutAddSubMenu(   "Depth Cue",     depthcuemenu);
	glutAddSubMenu(   "Projection",    projmenu );
	glutAddSubMenu(    "Texture", texturemenu);//added
	glutAddSubMenu(    "Distortion", distortmenu);//added
	glutAddMenuEntry( "Reset",         RESET );
	glutAddSubMenu(   "Debug",         debugmenu);
	glutAddMenuEntry( "Quit",          QUIT );

// attach the pop-up menu to the right mouse button:

	glutAttachMenu( GLUT_RIGHT_BUTTON );
}



// initialize the glut and OpenGL libraries:
//	also setup display lists and callback functions

void
InitGraphics( )
{
	// request the display modes:
	// ask for red-green-blue-alpha color, double-buffering, and z-buffering:

	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );

	// set the initial window configuration:

	glutInitWindowPosition( 0, 0 );
	glutInitWindowSize( INIT_WINDOW_SIZE, INIT_WINDOW_SIZE );

	// open the window and set its title:

	MainWindow = glutCreateWindow( WINDOWTITLE );
	glutSetWindowTitle( WINDOWTITLE );

	//creat texutre objects
	gentexobj(picture, id);
	
	// set the framebuffer clear values:

	glClearColor( BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3] );

	// setup the callback functions:
	// DisplayFunc -- redraw the window
	// ReshapeFunc -- handle the user resizing the window
	// KeyboardFunc -- handle a keyboard input
	// MouseFunc -- handle the mouse button going down or up
	// MotionFunc -- handle the mouse moving with a button down
	// PassiveMotionFunc -- handle the mouse moving with a button up
	// VisibilityFunc -- handle a change in window visibility
	// EntryFunc	-- handle the cursor entering or leaving the window
	// SpecialFunc -- handle special keys on the keyboard
	// SpaceballMotionFunc -- handle spaceball translation
	// SpaceballRotateFunc -- handle spaceball rotation
	// SpaceballButtonFunc -- handle spaceball button hits
	// ButtonBoxFunc -- handle button box hits
	// DialsFunc -- handle dial rotations
	// TabletMotionFunc -- handle digitizing tablet motion
	// TabletButtonFunc -- handle digitizing tablet button hits
	// MenuStateFunc -- declare when a pop-up menu is in use
	// TimerFunc -- trigger something to happen a certain time from now
	// IdleFunc -- what to do when nothing else is going on

	glutSetWindow( MainWindow );
	glutDisplayFunc( Display );
	glutReshapeFunc( Resize );
	glutKeyboardFunc( Keyboard );
	glutMouseFunc( MouseButton );
	glutMotionFunc( MouseMotion );
	glutPassiveMotionFunc( NULL );
	glutVisibilityFunc( Visibility );
	glutEntryFunc( NULL );
	glutSpecialFunc( NULL );
	glutSpaceballMotionFunc( NULL );
	glutSpaceballRotateFunc( NULL );
	glutSpaceballButtonFunc( NULL );
	glutButtonBoxFunc( NULL );
	glutDialsFunc( NULL );
	glutTabletMotionFunc( NULL );
	glutTabletButtonFunc( NULL );
	glutMenuStateFunc( NULL );
	glutTimerFunc( -1, NULL, 0 );
	glutIdleFunc( Animate );//changed

	// init glew (a window must be open to do this):

#ifdef WIN32
	GLenum err = glewInit( );
	if( err != GLEW_OK )
	{
		fprintf( stderr, "glewInit Error\n" );
	}
	else
		fprintf( stderr, "GLEW initialized OK\n" );
	fprintf( stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif

}


// initialize the display lists that will not change:
// (a display list is a way to store opengl commands in
//  memory so that they can be played back efficiently at a later time
//  with a call to glCallList( )

void
InitLists( )
{
	
	glutSetWindow( MainWindow );
	
	// create the axes:

	AxesList = glGenLists( 1 );
	glNewList( AxesList, GL_COMPILE );
		glLineWidth( AXES_WIDTH );
			Axes( 1.5 );
		glLineWidth( 1. );
	glEndList( );
}


// the keyboard callback:

void
Keyboard( unsigned char c, int x, int y )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Keyboard: '%c' (0x%0x)\n", c, c );

	switch( c )
	{
		case 'o':
		case 'O':
			WhichProjection = ORTHO;
			break;

		case 'p':
		case 'P':
			WhichProjection = PERSP;
			break;

		case 'q':
		case 'Q':
		case ESCAPE:
			DoMainMenu( QUIT );	// will not return here
			break;				// happy compiler
								//added from website
		case 'f':
		case 'F':
			Frozen = !Frozen;
			if (Frozen)
				glutIdleFunc(NULL);
			else
				glutIdleFunc(Animate);
			break;
		case 'l':
		case 'L':
			LightOn = !LightOn;
			break;
		default:
			fprintf( stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c );
	}

	// force a call to Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// called when the mouse button transitions down or up:

void
MouseButton( int button, int state, int x, int y )
{
	int b = 0;			// LEFT, MIDDLE, or RIGHT

	if( DebugOn != 0 )
		fprintf( stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y );

	
	// get the proper button bit mask:

	switch( button )
	{
		case GLUT_LEFT_BUTTON:
			b = LEFT;		break;

		case GLUT_MIDDLE_BUTTON:
			b = MIDDLE;		break;

		case GLUT_RIGHT_BUTTON:
			b = RIGHT;		break;

		default:
			b = 0;
			fprintf( stderr, "Unknown mouse button: %d\n", button );
	}


	// button down sets the bit, up clears the bit:

	if( state == GLUT_DOWN )
	{
		Xmouse = x;
		Ymouse = y;
		ActiveButton |= b;		// set the proper bit
	}
	else
	{
		ActiveButton &= ~b;		// clear the proper bit
	}
}


// called when the mouse moves while a button is down:

void
MouseMotion( int x, int y )
{
	if( DebugOn != 0 )
		fprintf( stderr, "MouseMotion: %d, %d\n", x, y );


	int dx = x - Xmouse;		// change in mouse coords
	int dy = y - Ymouse;

	if( ( ActiveButton & LEFT ) != 0 )
	{
		Xrot += ( ANGFACT*dy );
		Yrot += ( ANGFACT*dx );
	}


	if( ( ActiveButton & MIDDLE ) != 0 )
	{
		Scale += SCLFACT * (float) ( dx - dy );

		printf("middle button, scale = %.2f\n", Scale);

		// keep object from turning inside-out or disappearing:

		if( Scale < MINSCALE )
			Scale = MINSCALE;
	}

	Xmouse = x;			// new current position
	Ymouse = y;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// reset the transformations and the colors:
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene

void
Reset( )
{
	ActiveButton = 0;
	AxesOn = 1;
	DebugOn = 0;
	//added
	TextureOn = 0;
	Distort = 0;
	DepthCueOn = 0;
	Scale  = 1.0;
	WhichColor = WHITE;
	WhichProjection = PERSP;
	Xrot = Yrot = 0.;
}


// called when user resizes the window:

void
Resize( int width, int height )
{
	if( DebugOn != 0 )
		fprintf( stderr, "ReSize: %d, %d\n", width, height );

	// don't really need to do anything since window size is
	// checked each time in Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// handle a change to the window's visibility:

void
Visibility ( int state )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Visibility: %d\n", state );

	if( state == GLUT_VISIBLE )
	{
		glutSetWindow( MainWindow );
		glutPostRedisplay( );
	}
	else
	{
		// could optimize by keeping track of the fact
		// that the window is not visible and avoid
		// animating or redrawing it ...
	}
}



///////////////////////////////////////   HANDY UTILITIES:  //////////////////////////


// the stroke characters 'X' 'Y' 'Z' :

static float xx[ ] = {
		0.f, 1.f, 0.f, 1.f
	      };

static float xy[ ] = {
		-.5f, .5f, .5f, -.5f
	      };

static int xorder[ ] = {
		1, 2, -3, 4
		};

static float yx[ ] = {
		0.f, 0.f, -.5f, .5f
	      };

static float yy[ ] = {
		0.f, .6f, 1.f, 1.f
	      };

static int yorder[ ] = {
		1, 2, 3, -2, 4
		};

static float zx[ ] = {
		1.f, 0.f, 1.f, 0.f, .25f, .75f
	      };

static float zy[ ] = {
		.5f, .5f, -.5f, -.5f, 0.f, 0.f
	      };

static int zorder[ ] = {
		1, 2, 3, 4, -5, 6
		};

// fraction of the length to use as height of the characters:
const float LENFRAC = 0.10f;

// fraction of length to use as start location of the characters:
const float BASEFRAC = 1.10f;

//	Draw a set of 3D axes:
//	(length is the axis length in world coordinates)

void
Axes( float length )
{
	glBegin( GL_LINE_STRIP );
		glVertex3f( length, 0., 0. );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., length, 0. );
	glEnd( );
	glBegin( GL_LINE_STRIP );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., 0., length );
	glEnd( );

	float fact = LENFRAC * length;
	float base = BASEFRAC * length;

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 4; i++ )
		{
			int j = xorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( base + fact*xx[j], fact*xy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 5; i++ )
		{
			int j = yorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( fact*yx[j], base + fact*yy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 6; i++ )
		{
			int j = zorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( 0.0, fact*zy[j], base + fact*zx[j] );
		}
	glEnd( );

}


// function to convert HSV to RGB
// 0.  <=  s, v, r, g, b  <=  1.
// 0.  <= h  <=  360.
// when this returns, call:
//		glColor3fv( rgb );

void
HsvRgb( float hsv[3], float rgb[3] )
{
	// guarantee valid input:

	float h = hsv[0] / 60.f;
	while( h >= 6. )	h -= 6.;
	while( h <  0. ) 	h += 6.;

	float s = hsv[1];
	if( s < 0. )
		s = 0.;
	if( s > 1. )
		s = 1.;

	float v = hsv[2];
	if( v < 0. )
		v = 0.;
	if( v > 1. )
		v = 1.;

	// if sat==0, then is a gray:

	if( s == 0.0 )
	{
		rgb[0] = rgb[1] = rgb[2] = v;
		return;
	}

	// get an rgb from the hue itself:
	
	float i = floor( h );
	float f = h - i;
	float p = v * ( 1.f - s );
	float q = v * ( 1.f - s*f );
	float t = v * ( 1.f - ( s * (1.f-f) ) );

	float r, g, b;			// red, green, blue
	switch( (int) i )
	{
		case 0:
			r = v;	g = t;	b = p;
			break;
	
		case 1:
			r = q;	g = v;	b = p;
			break;
	
		case 2:
			r = p;	g = v;	b = t;
			break;
	
		case 3:
			r = p;	g = q;	b = v;
			break;
	
		case 4:
			r = t;	g = p;	b = v;
			break;
	
		case 5:
			r = v;	g = p;	b = q;
			break;
	}


	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
}
