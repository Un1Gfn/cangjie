#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include "./cairo.h"
#include "./fb.h"
#include "./rsvg.h"
#include "./dict.h"
#include "./termios.h"

// static void test(){
//   cleairo();
//   rsvg2cairo(0,rw,"Cjrm-a0.svg");
//   rsvg2cairo(0,rw,"Cjrm-a0.svg");
//   rsvg2cairo(0,rw,"Cjrm-a0.svg");
//   rsvg2cairo(0,rw,"Cjrm-a0.svg");
//   rsvg2cairo(0,rw,"Cjrm-a0.svg");
//   rsvg2cairo(0,rw,"Cjrm-a0.svg");
//   rsvg2cairo(0,rw,"Cjrm-a0.svg");
//   current_x=0;
//   rsvg2cairo(rw,ew,"Cjem-a0-1.svg");
//   rsvg2cairo(rw,ew,"Cjem-a0-1.svg");
//   rsvg2cairo(rw,ew,"Cjem-a0-1.svg");
//   rsvg2cairo(rw,ew,"Cjem-a0-1.svg");
//   rsvg2cairo(rw,ew,"Cjem-a0-1.svg");
//   cairo2fb();
// }

int main(){

  start_fb();
  start_cairo();
  start_termios();
  srand(time(NULL));
  assert(0==chdir("/home/darren/cangjie/auxiliary_shapes/svg/"));

  // test();

  for(;;){
    cleairo();
    const int i=rand()%dN;
    for(const char *const *svg=d[i].r;*svg;++svg)
      rsvg2cairo(0,rw,*svg);
    current_x=0;
    for(const char *const *svg=d[i].e;*svg;++svg)
      rsvg2cairo(rw,ew,*svg);
    cairo2fb();
    int c='\0';
    lb: c=getchar();
    if('9'==c||'0'==c||'Q'==c/*||'q'==c*/){break;}
    if(isalpha(c)&&d[i].k==tolower(c)){continue;}
    // if('5'==c){cairo2fb();}
    // fprintf(stderr,"%d\n",c);
    cairo2fb();
    goto lb;
  }

  end_termios();
  end_cairo();
  zero2fb();
  end_fb();
  return 0;

}
