#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>

Display* display;

int main(){
display = XOpenDisplay("");
 if (display == NULL) {
    printf("Cannot connect\n");
    exit (-1);
 }
 else{
   printf("Success!\n");
   /* do program stuff here */
   XCloseDisplay(display);
 }
}
