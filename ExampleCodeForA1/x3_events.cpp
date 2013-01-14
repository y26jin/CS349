/*
 * Commands to compile and run
 *
 * g++ -o x3_events x3_events.cpp -L/usr/X11R6/lib -lX11 -lstdc++
 * ./x3_events
 * 
 * Depending on the machine, the -L option and -lstdc++ may
 * not be needed.
 */

#include <iostream>
#include <list>
#include <cstdlib>
#include <sys/time.h>
#include <math.h>
#include <stdio.h>

/*
 * Header files for X functions
 */
#include <X11/Xlib.h>
#include <X11/Xutil.h>

using namespace std;
 
const int Border = 5;
const int BufferSize = 10;
const int FPS = 30;

/*
 * Information to draw on the window.
 */
struct XInfo {
	Display	 *display;
	int		 screen;
	Window	 window;
	GC		 gc[3];
};


/*
 * Function to put out a message on error exits.
 */
void error( string str ) {
  cerr << str << endl;
  exit(0);
}


/*
 * An abstract class representing displayable things. 
 */
class Displayable {
	public:
		virtual void paint(XInfo &xinfo) = 0;
};       


/*
 * Display some text where the user clicked the mouse.
 */
class Eye : public Displayable {
	public:
		virtual void paint(XInfo &xinfo) {
			if (visible){
				XFillArc(xinfo.display, xinfo.window, xinfo.gc[1], x, y, width, height, 0, 360*64);
				double cx = x + width/2;
				double cy = y + height/2;
							
				double alpha;
				if (look_x > cx ) alpha = atan((look_y - cy) / (look_x - cx) );
				else alpha = M_PI - atan((look_y - cy) / (cx - look_x) );
				XFillArc(xinfo.display, xinfo.window, xinfo.gc[0], 
					cx + cos(alpha)*width/4 - width/8,
					cy + sin(alpha)*height/4 - height/8,
					width/4, height/4,
					0,
					360*64);
			}
		}
		
		void lookat(int x, int y) {
			look_x = x;
			look_y = y;
		}
		
		void setInvisible(){
			visible = false;
		}
		
		void setVisible(){
			visible = true;
		}
      
	// constructor
	Eye(int x, int y, int width, int height):x(x), y(y), width(width), height(height)  {
		look_x = x+width/2;
		look_y = y+height/2;
		visible = true;
	}
      
	private:
		int x;
		int y;
		int width;
		int height;
		int look_x;
		int look_y;
		bool visible;
};



list<Displayable *> dList;           // list of Displayables
Eye left_eye(100, 100, 100, 200);
Eye right_eye(225, 100, 100, 200);


/*
 * Initialize X and create a window
 */
void initX(int argc, char *argv[], XInfo &xInfo) {
	XSizeHints hints;
	unsigned long white, black;

   /*
	* Display opening uses the DISPLAY	environment variable.
	* It can go wrong if DISPLAY isn't set, or you don't have permission.
	*/	
	xInfo.display = XOpenDisplay( "" );
	if ( !xInfo.display )	{
		error( "Can't open display." );
	}
	
   /*
	* Find out some things about the display you're using.
	*/
	xInfo.screen = DefaultScreen( xInfo.display );

	white = XWhitePixel( xInfo.display, xInfo.screen );
	black = XBlackPixel( xInfo.display, xInfo.screen );

	hints.x = 100;
	hints.y = 100;
	hints.width = 400;
	hints.height = 500;
	hints.flags = PPosition | PSize;

	xInfo.window = XCreateSimpleWindow( 
		xInfo.display,				// display where window appears
		DefaultRootWindow( xInfo.display ), // window's parent in window tree
		hints.x, hints.y,			// upper left corner location
		hints.width, hints.height,	// size of the window
		Border,						// width of window's border
		black,						// window border colour
		white );					// window background colour
		
	XSetStandardProperties(
		xInfo.display,		// display containing the window
		xInfo.window,		// window whose properties are set
		"x3_animation",		// window's title
		"Animate",			// icon's title
		None,				// pixmap for the icon
		argv, argc,			// applications command line args
		&hints );			// size hints for the window

	/* 
	 * Create Graphics Contexts
	 */
	int i = 0;
	xInfo.gc[i] = XCreateGC(xInfo.display, xInfo.window, 0, 0);
	XSetForeground(xInfo.display, xInfo.gc[i], BlackPixel(xInfo.display, xInfo.screen));
	XSetBackground(xInfo.display, xInfo.gc[i], WhitePixel(xInfo.display, xInfo.screen));
	XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
	XSetLineAttributes(xInfo.display, xInfo.gc[i],
	                     1, LineSolid, CapButt, JoinRound);

	// Reverse Video
	i = 1;
	xInfo.gc[i] = XCreateGC(xInfo.display, xInfo.window, 0, 0);
	XSetForeground(xInfo.display, xInfo.gc[i], WhitePixel(xInfo.display, xInfo.screen));
	XSetBackground(xInfo.display, xInfo.gc[i], BlackPixel(xInfo.display, xInfo.screen));
	XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
	XSetLineAttributes(xInfo.display, xInfo.gc[i],
	                     1, LineSolid, CapButt, JoinRound);


	XSelectInput(xInfo.display, xInfo.window, 
		ButtonPressMask | ButtonReleaseMask | KeyPressMask | PointerMotionMask | ExposureMask);

	/*
	 * Put the window on the screen.
	 */
	XMapRaised( xInfo.display, xInfo.window );
	
	XFlush(xInfo.display);
	sleep(2);	// let server get set up before sending drawing commands
}

/*
 * Function to repaint a display list
 */
void repaint( XInfo &xinfo) {
	list<Displayable *>::const_iterator begin = dList.begin();
	list<Displayable *>::const_iterator end = dList.end();

	//XClearWindow( xinfo.display, xinfo.window );
	
	XWindowAttributes windowInfo;
	XGetWindowAttributes(xinfo.display, xinfo.window, &windowInfo);
	unsigned int height = windowInfo.height;
	unsigned int width = windowInfo.width;

	XFillRectangle(xinfo.display, xinfo.window, xinfo.gc[0], 0, 0, width, height);
	while( begin != end ) {
		Displayable *d = *begin;
		d->paint(xinfo);
		begin++;
	}
	XFlush( xinfo.display );
}


void handleButtonPress(XInfo &xinfo, XEvent &event) {
	printf("Got button press!\n");
	// naive wink:  Set left eye to invisible
	left_eye.setInvisible();
}

void handleButtonRelease(XInfo &xinfo, XEvent &event) {
	printf("Got button release!\n");
	// naive wink:  Set left eye to visible
	left_eye.setVisible();
}

void handleKeyPress(XInfo &xinfo, XEvent &event) {
	KeySym key;
	char text[BufferSize];
	
	/*
	 * Exit when 'q' is typed.
	 * This is a simplified approach that does NOT use localization.
	 */
	int i = XLookupString( 
		(XKeyEvent *)&event, 	// the keyboard event
		text, 					// buffer when text will be written
		BufferSize, 			// size of the text buffer
		&key, 					// workstation-independent key symbol
		NULL );					// pointer to a composeStatus structure (unused)
	if ( i == 1) {
		printf("Got key press -- %c\n", text[0]);
		if (text[0] == 'q') {
			error("Terminating normally.");
		}
	}
}

void handleMotion(XInfo &xinfo, XEvent &event) {
	left_eye.lookat(event.xbutton.x, event.xbutton.y);
	right_eye.lookat(event.xbutton.x, event.xbutton.y);
	
}

void eventLoop(XInfo &xinfo) {
	// Add stuff to paint to the display list
	dList.push_front(&left_eye);
	dList.push_front(&right_eye);
	
	XEvent event;
	
	repaint(xinfo);
	while( true ) {
		XNextEvent( xinfo.display, &event );
		switch( event.type ) {
			case ButtonPress:
				handleButtonPress(xinfo, event);
				break;
			case ButtonRelease:
				handleButtonRelease(xinfo, event);
				break;
			case KeyPress:
				handleKeyPress(xinfo, event);
				break;
			case MotionNotify:
				handleMotion(xinfo, event);
				break;
			case Expose:
				repaint(xinfo);
				break;
		}
	
		repaint(xinfo);
	}
}


/*
 * Start executing here.
 *	 First initialize window.
 *	 Next loop responding to events.
 *	 Exit forcing window manager to clean up - cheesy, but easy.
 */
int main ( int argc, char *argv[] ) {
	XInfo xInfo;

	initX(argc, argv, xInfo);
	eventLoop(xInfo);
	XCloseDisplay(xInfo.display);
}
