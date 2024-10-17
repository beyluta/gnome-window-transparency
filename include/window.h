#ifndef WINDOW_H
#define WINDOW_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>

#define OPAQUE 0xffffffff

void set_transparency_for_all_windows(xcb_connection_t *connection,
                                      xcb_window_t exclude, uint32_t opacity);
xcb_window_t get_active_window(xcb_connection_t *connection);
void set_window_opacity(xcb_connection_t *connection, xcb_window_t window,
                        uint32_t opacity);

#endif