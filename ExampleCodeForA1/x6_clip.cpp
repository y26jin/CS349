/*
 * Header files for X functions and basic system libraries
 */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void repaint(Display* display, Window window, GC gc);
int main ( int argc, char *argv[] ) 
{
    Display*        display;            /* X display                                */
    int             screen;             /* Will reference the default screen        */
    Window          window;             /* Our simple window                        */
    GC              gc;                 /* Graphics context for drawing             */
    XSizeHints      hints;              /* For specifying size on creation          */
    int             BORDER_WIDTH = 5;   /* Window border width                      */
	unsigned long   background;         /* Background color                         */
	unsigned long   foreground;         /* Foreground color                         */
    XRectangle      clip_rect;

   /*
    * Display opening uses the DISPLAY  environment variable.
    * It can go wrong if DISPLAY isn't set, or you don't have permission.
    */  
  	display = XOpenDisplay( "" );
  	if ( !display )
    {
       printf( "Can't open display.\n" );
       exit(0);
  	}

   /*
    * Get default screen
    */
  	screen = DefaultScreen( display );

    /*
     * Get the background/foreground colors (black/white)
     */
  	background = BlackPixel( display, screen );
  	foreground = WhitePixel( display, screen );

    /*
     * Set up the window's size and location
     */
  	hints.x = 100;
  	hints.y = 100;
  	hints.width = 400;
  	hints.height = 300;
  	hints.flags = PPosition | PSize;

    /*
     * Create a simple window
     */
  	window = XCreateSimpleWindow(   display,
                                    DefaultRootWindow( display ),
                                    hints.x,
                                    hints.y,
                                    hints.width,
                                    hints.height,
                                    BORDER_WIDTH,
                                    foreground,
                                    background);

    /*
     * Set the window's basic properties
     */
  	XSetStandardProperties( display,
                            window,
                            "Window Title",
                            "Icon Title",
                            None,           /* Pixmap for icon (none used here) */
                            argv,           /* Commands to run in the window */
                            argc,
                            &hints );       /* Sizes to use for window */

   /*
    * Put the window on the screen.
    */
  	XMapRaised( display, window );

    /*
     * Send the commands to the server.
     * Without this, nothing happens!
     */
    XFlush(display);

    /*
     * Draw something on the window
     */

    /*
    * Create a graphics context to hold our drawing state information
    */
  	gc = XCreateGC (display,
                    window,
                    0,          /* GCflags for determining which parts of the GC are used */
                    0);         /* A struct for filling in GC data at initialization */

    /*
     * Set the foreground/background colors to draw with
     */
  	XSetBackground( display, gc, background );
  	XSetForeground( display, gc, foreground );

    /*
     * Indicate what events we are interested in receiving
     */
  	XSelectInput(   display,
                    window,
                    ButtonPressMask | KeyPressMask | ExposureMask );

    clip_rect.x = 0;
    clip_rect.y = 20;
    clip_rect.width = 30;
    clip_rect.height = 40;

	printf("Press a key to move the clip and redraw.\n");
	printf("Click the mouse to quit.\n");
	
    /*
     * Enter event loop
     */
    while (1) {
        XEvent event;

        /* 
         * Grab next event, process it
         */
        XNextEvent( display, &event );

    	switch( event.type ) {
          /*
           * Repaint the window on expose events.
           */
            case Expose:
                if ( event.xexpose.count == 0 ) {
                    printf("Expose event received. Redrawing\n");
                    repaint(display, window, gc);
                }
                break;

            case ButtonPress:
                /* Mouse button pressed. Exit for now */
                printf("Mouse button pressed, exiting\n");
                XCloseDisplay(display);
                exit(0);
                break;

            case KeyPress:
                clip_rect.x += 10;
                XSetClipRectangles(display, gc, 0, 0, &clip_rect, 1, Unsorted);
                repaint(display, window, gc);
                /* Key pressed. Do something */
                printf("Key pressed\n");
                break;
            }
    }
}

void repaint(Display* display, Window window, GC gc)
{
    XClearWindow( display, window );
    XDrawString(display,
                window,
                gc,
                30,
                50,
                "String test",
                strlen("String test"));
    XDrawLine(display,
                window,
                gc,
                30,
                50,
                200,
                50);
    /*
     * Don't forget to flush the display to send our commands to the server
     */
    XFlush(display);
}


