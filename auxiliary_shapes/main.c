#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include "./cairo.h"
#include "./fb.h"
#include "./rsvg.h"
#include "./dict.h"

static void test(){
  cleairo();
  rsvg2cairo(0,rw,"Cjrm-a0.svg");
  rsvg2cairo(0,rw,"Cjrm-a0.svg");
  rsvg2cairo(0,rw,"Cjrm-a0.svg");
  rsvg2cairo(0,rw,"Cjrm-a0.svg");
  rsvg2cairo(0,rw,"Cjrm-a0.svg");
  rsvg2cairo(0,rw,"Cjrm-a0.svg");
  rsvg2cairo(0,rw,"Cjrm-a0.svg");
  current_x=0;
  rsvg2cairo(rw,ew,"Cjem-a0-1.svg");
  rsvg2cairo(rw,ew,"Cjem-a0-1.svg");
  rsvg2cairo(rw,ew,"Cjem-a0-1.svg");
  rsvg2cairo(rw,ew,"Cjem-a0-1.svg");
  rsvg2cairo(rw,ew,"Cjem-a0-1.svg");
  cairo2fb();
}

int main(){

  start_fb();
  start_cairo();
  srand(time(NULL));
  assert(0==chdir("/home/darren/cangjie/auxiliary_shapes/svg/"));

  // test();

  // console_codes(4) DEC Private Mode (DECSET/DECRST) sequences
  // "\e[?25h"

  for(;;){
    cleairo();
    const int i=rand()%dN;
    for(const char *const *svg=d[i].r;*svg;++svg)
      rsvg2cairo(0,rw,*svg);
    current_x=0;
    for(const char *const *svg=d[i].e;*svg;++svg)
      rsvg2cairo(rw,ew,*svg);
    cairo2fb();
    getchar();
  }

  end_cairo();
  end_fb();
  return 0;

}
