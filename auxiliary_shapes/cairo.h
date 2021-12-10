#pragma once

#include <cairo.h>

extern cairo_t *_cr; // context

void start_cairo();

void cleairo();

void cairo2fb();

void end_cairo();
