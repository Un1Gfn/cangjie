#include <assert.h>
#include <stdio.h>
#include <sys/types.h> /* size_t, ssize_t */
// #include <stdarg.h> /* va_list */
// #include <stddef.h> /* NULL */
#include <stdint.h> /* int64_t */

#include <kcgi.h>

int main(){
  
  struct kreq req={};
  // struct kreq req={.port=9513};
  enum page {
    PAGE_INDEX,
    PAGE__MAX
  };
  const char *const pages[PAGE__MAX] = {
    "index", /* PAGE_INDEX */
  };

  puts("");
  puts("");
  puts("");

  printf("%d %d %d - %s - %d\n", KCGI_VMAJOR, KCGI_VMINOR, KCGI_VBUILD, KCGI_VERSION, KCGI_VSTAMP);

  fprintf(stderr, "A\n");
  assert(KCGI_OK==khttp_parse(&req, NULL, 0, pages, PAGE__MAX, PAGE_INDEX));
  fprintf(stderr, "B\n");

  for(struct kpair *p=*(req.fieldmap); p; ++p){
    printf("%s\n", p->key);
  }
  fprintf(stderr, "C\n");

  khttp_free(&req); req=(struct kreq){};
  fprintf(stderr, "D\n");

  puts("");
  puts("");
  puts("");
  
  return 0;
}
