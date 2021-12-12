#include <stdio.h>
#include <librsvg/rsvg.h>
#include <assert.h>
#include <math.h> // NAN
#include <stdint.h>
#include "./cairo.h"
#include "./color.h"
#include "./fb.h"

static_assert(2==LIBRSVG_MAJOR_VERSION);
static_assert(52<=LIBRSVG_MINOR_VERSION);

void rsvg2cairo(const __u32 y,const __u32 l,const char *const s){

  assert(s&&s[0]);
  // puts(s);

  // MAX() defined in /usr/include/glib-2.0/glib/gmacros.h
  void *_=alloca(MAX(sizeof(RsvgLength),sizeof(gdouble)));

  RsvgHandle *h=NULL;
  GError *e=NULL;
  assert(0==access(s,R_OK));
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

  // gchar *base_uri=NULL;
  gdouble dpi_x=0.0;
  gdouble dpi_y=0.0;
  gint flags=-1;
  g_object_get(G_OBJECT(h),
    // deprecated: desc em ex height metadata title width
    // "base_uri",   &base_uri,
    "dpi-x",      &dpi_x,
    "dpi-y",      &dpi_y,
    "flags",      &flags,
  NULL);

  // assert(base_uri&&0==strncmp("file://",base_uri,7)&&0==strcmp(s,base_uri+7));
  // g_free(base_uri);base_uri=NULL;
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

  assert(_cr);
  assert(current_x+l+10<fb.w);
  assert(y+l+10<fb.h);
  assert(rsvg_handle_render_document(h,_cr,&(RsvgRectangle){
    .x=(double)current_x,
    .y=(double)y,
    .width=l,
    .height=l
  },&e));
  current_x+=l;
  assert(!e);

  // puts("A");
  // assert(CAIRO_STATUS_SUCCESS==cairo_surface_write_to_png(_surface,"hello.png"));
  // puts("B");

  // g_object_unref(h);h=NULL;
  g_clear_object(&h);

}
