#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <xcb/xcb.h>
#include "../include/window.h"

static xcb_atom_t intern_atom(xcb_connection_t *conn, const char *atom)
{
    xcb_atom_t result = XCB_NONE;
    xcb_intern_atom_reply_t *r = xcb_intern_atom_reply(conn,
            xcb_intern_atom(conn, 0, strlen(atom), atom), NULL);
    if (r)
        result = r->atom;
    free(r);
    return result;
}

int main (int argc, char **argv)
{
    float opacity = argc <= 1 ? .85 :(float)atof(argv[1]);
    uint32_t active_opacity = (unsigned long)(opacity * (float)OPAQUE);

    xcb_connection_t *conn = xcb_connect(NULL, NULL);
    if (xcb_connection_has_error(conn)) {
        printf("failed to open a connection to the X-Server\n");
        return EXIT_FAILURE;
    }

    xcb_atom_t active_window = intern_atom(conn, "_NET_ACTIVE_WINDOW");
    xcb_screen_t* screen = xcb_setup_roots_iterator(xcb_get_setup(conn)).data;

    uint32_t values[] = { XCB_EVENT_MASK_PROPERTY_CHANGE };
    xcb_change_window_attributes(
        conn,
        screen->root,
        XCB_CW_EVENT_MASK,
        values);
    xcb_flush(conn);

    xcb_window_t last_active_window;
    xcb_generic_event_t *ev;
    while ((ev = xcb_wait_for_event(conn))) {
        switch (ev->response_type & 0x7F) {
        case XCB_PROPERTY_NOTIFY:
            xcb_property_notify_event_t *e = (void *) ev;
            if (e->atom == active_window) {
                xcb_window_t active_window = get_active_window(conn);
                set_transparency_for_all_windows(conn, active_window, active_opacity);

                if (last_active_window != active_window) {
                    last_active_window = active_window;
                    set_window_opacity(conn, active_window, OPAQUE);
                }
                xcb_flush(conn);
            }
            break;
        }
        free(ev);
    }

    xcb_disconnect(conn);
    return EXIT_SUCCESS;
}