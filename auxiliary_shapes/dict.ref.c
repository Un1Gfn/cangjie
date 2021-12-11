#include <assert.h>
#include "./dict.h"

const D d[]={
  (D){
    .k='a',
    .r=(const char *const[]){"Cjrm-a0.svg","Cjrm-a1.svg","Cjrm-a2.svg"},
    .e=(const char *const[]){"Cjem-a0-1.svg","Cjem-a0-2.svg","Cjem-a0-3.svg","Cjem-a1-1.svg","Cjem-a2-1.svg"}
  },
  (D){
    .k='a',
    .r=(const char *const[]){"Cjrm-a3.svg"},
    .e=(const char *const[]){"Cjem-a3-1.svg","Cjem-a3-2.svg","Cjem-a3-3.svg","Cjem-a3-4.svg","Cjem-a3-5.svg"}
  }
};

static_assert(2==sizeof(d)/sizeof(D));
