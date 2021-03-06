
#ifndef _DRAW_H_
#define _DRAW_H_

#ifdef __cplusplus
extern "C"
{
#endif

    void show_rgba(int width, int height);
    void show_yuyv(int width, int height);
    void show_nv12(int width, int height);
    void draw_rect(float r, float g, float b);
    void show_default(int width, int height);
    void mvptest();
    void draw_vertexs_update();
    void draw_tetrahedron();
    void draw_antialiasfiltering();
    void draw_simple();

#ifdef __cplusplus
}
#endif

#endif
