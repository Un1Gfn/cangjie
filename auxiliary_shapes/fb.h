// fb.h
// high-level framebuffer api

#pragma once

#include <stdint.h>
#include <linux/types.h> // __u32
#include "./color.h"

struct _fb_t {
  __u32 h; // framebuffer height
  __u32 w; // framebuffer width
  __u32 _s; // framebuffer stride (padded width)
  void *_d; // framebuffer data
  #define FBD ((Color(*)[fb._s])(fb._d))
};

extern struct _fb_t fb;

void start_fb();

void end_fb();
