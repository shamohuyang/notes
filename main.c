
#include <stdio.h>
#include <stdlib.h>

#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include "wayland/wayland.h"
#include "egl/egl.h"
#include "gles/shader.h"
#include "gles/texture.h"
#include "gles/draw.h"
#include "cairo/cairo.h"

struct window_wayland *window;
struct egl_wayland* egl;

void* display_dispatch_thread(void* p)
{
    int ret = 0;
    while (ret != -1) {
        ret = wl_display_dispatch(window->p_wl_display);
    }
    
    return 0;
}

void* render_thread(void* p)
{
    /* egl init */
    int width = 640, height = 480;
    struct wl_egl_window* p_wl_egl_window
        = wl_egl_window_create(window->p_wl_surface, width, height);
    if (!p_wl_egl_window) {
        printf("wl_egl_window_create error\n");
    }
    egl = egl_init((EGLNativeDisplayType)window->p_wl_display,
                   (EGLNativeWindowType)p_wl_egl_window);

    init_window();
    
    while(1) {
        // Clear the color buffer
        glClearColor(1.0, 1.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        glEnable (GL_BLEND);
        /* glBlendFunc(GL_SRC_ALPHA, GL_ONE); */
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

        // Set the viewport
        glViewport(0, 0, width/2, height/2);
        show_default(width, height);

        glViewport(width/2, 0, width/2, height/2);
        show_yuyv(width, height);

        glViewport(0, height/2, width/2, height/2);
        show_rgba(width, height);

        glViewport(width/2, height/2, width/2, height/2);
        redraw_window();
        
        eglSwapBuffers(egl->display, egl->surface);
        
        //printf("render\n");
    }

    return NULL;
}

int main(int argc, char **argv)
{
    /* wayland init */
    window = wayland_init();
    pthread_t pid;
    pthread_create(&pid, NULL, display_dispatch_thread, NULL);

    render_thread(NULL);

    return 0;
}
