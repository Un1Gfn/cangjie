#include <assert.h>
#include <locale.h>
#include <string.h>
#include <gdbm.h>
#include <stdlib.h>
#include <limits.h>
#include <wchar.h>
#include <stdio.h>

#define DB "Cangjie5.gdbm"

// static GDBM_FILE dbf=NULL;

int main(){

  static_assert(16==MB_LEN_MAX);
  assert(setlocale(LC_ALL,"zh_TW.UTF-8"));
  assert(6==MB_CUR_MAX);
  // printf("%zu\n",MB_CUR_MAX);

  // assert(0==access(DB,F_OK));
  // assert(0==chmod(DB,00444));
  // assert(0==access(DB,R_OK));
  // assert((dbf=gdbm_open(DB,0,GDBM_READER,0,NULL)));

  // const char *s="!A啊うu？#?";
  // size_t n=strlen(s)+1;

  char *s=NULL;
  size_t _=0;
  size_t n=getline(&s,&_,stdin);
  assert(s);
  assert(2<=n&&n==strlen(s));
  // puts(s);
  s[--n]='\0'; // ditch newline

  assert(0==mbrlen(NULL,0,NULL));
  const char *p=s;
  for(;;){
    if(*p=='\0')
      break;
    const size_t l=mbrlen(p,n,NULL);
    assert(l!=0);
    assert(l!=(size_t)-1);
    assert(l!=(size_t)-2);
    assert(l<=4);
    printf("%.*s\n",(int)l,p);
    n-=l;
    p+=l;
  }
  p=NULL;

  free(s);s=NULL;

  // assert(0==gdbm_close(dbf));dbf=NULL;

  return 0;

}
