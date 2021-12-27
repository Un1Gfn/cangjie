#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>

#include <assert.h>
#include <gdbm.h>
#include <limits.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <termios.h>
#include <uchar.h>
#include <unistd.h>
#include <wchar.h>
#include <signal.h>

#include "./rpk.h"
#include "./l2r.h"

#define DB "Cangjie5.gdbm"

#define NAKAGURO "\u30FB"
#define FULLWIDTH_SPACE "\u3000"
#define FULLWIDTH_BRACKET_L "\uFF3B"
#define FULLWIDTH_BRACKET_R "\uFF3D"

#define OFF(FS,F)      assert((FS)&(F));FS&=(~(F))
#define ON(FS,F)  assert(0==((FS)&(F)));FS|=(F)

static GDBM_FILE dbf=NULL;

static void lookup(const wchar_t wc){
  datum v=gdbm_fetch(dbf,(datum){
    .dptr=(void*)(&wc),
    .dsize=sizeof(wchar_t)
  });
  if(!v.dptr){
    assert(!v.dsize);
    puts(NAKAGURO);
    return;
  }
  // fputwc(wc,stdout);
  printf("%lc",wc);
  // fwrite(d.dptr,1,d.dsize,stdout);
  for(int i=0;i<v.dsize;i+=RPK+1){
    printf(FULLWIDTH_SPACE);
    for(int j=0;j<RPK;++j){
      const char c=v.dptr[i+j];
      if('\0'==c)
        printf(FULLWIDTH_SPACE);
      else
        printf("%s",letter2radical(c));
    }
  }
  puts("");

}

static void help(const char *const argv0){
  fprintf(stderr,"Usage: %s \"TEXT\" [\"TEXT\"...]\n",argv0);
  fprintf(stderr,"       %s -h|--help\n",argv0);
  exit(1);
}

static void line(const char *s, size_t n){
  for(;;){
    assert('\0'!=*s);
    mbstate_t m={};
    wchar_t wc=0;
    const size_t r=mbrtowc(&wc,s,n,&m);
    assert(1<=r&&r<=4);
    // printf("%zu\n",r);
    assert(1==mbsinit(&m));
    lookup(wc);
    assert(n>=r);
    // printf("n=%zu r=%zu \n",n,r);
    if(n==r)
      break;
    n-=r;
    s+=r;
  }
}

int main(const int argc,char *const argv[]){

  // SJLJ
  // > ...^C
  // >
  // https://stackoverflow.com/questions/16828378/readline-get-a-new-prompt-on-sigint
  // signal(SIGINT,SIG_IGN);

  static_assert(16==MB_LEN_MAX);
  assert(setlocale(LC_ALL,"zh_TW.UTF-8"));
  assert(6==MB_CUR_MAX);
  // printf("%zu\n",MB_CUR_MAX);
  if(2<=argc&&(0==strcmp(argv[1],"-h")||0==strcmp(argv[1],"--help")))
    help(argv[0]);

  assert(0==access(DB,F_OK));
  assert(0==chmod(DB,00444));
  assert(0==access(DB,R_OK));
  assert((dbf=gdbm_open(DB,0,GDBM_READER,0,NULL)));

  assert(1<=argc);
  if(1==argc){ // look up stdin loop
    using_history();
    puts("");
    for(;;){
      char *s=readline("?> ");
      if(!s){ // EOF
        clear_history();
        puts("");
        break;
      }
      if('\0'==s[0]) // blank
        continue;
      add_history(s);
      puts("");
      line(s,strlen(s));
      free(s);s=NULL;
      puts("");
    }
  }else{ // look up argv
    3<=argc?puts(""):0;
    for(int i=1;i<argc;++i){
      line(argv[i],strlen(argv[i]));
      3<=argc?puts(""):0;
    }
  }

  assert(0==gdbm_close(dbf));dbf=NULL;

  return 0;

}
