
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stack>

#include "Frame.hpp"
#include "Widget.hpp"

Frame::Frame(int x, int y, int width, int height)
    : debug(0)
{
    abs_x = x;
    abs_y = y;
    this->width = width;
    this->height = height;
    mpNativeWindow = new NativeWindow(width, height);
    mpNativeWindow->f = this;

    init();
}

void Frame::init()
{
    /* create root Widget */
    Widget *root_wid = new Widget(0, 0, width, height);
    root_wid->SetName("root");
    root_wid->f = this;
    root_wid->SetBgColor(128, 128, 128);
    root_wid->dump();
    SetRootWidget(root_wid);
}

/*
  depth-first scan
 */
void Frame::draw(Node *node)
{
    if (!node) {
        printf("node==null\n");
        return;
    }

    stack<Node*> s;
    s.push(node);

    while(!s.empty()) {
        // pop
        Node* n = s.top(); s.pop();
        // handle
        Widget *wid = dynamic_cast<Widget *>(n);
        if (!wid->getShowStatus()) {
            continue;
        }
        wid->redraw();
        // push form the tail child node
        Node* fn = n->first_child();
        Node* sn = n->last_child();
        if (fn && sn) {
            while(sn != fn) {
                s.push(sn);
                sn = sn->prev();
            }
            s.push(sn);
        }
    }
}

void Frame::redraw()
{
    draw(rootWidget);

    GetNativeWindow()->SwapBackBuffer();
}

int Frame::SetRootWidget(Widget* wid)
{
    rootWidget = wid;

    return 0;
}

Widget* Frame::GetRootWidget()
{
    return rootWidget;
}

NativeWindow* Frame::GetNativeWindow()
{
    return mpNativeWindow;
}

void Frame::SetNativeWindow(NativeWindow* nwin)
{
    mpNativeWindow = nwin;
}

bool Frame::NeedQuit()
{
    return quit == 1;
}

Widget* Frame::findWidgetWithxy(int x, int y)
{
    bool ret = false;
    Widget* ret_wid = rootWidget;

    Node* node = rootWidget;
    stack<Node*> s;
    s.push(node);

    while(!s.empty()) {
        // pop
        Node* n = s.top(); s.pop();
        // handle
        Widget *wid = dynamic_cast<Widget *>(n);
        rect r = wid->getScreenRect();
        if (ret = r.inside(x, y)) {
            ret_wid = wid;
        }
        // push form the tail child node
        Node* fn = n->first_child();
        Node* sn = n->last_child();
        if (fn && sn) {
            while(sn != fn) {
                s.push(sn);
                sn = sn->prev();
            }
            s.push(sn);
        }
    }
    return ret_wid;
}

Widget* Frame::findWidgetWithxy(point p)
{
    return findWidgetWithxy(p.x, p.y);
}

point Frame::pointScreenToGlWindow(point p)
{
    point p1 = point(p.x, this->height - p.y);
    return p1;
}

int Frame::DispatchEvent()
{
    Event* ev = PopEvent();
    //printf("%x\n", ev);
    if (ev) {
        switch(ev->et) {
        case EVENT_TOUCH: {
            TouchEvent *tev = dynamic_cast<TouchEvent*>(ev);
            if (tev) {
                point p = pointScreenToGlWindow(point(tev->x, tev->y));
                Widget* wid = findWidgetWithxy(p);
                if (debug) {
                    printf("find [%d, %d] in %s[%d, %d, %d, %d]\n",
                           p.x, p.y,
                           wid->get_name().c_str(),
                           wid->abs_x, wid->abs_y,
                           wid->width, wid->height);
                }
                int x = p.x;
                int y = p.y;
                switch(tev->type) {
                case 0: wid->touchDownHandler(x, y); break;
                case 1: wid->touchUpHandler(x, y); break;
                case 2: wid->touchMotionHandler(x, y); break;
                default:
                    printf("unkown touch type\n");
                    break;
                }
            }
            break;
        }
        case EVENT_POINTER: {
            PointerEvent *pev = dynamic_cast<PointerEvent*>(ev);
            if (pev) {
                point p = pointScreenToGlWindow(point(pev->x, pev->y));
                Widget* wid = findWidgetWithxy(p);

                if (debug) {
                    printf("find [%d, %d] in %s[%d, %d, %d, %d]\n",
                           p.x, p.y,
                           wid->get_name().c_str(),
                           wid->abs_x, wid->abs_y,
                           wid->width, wid->height);
                }
                int v1 = pev->x;
                int v2 = pev->y;
                switch(pev->type) {
                case 0:
                    wid->pointerMotionHandler(v1, v2);
                    break;
                case 1:
                    wid->pointerButtonHandler(
                        pev->pointer.button, pev->pointer.state_w);
                    break;
                case 2:
                    wid->pointerAxisHandler(
                        pev->pointer.axis, pev->pointer.value);
                    break;
                default:
                    printf("unkown pointer type\n");
                    break;
                }
            }
            break;
        }
        default:
            break;
        }
        delete ev;
    }
    return 0;
}

Frame* Frame::PushEvent(Event* e)
{
    EventQueue.push(e);
    //printf("+:%ld\n", EventQueue.size());

    return this;
}

Event* Frame::PopEvent()
{
    Event* ret = NULL;

    EventQueue.wait_and_pop(ret);
    //printf("-:%ld\n", EventQueue.size());

    return ret;
}

bool Frame::HaveEvent()
{
    return !EventQueue.empty();
}

void* DispatchEvent_thread(void* p)
{
    Frame *f = reinterpret_cast<Frame*>(p);

    while(!f->NeedQuit()) {
        f->DispatchEvent();
    }
}

int Frame::DispatchEventRun(int flag)
{
    pthread_t pid;
    int p = pthread_create(&pid, NULL, DispatchEvent_thread, this);
}
