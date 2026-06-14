#define STB_IMAGE_IMPLEMENTATION
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include "../vendor/stb_image.h" 

#include <stdio.h>
#include <string.h>
#include <err.h>

typedef struct {	
	const char *jpg;
	const char *jpeg;
	const char *png;
} Filetype;

Filetype filetype = {
	.jpg  = ".jpg",
	.jpeg = ".jpeg",
	.png  = ".png"
};

static int m_Width;
static int m_Height; 
static int m_BPP;

static int image_x  = 0;
static int image_y  = 0;
static int source_x = 0;
static int source_y = 0;
static int win_Width;
static int win_Height;

static unsigned char *m_LocalBuffer;

static Display *display;
static XImage *image;

static Pixmap double_buffer;
static Window window;
static XEvent event;
static GC gc;

const char *GetFileType(const char *filename) {
	const char *dot = strrchr(filename, '.');
	if(dot) return dot;
	else err(1, "Invalid filetype. Try something called an image");
}

void inverse() {
	for (int i = 0; i < m_Width * m_Height * 4; i += 4) {
		unsigned char jakob = m_LocalBuffer[i];
		m_LocalBuffer[i]    = m_LocalBuffer[i+2];
		m_LocalBuffer[i+2]  = jakob;
	}
}

void center() {
	if (m_Width > event.xconfigure.width) {
		image_x  = 0;
		source_x = (m_Width  - event.xconfigure.width)  / 2;
	} 

	else {
		image_x = (event.xconfigure.width  - m_Width)  / 2;
		source_x = 0;
	}

	if (m_Height > event.xconfigure.height) {
		image_y  = 0;
		source_y = (m_Height - event.xconfigure.height) / 2;
	} 

	else {
		image_y = (event.xconfigure.height - m_Height) / 2;
		source_y = 0;
	}
}

void redraw() {
	XSetForeground(display, gc, BlackPixel(display, DefaultScreen(display)));
	XFillRectangle(display, double_buffer, gc, 0, 0, win_Width, win_Height);
	XPutImage(display, double_buffer, 
			gc, image, 
			source_x, source_y, 
			image_x, image_y, 
			m_Width, 
			m_Height);
	XCopyArea(display, double_buffer, window, gc,
		  0, 0, win_Width, win_Height, 0, 0);
	XFlush(display);
}

int main (int argc, char *argv[]) 
{
	if (argc == 1) {
		err(1, "Please provide a filepath to your desired image");
	}

	const char *type = GetFileType(argv[1]);
	
	if (strcmp(type, filetype.jpg) == 0);
	else if (strcmp(type, filetype.jpeg) == 0);
	else if (strcmp(type, filetype.png) == 0);
	else err(1, "Invalid filetype. Only JPEGs and PNGs are supported");	

	display = XOpenDisplay(NULL);
	if (display == NULL) err(1, "XOpenDisplay: error");

	win_Width = m_Width;
	win_Height = m_Height; //Make sure image is rendered differently depending on the window size

	
	m_LocalBuffer = stbi_load(
			argv[1], 
			&m_Width, 
			&m_Height, 
			&m_BPP, 4);

	window = XCreateSimpleWindow(
			display,
			XDefaultRootWindow(display),
			0, 0,
			m_Width, m_Height,
			0,
			0x00000000,
			0x00000000);
	
    XSetWindowBackgroundPixmap(display, window, None); //Makes sure there's no flickering when resizing the image. There is still some jiggling present when resizing from certain angles, but it's not worth fixing for now.
    XStoreName(display, window, "mage");
    inverse(); 
	
	XWindowAttributes wa = {0};	
	XGetWindowAttributes(display, window, &wa);

	image = XCreateImage(
			display,
			wa.visual,
			wa.depth,
			ZPixmap,
			0,
			m_LocalBuffer,
			m_Width,
			m_Height,
			32,
			m_Width * sizeof(int));

	gc = XCreateGC(display, window, 0, NULL);	

	Atom wm_delete_window = XInternAtom(
			display, 
			"WM_DELETE_WINDOW", 
			False);
	
	XSetWMProtocols(display, window, &wm_delete_window, 1);

	XSelectInput(display, window, 
			KeyPressMask | 
			ExposureMask |
			StructureNotifyMask);

    double_buffer =  XCreatePixmap(display, window, m_Width, m_Height, DefaultDepth(display, DefaultScreen(display)));

	XMapWindow(display, window);

	while (XNextEvent(display, &event) == 0) {
		switch (event.type) {
			case Expose:
				if (event.xexpose.count == 0) redraw();
			break;
			
			case ConfigureNotify:
				center();

				if (win_Width != event.xconfigure.width || win_Height != event.xconfigure.height) 
				{ 
				    
					win_Width  = event.xconfigure.width;
                	win_Height = event.xconfigure.height;
                	XFreePixmap(display, double_buffer);
                	
					double_buffer = XCreatePixmap(
							display, 
							window, 
							win_Width, 
							win_Height,
					DefaultDepth(display, DefaultScreen(display)));
				}
				
				redraw();
			break;

			KeySym key;
			case KeyPress:
				key = XLookupKeysym(&event.xkey, 0);

				int speed = 25;

				if (key == XK_Up)    image_y -= speed;
				if (key == XK_Down)  image_y += speed;
				if (key == XK_Left)  image_x -= speed;
				if (key == XK_Right) image_x += speed;

				if (key == XK_Return) inverse();

				redraw();
			break;

			case ClientMessage:
				if ((Atom) event.xclient.data.l[0] == wm_delete_window) {
					printf("WM_SAVE_YOUSELF\n"); // This is apart of the official ICCCM documentation. Thank you Tsoding
				return 0;
				} 
			break;
		}
	} 

	stbi_image_free(m_LocalBuffer);

    XFreePixmap(display, double_buffer);
	XUnmapWindow(display, window);
	XDestroyWindow(display, window);
	XCloseDisplay(display);
	
	return 0;
}
