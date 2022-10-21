#include "./l2r.h"
#include <assert.h>

/* PRIVATE */

static const char *const keyboard[]={
  /*a*/ "日",
  /*b*/ "月",
  /*c*/ "金",
  /*d*/ "木",
  /*e*/ "水",
  /*f*/ "火",
  /*g*/ "土",
  /*h*/ "竹",
  /*i*/ "戈",
  /*j*/ "十",
  /*k*/ "大",
  /*l*/ "中",
  /*m*/ "一",
  /*n*/ "弓",
  /*o*/ "人",
  /*p*/ "心",
  /*q*/ "手",
  /*r*/ "口",
  /*s*/ "尸",
  /*t*/ "廿",
  /*u*/ "山",
  /*v*/ "女",
  /*w*/ "田",
  /*x*/ "難",
  /*y*/ "卜",
  /*z*/ "重"
};

static_assert('z'-'a'+1==sizeof(keyboard)/sizeof(char*));

/* PUBLIC */

const char *letter2radical(const char c){
  // assert(('A'<=c&&c<='Z')||('a'<=c&&c<='z'))
  // return keyboard[tolower(c)-'a'];
  assert('a'<=c&&c<='z');
  return keyboard[c-'a'];
}
