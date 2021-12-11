#include <string.h> // memcpy
#include <cairo.h>
#include <stdio.h>
#include <math.h> // NAN
#include "./cairo.h"
#include "./color.h"
#include "./fb.h"

#ifndef CAIRO_HAS_IMAGE_SURFACE
#error
#endif

/* PRIVATE */

// #define CRD ((const Color(*)[_stride/ColorBytes])(_data))
static cairo_surface_t *_surface=NULL;
static const unsigned char *_data=NULL;
static int _stride=-1; // in bytes, not pixels

/* PUBLIC */

__u32 current_x=0; // hight x value reached so far

cairo_t *_cr=NULL; // context

void start_cairo(){

  // create surface
  assert((_surface=cairo_image_surface_create(ColorCairo,fb.w,fb.h)));
  assert(CAIRO_STATUS_SUCCESS==cairo_surface_status(_surface));
  assert(ColorCairo==cairo_image_surface_get_format(_surface)
    &&(int64_t)fb.w==cairo_image_surface_get_width(_surface)
    &&(int64_t)fb.h==cairo_image_surface_get_height(_surface));
  assert((int64_t)fb.w<(_stride=cairo_image_surface_get_stride(_surface)));
  fprintf(stderr,"cairo::stride %d bytes\n",_stride);
  _data=cairo_image_surface_get_data(_surface);
  assert(0==_stride%ColorBytes);

  // create context
  assert((_cr=cairo_create(_surface)));
  cairo_pattern_t *const pat=cairo_get_source(_cr);
  assert(pat);
  assert(CAIRO_PATTERN_TYPE_SOLID==cairo_pattern_get_type(pat));
  double red=NAN, green=NAN, blue=NAN, alpha=NAN;
  assert(CAIRO_STATUS_SUCCESS==cairo_pattern_get_rgba(pat,&red,&green,&blue,&alpha));
  assert( 0.0==red && 0.0==green && 0.0==blue && 0.0+1.0==alpha );

  // 1:1
  cairo_scale(_cr,1.0,1.0);

}

void cleairo(){
  current_x=0;
  assert(_cr);
  cairo_set_source_rgb(_cr,1.0,1.0,1.0);
  cairo_paint(_cr);
}

void cairo2fb(){
  cairo_surface_flush(_surface);
  // for(int r=0;r<A;++r){
  //   printf("\e[2m" "%02d # " "\e[0m",r);
  //   for(int c=0;c<A;++c){
  //     if(0==CRD[r][c])putchar('.');
  //     // else {printf("\n%X\n",DAT[r][c]);exit(0);}
  //     // MSB alphaFF red00 greenFF blue00 LSB
  //     else if(0xFF00FF00==CRD[r][c])putchar('O');
  //     else assert(0);
  //   }
  //   printf("\e[2m"     " # " "\e[0m");
  //   puts("");
  // }
  for(__u32 r=0;r<fb.h;++r){
    memcpy(fb._d+fb._s*r, _data+_stride*r, fb.w*ColorBytes);
  }
}

void end_cairo(){
  _stride=-1;
  cairo_destroy(_cr);_cr=NULL;
  _data=NULL;
  cairo_surface_destroy(_surface);_surface=NULL;
}
