#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include "./cairo.h"
#include "./fb.h"
#include "./rsvg.h"
#include "./kre.h"
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

int main(const int argc, const char *const *const argv){

  if(!(argc==2&&0==strcmp("--nohelp",argv[1]))){
    puts("press <Shift+Q> to quit");
    puts("press <letter> to guess");
    puts("run with '--nohelp' to suppress this tip");
    printf("press <Enter> to continue ..."); fflush(stdout);
    if('Q'==getchar()) exit(0);
  }

  start_fb();
  start_cairo();
  start_termios();
  srand(time(NULL));
  assert(0==chdir("svg/"));

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
    if('Q'==c)
      break;
    if(isalpha(c)&&d[i].k==tolower(c))
      continue;
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
