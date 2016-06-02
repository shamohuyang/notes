
#ifndef _WIDGET_HPP_
#define _WIDGET_HPP_

#include "node.hpp"
#include "common.hpp"

class window;

/* widget */
class widget : public Node {

    friend class window;

public:
    widget(int x, int y, int w, int h);
    int destory();
    virtual int show();
    virtual int hide();
    virtual int redraw();
    int add_sub_widget(widget*);

public:
    int abs_x, abs_y;
    int width, height;
    int is_show;
    color_rgba bg_color;
    window *win;
};

#endif
