#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <unistd.h>
#include <iostream>

using namespace std;

int main(){
  Display *display;
  char *display_name = getenv("DISPLAY");
  display = XOpenDisplay(display_name);

  Window win;
  int win_x = 0, win_y = 0, win_border_width = 2, screen_num = DefaultScreen(display);
  int win_width = DisplayWidth(display, screen_num)/3;
  int win_height = DisplayHeight(display, screen_num)/3;

  win = XCreateSimpleWindow(display,
			    RootWindow(display, screen_num),
			    win_x,
			    win_y,
			    win_width,
			    win_height,
			    win_border_width,
			    BlackPixel(display, screen_num),
			    WhitePixel(display, screen_num));
  if(!win){
    cout<<"Failed to display window!"<<endl;
    exit(-1);
  }

  GC gc;
  XGCValues values;
  unsigned long valuemask = GCCapStyle | GCJoinStyle;
  gc = XCreateGC(display, win, 0, 0);

  XSetForeground(display, gc, WhitePixel(display, screen_num));
  XSetBackground(display, gc, BlackPixel(display, screen_num));
  XSetFillStyle(display, gc, FillSolid);
  XSetLineAttributes(display, gc, 2, LineSolid, CapRound, JoinRound);

  XMapWindow(display, win);
  XSync(display, False);
  sleep(3);
  XCloseDisplay(display);
  cout<<"Success to connect to X11 server"<<endl;
  return 0;

}
