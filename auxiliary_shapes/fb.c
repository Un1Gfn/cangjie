#include <assert.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "./fb.h"
#include "./color.h"

/* PRIVATE */

static int _fd=-1;

static __u32 _smem_len=0;

static void f_f_s(){

  struct fb_fix_screeninfo F={};
  assert(0==ioctl(_fd,FBIOGET_FSCREENINFO,&F));

  // assert(0==strcmp("i915""drm""fb",F.id));
  // puts(F.id);
  assert(0==strcmp("i915",F.id));

  assert(0==F.smem_start);

  assert(FB_TYPE_PACKED_PIXELS==F.type);
  assert(!F.type_aux);

  assert(FB_VISUAL_TRUECOLOR==F.visual);

  assert(1==F.xpanstep);
  assert(1==F.ypanstep);
  assert(0==F.ywrapstep);

  assert(0==F.line_length%ColorBytes);
  fb._s=F.line_length;
  printf("fb::stride %u bytes %u pixels\n",fb._s,fb._s/ColorBytes);
  assert(1366<fb._s/ColorBytes&&fb._s/ColorBytes<1366+1366); // hard-code

  _smem_len=F.smem_len;
  assert(0==_smem_len%fb._s);
  fb.h=_smem_len/fb._s;
  assert(768==fb.h); // hard-code

  assert(0==F.mmio_start);
  assert(0==F.mmio_len);
  assert(0==F.accel);
  assert(0==(FB_CAP_FOURCC&F.capabilities));
  assert(0==F.capabilities);
  assert(0==F.reserved[0]);
  assert(0==F.reserved[1]);

}

static void f_v_s(){

  struct fb_var_screeninfo V={};
  assert(0==ioctl(_fd,FBIOGET_VSCREENINFO,&V));

  assert(0==V.xoffset);
  assert(0==V.yoffset);
  assert(fb.h==V.yres);
  assert(fb.h==V.yres_virtual);
  assert(V.xres==V.xres_virtual);
  fb.w=V.xres;
  assert(1366==fb.w); // hard-code

  assert(32==V.bits_per_pixel);
  assert(0==V.grayscale);

  assert(16==V.red.offset
        &&8==V.green.offset
        &&0==V.blue.offset
        &&8==V.red.length
        &&8==V.green.length
        &&8==V.blue.length
        &&0==V.red.msb_right
        &&0==V.green.msb_right
        &&0==V.blue.msb_right);
  
  // printf("%u %u %u\n",
  //        V.transp.offset,
  //        V.transp.length,
  //        V.transp.msb_right);
  assert(0==V.transp.offset
       &&0==V.transp.length
       &&0==V.transp.msb_right);

  assert(!V.nonstd);

  assert(FB_ACTIVATE_NOW==V.activate);
  
  // printf("%0umm x %umm\n",V.height,V.width);
  assert(160==V.height); // hard-code
  assert(280==V.width); // hard-code

  assert(1==V.accel_flags);

  assert(0==V.pixclock
       &&0==V.left_margin
       &&0==V.right_margin
       &&0==V.upper_margin
       &&0==V.lower_margin
       &&0==V.hsync_len
       &&0==V.vsync_len
       &&0==V.sync
       &&0==V.vmode
       &&0==V.rotate
       &&0==V.colorspace);

  static_assert(16==sizeof(V.reserved));
  for(size_t i=0;i<sizeof(V.reserved)/sizeof(V.reserved[0]);++i)
    assert(0==V.reserved[i]);

}

/* PUBLIC */

struct _fb_t fb={};

void start_fb(){
  assert(!getenv("DISPLAY"));assert(!getenv("WAYLAND_DISPLAY"));assert(!getenv("GDK_BACKEND"));assert(!getenv("QT_QPA_PLATFORM"));
  const char *const t=getenv("XDG_SESSION_TYPE");
  assert(t&&0==strcmp("tty",t));

  assert(3<=(_fd=open("/dev/fb0",O_RDWR)));

  f_f_s();
  f_v_s();

  fb._d=mmap(NULL,_smem_len,PROT_READ|PROT_WRITE,MAP_SHARED,_fd,0);
  static_assert(NULL==(char*)MAP_FAILED+1);
  assert((fb._d)&&fb._d!=MAP_FAILED);

}

void end_fb(){
  assert(0==munmap(fb._d,_smem_len));fb._d=NULL;
  close(_fd);
  _fd=-1;
}
