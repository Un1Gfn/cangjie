#include <stdio.h>
#include "./cairo.h"
#include "./fb.h"
#include "./rsvg.h"

void test_cairo();

int main(){

  start_fb();
  start_cairo();

  // getchar();

  cleairo();
  rsvg2cairo("/home/darren/cangjie/auxiliary_shapes/svg/Cjem-a0-1.svg");
  cairo2fb();

  // rsvg2cairo("/home/darren/cangjie/auxiliary_shapes/svg/Cjrm-a0.svg");
  // cairo2fb();

  end_cairo();
  end_fb();
  return 0;

}
