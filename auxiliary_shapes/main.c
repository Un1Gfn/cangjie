#include "./cairo.h"
#include "./fb.h"

void test_cairo();

int main(){

  start_fb();

  start_cairo();
  // start_rsvg();

  // rsvg2cairo();
  test_cairo();

  cairo2fb();

  // end_rsvg();
  // end_cairo();
  end_fb();
  return 0;

}
