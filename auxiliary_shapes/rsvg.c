#include <stdio.h>
#include <librsvg/rsvg.h>
#include <assert.h>
#include <math.h>
#include <stdint.h>

#ifndef CAIRO_HAS_IMAGE_SURFACE
#error
#endif

#define FMT_IMG CAIRO_FORMAT_ARGB32
#define FMT_T uint32_t
#define FMT_BITS 32
#define FMT_BYTES 4
static_assert(FMT_BYTES==sizeof(FMT_T));

static_assert(2==LIBRSVG_MAJOR_VERSION);
static_assert(52<=LIBRSVG_MINOR_VERSION);

static void cairo(){

  const int A=30;

  cairo_surface_t *surface=NULL;
  assert((surface=cairo_image_surface_create(FMT_IMG,A,A)));
  assert(CAIRO_STATUS_SUCCESS==cairo_surface_status(surface));
  assert(
    FMT_IMG==cairo_image_surface_get_format(surface)
    &&A==cairo_image_surface_get_width(surface)
    &&A==cairo_image_surface_get_height(surface)
  );

  const int stride=cairo_image_surface_get_stride(surface);
  assert(A<stride);
  printf("stride %d bytes\n",stride);
  const unsigned char *data=cairo_image_surface_get_data(surface);
  assert(0==stride%FMT_BYTES);
  #define DAT ((const FMT_T (*)[stride/FMT_BYTES])(data))

  cairo_t *cr=NULL; // the cr
  assert((cr=cairo_create(surface)));
  cairo_scale(cr,(double)A,(double)A);
  cairo_set_line_width(cr,0.1);

  cairo_pattern_t *pat=cairo_get_source(cr);
  assert(pat);
  assert(CAIRO_PATTERN_TYPE_SOLID==cairo_pattern_get_type(pat));
  double red=NAN, green=NAN, blue=NAN, alpha=NAN;
  assert(CAIRO_STATUS_SUCCESS==cairo_pattern_get_rgba(pat,&red,&green,&blue,&alpha));
  assert( 0.0==red && 0.0==green && 0.0==blue && 0.0+1.0==alpha );

  cairo_set_source_rgb(cr,1.0,1.0,0.0);
  cairo_rectangle(cr,0.25,0.25,0.5,0.5);
  cairo_stroke(cr);

  cairo_surface_flush(surface);
  for(int r=0;r<A;++r){
    printf("\e[2m" "%02d # " "\e[0m",r);
    for(int c=0;c<A;++c){
      if(0==DAT[r][c])putchar('.');
      // else {printf("\n%X\n",DAT[r][c]);exit(0);}
      // MSB alphaFF redFF greenFF blue00 LSB
      else if(0xFFFFFF00==DAT[r][c])putchar('O');
      else assert(0);
    }
    printf("\e[2m"     " # " "\e[0m");
    puts("");
  }

  cairo_destroy(cr);cr=NULL;
  cairo_surface_destroy(surface);surface=NULL;

  puts("done");

}

static void load(const char *const s){

  assert(s&&s[0]);
  puts(s);

  // MAX() defined in /usr/include/glib-2.0/glib/gmacros.h
  void *_=alloca(MAX(sizeof(RsvgLength),sizeof(gdouble)));

  RsvgHandle *h=NULL;
  GError *e=NULL;
  assert((h=rsvg_handle_new_from_file(s,&e))&&(!e));
  assert(G_IS_OBJECT(h)&&G_OBJECT_TYPE_NAME(h)&&0==strcmp("RsvgHandle",G_OBJECT_TYPE_NAME(h)));

  // file:///usr/share/gtk-doc/html/gobject/gobject-The-Base-Object-Type.html#g-object-get-property
  // assert(2==GLIB_MAJOR_VERSION&&60<=GLIB_MINOR_VERSION||2<GLIB_MAJOR_VERSION);
  // GValue v=G_VALUE_INIT;
  // assert(!G_IS_VALUE(&v)&&!G_VALUE_TYPE_NAME(&v));
  // g_object_get_property(G_OBJECT(h),"base_uri",&v);assert(G_VALUE_HOLDS_STRING(&v)&&g_value_get_string(&v)&&0==strcmp("file://"SVG,g_value_get_string(&v)));g_value_unset(&v);
  // g_object_get_property(G_OBJECT(h),"desc",&v);    assert(G_VALUE_HOLDS_STRING(&v)&&!g_value_get_string(&v));g_value_unset(&v);
  // g_object_get_property(G_OBJECT(h),"dpi-x",&v);   assert(G_VALUE_HOLDS_DOUBLE(&v));
  // g_object_get_property(o,"?",&v);puts(X_g_value_get_string(&v));
  // v=(GValue){};

  gchar *base_uri=NULL;
  gdouble dpi_x=0.0;
  gdouble dpi_y=0.0;
  gint flags=-1;
  g_object_get(G_OBJECT(h),
    "base_uri",   &base_uri,
    // "desc"     deprecated
    "dpi-x",      &dpi_x,
    "dpi-y",      &dpi_y,
    // "em"       deprecated
    // "ex"       deprecated
    "flags",      &flags,
    // "height",  deprecated
    // "metadata" deprecated
    // "title",   deprecated
    // "width",   deprecated
  NULL);

  assert(base_uri&&0==strncmp("file://",base_uri,7)&&0==strcmp(s,base_uri+7));
  g_free(base_uri);base_uri=NULL;
  assert(0==90.0-dpi_x);
  assert(0==90.0-dpi_y);
  // printf("resolution (dots per inch) %.1lfx%.1lf\n",dpi_x,dpi_y);

  gboolean hw=TRUE, hh=TRUE, hv=FALSE;
  // RsvgLength _={}; // ow={}, oh={};
  RsvgRectangle ov={};
  // rsvg_handle_get_intrinsic_dimensions(h,&hw,&ow,&hh,&oh,&hv,&ov);
  rsvg_handle_get_intrinsic_dimensions(h,&hw,_,&hh,_,&hv,&ov);
  assert((!hw)&&(!hh)&&hv);
  assert(0==45.0-ov.x);
  assert(0.0001>fabs(-33.8-ov.y));
  assert(0==150-ov.width);
  assert(0==150-ov.height);
  // printf("+-------+----Y=%.3lf\n",ov.y);
  // printf("|       |\n");
  // printf("|       |%.3lf\n",ov.height);
  // printf("|       |\n");
  // printf("+-------+\n");
  // printf("| %.3lf\n",ov.width);
  // printf("|\n");
  // printf("X=%.3lf\n",ov.x);

  assert(FALSE==rsvg_handle_get_intrinsic_size_in_pixels(h,_,_));

  // document <- layer <- element

  cairo_t *cr=NULL;

  assert(rsvg_handle_render_document(h,cr,&(RsvgRectangle){.x=0,.y=0,.width=100,.height=100},&e));

  assert(!e);

  g_clear_object(&h);

}

int main(){

  // puts("XxY");
  // load("/home/darren/cangjie/auxiliary_shapes/svg/Cjrm-a0.svg");
  // load("/home/darren/cangjie/auxiliary_shapes/svg/Cjem-a0-1.svg");

  cairo();


  return 0;

}