#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>

// void draw(char *rgb_out, int w, int h)
// {
//         int i = 0;

//         for (i = 0;i < w*h;i += 4) {
//                 rgb_out[i + 1] = 0;
//                 rgb_out[i + 2] = 0;
//                 rgb_out[i + 3] = 0;
//         }

//         return;
// }

extern int render(char *rgb_out, int nx, int ny, float camera_x, float camera_y, float camera_z);


XImage *create_ximage(Display *display, Visual *visual, int width, int height, float camera_x, float camera_y, float camera_z)
{
        char *image32 = (char *)malloc(width * height * 4);
        render(image32, width, height, camera_x, camera_y, camera_z);
        return XCreateImage(display, visual, 24,
                            ZPixmap, 0, image32,
                            width, height, 32, 0);
}

int main(int argc, char **argv)
{
        int win_b_color;
        int win_w_color;
        XEvent xev;
        Window window;
        GC gc;
        Display *display = XOpenDisplay(NULL);
        Visual *visual;
        XImage *ximage;
        KeySym key_sym;
        float camera_x = 0.0; 
        float camera_y = 0.0;
        float camera_z = 0.0;

        win_b_color = BlackPixel(display, DefaultScreen(display));
        win_w_color = BlackPixel(display, DefaultScreen(display));
        window = XCreateSimpleWindow(display,
                                DefaultRootWindow(display),
                                0, 0, 1200, 800, 0,
                                win_b_color, win_w_color);

        visual = DefaultVisual(display, 0);

        XSelectInput(display, window, ExposureMask | KeyPressMask);

        XMapWindow(display, window);
        XFlush(display);
        gc = XCreateGC(display, window, 0, NULL);
        XEvent event;
        bool exit = false;

        while (!exit) {
                int r;

                XNextEvent(display, &event);

                if (event.type == Expose)
                {
                    ximage = create_ximage(display, visual, 1200, 800, camera_x, camera_y, camera_z);
                    r = XPutImage(display, window, gc, ximage, 0, 0, 0, 0, 1200, 800);
                    continue;
                }
                else if (event.type == KeyPress) {
                    key_sym = XkbKeycodeToKeysym( display, event.xkey.keycode, 0, event.xkey.state & ShiftMask ? 1 : 0);
                    if( key_sym == XK_Escape ){
                        XDestroyWindow(display, window);
                        exit=true;
                        continue;
                    } else if (key_sym == XK_Right) {
                        camera_x += 0.1;
                    } else if (key_sym == XK_Left) {
                        camera_x -= 0.1;
                    } else if (key_sym == XK_Up) {
                        camera_y += 0.1;
                    } else if (key_sym == XK_Down) {
                        camera_y -= 0.1;
                    } else if (key_sym == XK_w) {
                        camera_z += 0.1;
                    } else if (key_sym == XK_s) {
                        camera_z -= 0.1;
                    } else {
                        continue;
                    }
                    ximage = create_ximage(display, visual, 1200, 800, camera_x, camera_y, camera_z);
                    r = XPutImage(display, window, gc, ximage, 0, 0, 0, 0, 1200, 800);
                }
        }

        return 0;
}