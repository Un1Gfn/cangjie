#pragma once

#include <cairo.h>
#include <linux/types.h> // __u32

extern __u32 current_x;

extern cairo_t *_cr; // context

void start_cairo();

void cleairo();

void cairo2fb();

void end_cairo();
