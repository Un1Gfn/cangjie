// serialize com.github.Jackchows.Cangjie5 to a gdbm database

// instructions in sphinx.public/cangjie.rst

#include <assert.h>
#include <errno.h>
#include <fcntl.h> // S_IR* S_IW*
#include <gdbm.h>
#include <locale.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wchar.h>

#include "./rpk.h"

#define STR0(x) #x
#define STR(x) STR0(x)

static GDBM_FILE dbf=NULL;

static_assert(32/8==sizeof(wint_t));
static wint_t k=0;
static const datum pk={.dptr=(void*)(&k),.dsize=sizeof(k)};
static_assert(32/8==sizeof(wint_t));
static char v[RPK+1]={};
static const datum pv={.dptr=v,.dsize=RPK+1};

static void help(const char *const argv0){
  eprintf("Usage: %s dictionary.gdbm 0<input.txt\n",argv0);
  eprintf("       %s -h|--help\n",argv0);
}

static inline void count(const bool buf){
  gdbm_count_t ks=0;
  size_t bs=0;
  assert(0==gdbm_count(dbf,&ks));
  assert(0==gdbm_bucket_count(dbf,&bs));
  buf?
    printf("%llu keys in %zu buckets\n",ks,bs):
    eprintf("%llu keys in %zu buckets\n",ks,bs);
}

static inline bool getline2(){

  // assert(WEOF!=(k=fgetwc(stdin)));
  const int n=scanf("%lc",&k);
  if(1!=n){
    assert(-1==n);
    assert(!ferror(stdin));
    assert(feof(stdin));
    return false;
  }

  // eprintf("0x%08X [%lc] ",k,k); //
  assert(1<=k);

  bzero(v,RPK+1);
  assert('\t'==getchar());
  assert(1==scanf("%"STR(RPK)"s",v)); // (unreliable) whitespace of any kind matches 0+ whitespace of any kind
  assert('\0'==v[RPK]);
  // eprintf("\"%s\" ",v); //
  if('\n'!=getchar())
    assert(!ferror(stdin)&&feof(stdin));

  // eprintf("\n"); //
  return true;

}

static inline void add(){

  // count(true);

  // assert(!gdbm_exists(dbf,pk));

  const int n=gdbm_store(dbf,pk,pv,GDBM_INSERT);

  if(0==n){

    // putchar('.');

  }else{

    assert(+1==n);
    // putchar('O');
    // printf("\e[7m \e[0m");

    // fetch
    datum d=gdbm_fetch(dbf,pk);
    assert(d.dptr);
    assert(RPK+1<=d.dsize);
    assert(0==d.dsize%(RPK+1));

    // modify
    assert((d.dptr=realloc(d.dptr,d.dsize+(RPK+1))));
    bzero(d.dptr+d.dsize,RPK+1);
    strcpy(d.dptr+d.dsize,v);
    d.dsize+=(RPK+1);

    // replace
    assert(0==gdbm_store(dbf,pk,d,GDBM_REPLACE));
    free(d.dptr);d.dptr=NULL;

  }

}

int main(const int argc,const char *const argv[]){

  if(true
    && (0==isatty(STDIN_FILENO)&&ENOTTY==errno) // make sure stdin is not terminal
    && (2==argc&&0!=strcmp("-h",argv[1])&&0!=strcmp("--help",argv[1]))
  ){
    ;
  }else{
    help(argv[0]);
    exit(EXIT_FAILURE);
  }

  assert(setlocale(LC_ALL,""));
  assert(-1==access(argv[1],F_OK)&&ENOENT==errno); // make sure argv[1] does not exist
  assert((dbf=gdbm_open(argv[1],0,GDBM_WRCREAT,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH,NULL)));
  count(false);

  while(getline2())
    add();

  assert(0==gdbm_reorganize(dbf));
  count(false);
  assert(0==gdbm_close(dbf));dbf=NULL;
  return 0;

}
