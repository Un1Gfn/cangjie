#include <assert.h>
#include <locale.h>
#include <string.h>
#include <gdbm.h>
#include <stdlib.h>
#include <limits.h>
#include <wchar.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <uchar.h>

#include "./rpk.h"
#include "./l2r.h"

#define DB "Cangjie5.gdbm"

static GDBM_FILE dbf=NULL;

static void lookup(const wchar_t wc){
  datum v=gdbm_fetch(dbf,(datum){
    .dptr=(void*)(&wc),
    .dsize=sizeof(wchar_t)
  });
  if(!v.dptr){
    assert(!v.dsize);
    puts("\u30FB"); // nakaguro
    return;
  }
  // fputwc(wc,stdout);
  printf("%lc",wc);
  // fwrite(d.dptr,1,d.dsize,stdout);
  for(int i=0;i<v.dsize;i+=RPK+1){
    printf("\u3000"); // fullwidth space
    // printf("\uFF3B"); // fullwidth '['
    for(int j=0;j<RPK;++j){
      const char c=v.dptr[i+j];
      if('\0'==c)
        printf("\u3000"); // fullwidth space
      else
        printf("%s",letter2radical(c));
    }
    // printf("\uFF3D"); // fullwidth ']'
  }
  puts("");

}

int main(const int argc,char *const argv[]){

  static_assert(16==MB_LEN_MAX);
  assert(setlocale(LC_ALL,"zh_TW.UTF-8"));
  assert(6==MB_CUR_MAX);
  // printf("%zu\n",MB_CUR_MAX);

  assert(2==argc||3<=argc);
  // assert(argv[1]);
  // assert(3<=strlen(argv[1]));

  assert(0==access(DB,F_OK));
  assert(0==chmod(DB,00444));
  assert(0==access(DB,R_OK));
  assert((dbf=gdbm_open(DB,0,GDBM_READER,0,NULL)));

  // wchar_t wc=0;
  // assert(strlen("麻")==mbrtowc(&wc,"麻",strlen("麻"),&(mbstate_t){}));
  // datum d=gdbm_fetch(dbf,(datum){
  //   .dptr=(char*)&wc,
  //   .dsize=sizeof(wchar_t)
  // });
  // assert(d.dptr);
  // fwrite(d.dptr,1,d.dsize,stdout);
  // puts("");
  // free(d.dptr);d=(datum){};
  // exit(1);

  argc>=3?puts(""):0;
  for(int i=1;i<argc;++i){
    const char *s=argv[i];
    size_t n=strlen(s);
    for(;;){
      assert('\0'!=*s);
      mbstate_t m={};
      wchar_t wc=0;
      const size_t r=mbrtowc(&wc,s,n,&m);
      assert(1<=r&&r<=3);
      assert(1==mbsinit(&m));
      lookup(wc);
      assert(n>=r);
      // printf("n=%zu r=%zu \n",n,r);
      if(n==r)
        break;
      n-=r;
      s+=r;
    }
    argc>=3?puts(""):0;
  }

  assert(0==gdbm_close(dbf));dbf=NULL;

  return 0;

}
