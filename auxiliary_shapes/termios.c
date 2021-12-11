// $ stty -cooked

#include <limits.h> // MAX_INPUT
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include "./termios.h"

#define for_range_int(var,range) for(size_t var=0;var<sizeof(range)/sizeof(int);++var)

// ascii caret/circumflex
#define XOR(C) (C^0b01000000)

#define OFF(FS,F)      assert((FS)&(F));FS&=(~(F))
#define ON(FS,F)  assert(0==((FS)&(F)));FS|=(F)

/* PRIVATE */

static_assert(0xFF==MAX_INPUT);
static_assert(0xFF==_POSIX_MAX_INPUT);
static_assert(0xFF==MAX_CANON);
static_assert(0xFF==_POSIX_MAX_CANON);

static_assert(0==STDIN_FILENO);
static_assert(1==STDOUT_FILENO);
static_assert(2==STDERR_FILENO);

static_assert(32==NCCS);
static_assert(-1==EOF);

// rely on fb.c:start_fb() to ensure virtual console

static void vt(){

  assert(1==isatty(STDIN_FILENO));
  assert(1==isatty(STDOUT_FILENO));

  const char *s=NULL;

  assert((s=ttyname(STDIN_FILENO)));
  assert(s==ttyname(STDOUT_FILENO));
  // puts(s);

  assert((s=ctermid(NULL)));
  assert(0==(strcmp("/dev/tty",s)));

}

/* PUBLIC */

void start_termios(){

  vt();

  // console_codes(4) DEC Private Mode (DECSET/DECRST) sequences
  // make cursor invisible
  printf("\e[?25l");
  fflush(stdout);

  // identical terminal attributes from stdin stdout
  struct termios ts[2]={};
  for(int i=0;i<2;++i){
    assert(0==tcgetattr(i,ts+i));
    if(i)
      assert(0==memcmp(ts,ts+i,sizeof(struct termios)));
  }

  // https://www.gnu.org/software/libc/manual/html_node/Editing-Characters.html
  assert(XOR('D')==ts->c_cc[VEOF]);
  assert(0==ts->c_cc[VEOL]);
  assert(0==ts->c_cc[VEOL2]);
  assert(XOR('?')==ts->c_cc[VERASE]);
  assert(XOR('W')==ts->c_cc[VWERASE]);
  assert(XOR('U')==ts->c_cc[VKILL]);
  assert(XOR('R')==ts->c_cc[VREPRINT]);

  // printf("0x%X\n",ts->c_iflag);
  // printf("0x%X\n",ts->c_oflag);
  // printf("0x%X\n",ts->c_cflag);

  // /usr/include/bits/termios-c_lflag.h
  // bc <<<"obase=2;ibase=16;8A3B"
  // 1000 1010 0011 1011
  if(0x8A3B!=ts->c_lflag){
    fprintf(stderr,"err\n");
    fprintf(stderr,"c_lflag 0x%X ",ts->c_lflag);
    const tcflag_t r[]={/*ALTWERASE,NOKERNINFO,*/ECHO,ECHOCTL,ECHOE,ECHOK,ECHOKE,ECHONL,ECHOPRT,FLUSHO,ICANON,IEXTEN,ISIG,NOFLSH,PENDIN,TOSTOP};
    const char *rs[]={"ECHO","ECHOCTL","ECHOE","ECHOK","ECHOKE","ECHONL","ECHOPRT","FLUSHO","ICANON","IEXTEN","ISIG","NOFLSH","PENDIN","TOSTOP"};
    for_range_int(i,r){
      // printf("%#6x %s\n",r[i],rs[i]);
      if((r[i])&(ts->c_lflag))
        fprintf(stderr,"%s ",rs[i]);
    }
    fprintf(stderr,"\n");
  }

  OFF(ts->c_lflag,IEXTEN);
  OFF(ts->c_lflag,ICANON);
  OFF(ts->c_lflag,ECHO);

  fflush(stdout);
  assert(0==tcsetattr(STDIN_FILENO,TCSAFLUSH,ts));

}

void end_termios(){
  printf("\e[?25h"); // make cursor visible again
  struct termios t={};
  assert(0==tcgetattr(STDIN_FILENO,&t));
  ON(t.c_lflag,IEXTEN);
  ON(t.c_lflag,ICANON);
  ON(t.c_lflag,ECHO);
  fflush(stdout);
  assert(0==tcsetattr(STDIN_FILENO,TCSAFLUSH,&t));
}

// static void loop(){
//   puts("press <Q> to quit");
//   for(;;){
//     int c=getchar();
//     if('q'==c||'Q'==c){
//       printf("\e[0m");
//       // printf("\r\n");
//       printf("\n");
//       return;
//     }
//     if(isalnum(c)){
//       isalpha(c)?(c=toupper(c)):0;
//       // [31,37]
//       printf("\e[%dm""%c""\e[0m",31+c%7,c);
//       // printf("[%c %d 0x%02X] ",c,c,c); // unix2dos(1) // CRLF
//       fflush(stdout);
//     }
//   }
// }

// int main(){

//   tty2();

//   start_termios();

//   loop();

//   end_termios();

//   return 0;
// }
