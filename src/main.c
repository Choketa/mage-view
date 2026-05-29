#define STB_IMAGE_IMPLEMENTATION
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include "../vendor/stb_image.h" 

#include <stdio.h>
#include <string.h>
#include <err.h>


int main (int argc, char *argv[]) {

	int m_Width  = 0;  
	int m_Height = 0; 
	int m_BPP    = 0;    

	if (argc == 1) {
		err(1, "Please provide a filepath to your desired image");
	}

	Display *display = XOpenDisplay(NULL);
	if (display == NULL) {
		err(1, "XOpenDisplay: error");
	}

	unsigned char* m_LocalBuffer = stbi_load(
			argv[1], 
			&m_Width, 
			&m_Height, 
			&m_BPP, 4);

	Window window = XCreateSimpleWindow(
			display,
			XDefaultRootWindow(display),
			0, 0,
			m_Width, m_Height,
			0,
			0x00000000,
			0x00000000);

	void Inverse() {
		for (int i = 0; i < m_Width * m_Height * 4; i += 4) {
			unsigned char jakob = m_LocalBuffer[i];
			m_LocalBuffer[i]    = m_LocalBuffer[i+2];
			m_LocalBuffer[i+2]  = jakob;
		}
	}
	
	Inverse(); 
	
	XWindowAttributes wa = {0};	
	XGetWindowAttributes(display, window, &wa);

	XImage *image = XCreateImage(
			display,
			wa.visual,
			wa.depth,
			ZPixmap, // Load of  bullshit
			0,
			m_LocalBuffer,
			m_Width,
			m_Height,
			32,
			m_Width * sizeof(int));

	GC gc = XCreateGC(display, window, 0, NULL);	

	Atom wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(display, window, &wm_delete_window, 1);

	XSelectInput(display, window, 
			KeyPressMask | 
			ExposureMask);

	XMapWindow(display, window);

	int image_x = 0;
	int image_y = 0;

	void redraw(void) {
		XClearWindow(display, window);
		XPutImage(display, window, 
				gc, image, 
				0, 0, 
				image_x, image_y, 
				m_Width, 
				m_Height);
		XSync(display, False);
	}
	
	XEvent event;
	while (XNextEvent(display, &event) == 0) {
		switch (event.type) {
			case Expose:
				redraw();
			break;

			case KeyPress:
				KeySym key = XLookupKeysym(&event.xkey, 0);

				int speed = 25;

				if (key == XK_Up) {
					image_y -= speed;
				} else if (key == XK_Down) {
					image_y += speed;
				} else if (key == XK_Left) {
					image_x -= speed;
				} else if (key == XK_Right) {
					image_x += speed;
				}
				redraw();
			break;

			case ClientMessage:
				if ((Atom) event.xclient.data.l[0] == wm_delete_window) {
					printf("WM_SAVE_YOUSELF\n"); //TsodingDaily
					return 0;
				} 
			break;
		}
	} 

	stbi_image_free(m_LocalBuffer);

	XUnmapWindow(display, window);
	XDestroyWindow(display, window);
	XCloseDisplay(display);
	
	return 0;
}
