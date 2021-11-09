// instructions in sphinx.public/cangjie.rst

#include <assert.h>
#include <stdio.h>
#include <curses.h>
#include <stdlib.h>
#include <locale.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <gdbm.h>
#include <unistd.h> // access()
#include <sys/stat.h> // chmod()

#include "serialize.h"

#define KEY2_CTRL_R 18
#define KEY2_DOUBLEESC 27

enum {
  PAIR_DEFAULT=0,
  PAIR_KEYFUNC,
  PAIR_QUESTION,
  PAIR_INCORRECT,
  PAIR_CORRECT
};

static_assert(1<=TRUE);
static_assert(0==FALSE);

static GDBM_FILE dbf=NULL;

static WINDOW *w0=NULL;
static WINDOW *w=NULL;

const char *const keyboard[]={
  /*a*/ "日",
  /*b*/ "月",
  /*c*/ "金",
  /*d*/ "木",
  /*e*/ "水",
  /*f*/ "火",
  /*g*/ "土",
  /*h*/ "斜(竹)",
  /*i*/ "點(戈)",
  /*j*/ "交(十)",
  /*k*/ "叉(大)",
  /*l*/ "縱(中)",
  /*m*/ "橫(一)",
  /*n*/ "鉤(弓)",
  /*o*/ "人",
  /*p*/ "心",
  /*q*/ "手",
  /*r*/ "口",
  /*s*/ "側(尸)",
  /*t*/ "並(廿)",
  /*u*/ "仰(山)",
  /*v*/ "紐(女)",
  /*w*/ "方(田)",
  /*x*/ "難",
  /*y*/ "卜",
  /*z*/ "重"
};

static_assert(26*sizeof(void*)==sizeof(keyboard));

static void help(const char *const argv0){
  eprintf("Usage: %s dictionary.gdbm\n",argv0);
  eprintf("       %s -h|--help\n",argv0);
}

static void initscr2(){

  // ncurses(3x)
  setlocale(LC_ALL,"");
  assert(NULL!=(w0=initscr()));

  // curs_color(3x)
  // curs_variables(3x)
  assert(OK==start_color());
  assert(TRUE==has_colors());
  assert(256==COLORS);
  assert(COLORS*COLORS==COLOR_PAIRS);
  assert(TRUE==can_change_color());
  assert(OK==use_default_colors());
  assert(OK==init_pair(PAIR_KEYFUNC,-1,COLOR_BLUE));
  assert(OK==init_pair(PAIR_QUESTION,COLOR_YELLOW,-1));
  assert(OK==init_pair(PAIR_INCORRECT,COLOR_RED,-1));
  assert(OK==init_pair(PAIR_CORRECT,COLOR_GREEN,-1));

  assert(OK==cbreak()); // disable cooked mode
  assert(OK==noecho());
  assert(OK==keypad(w0,TRUE)); // getch function keys
  timeout(-1); // blocking read
  assert(OK==nl()); // Enter='\n' (default)
  // assert(OK==nonl()); // Enter='\r'
  // assert(OK==intrflush(w0,?));

  assert(OK==refresh());

}

static inline void keyboardcontrol(const char *const *keys,const char *const function){
  addch(' ');
  for(;*keys;++keys){
    printw(*keys);
    assert(OK==attron(A_DIM));
    addch('/');
    assert(OK==attroff(A_DIM));
  }
  assert(OK==attron(COLOR_PAIR(PAIR_KEYFUNC)));
  printw("%-7s",function);
  assert(OK==attroff(COLOR_PAIR(PAIR_KEYFUNC)));
}

static void newwin2(){

  int y=-2,x=-2;
  // printw("%d %d\n",y,x);
  getyx(w0,y,x);assert(0==y&&0==x);
  getparyx(w0,y,x);assert(-1==y&&-1==x);
  getbegyx(w0,y,x);assert(0==y&&0==x);
  getmaxyx(w0,y,x);assert(2<=y&&y<=99&&20<=x&&x<=499);assert(LINES==y&&COLS==x); // curs_variables(3x)
  assert(NULL!=(w=newwin(y-1,x,0,0)));

  // https://stackoverflow.com/questions/29565403/how-to-scroll-a-window-other-than-stdscreen-in-ncurses
  assert(OK==idlok(w,TRUE)); // use hardware insert/delete line feature
  assert(OK==scrollok(w,TRUE)); // on newline action on the bottom line, scrolled window up one line
  assert(OK==wrefresh(w));

  move(y-1,0);
  // keyboardcontrol((const char *const []){"F1","?",NULL},"Help");
  keyboardcontrol((const char *const []){"F5","^R",NULL},"Resize");
  keyboardcontrol((const char *const []){"F10","ESCESC","BKSP","Enter",NULL},"Quit");
  move(0,0); // physical cursor not moved until refresh
  assert(OK==refresh());

}

static inline void echo2(const int pair,const int index,const char *const postfix){
  assert(0<=index&&index<26);
  assert(OK==wattron(w,COLOR_PAIR(pair)));
  wprintw(w,"%s%s",keyboard[index],postfix);
  assert(OK==wattroff(w,COLOR_PAIR(pair)));
  assert(OK==wrefresh(w));
}

static void resize(){
  delwin(w);w=NULL;
  newwin2();
  // wprintw(w,"resize()\n");
  // assert(OK==clearok(w,TRUE));
  // wrefresh(w);
}

static void loop(){

  for(;;){

    const char question=random()%('z'-'a'+1);

    echo2(PAIR_QUESTION,question,"? ");
    // wechochar(w,' ');
    // waddch(w,' ');
    // assert(OK==wrefresh(w));

    for(;;){

      const int answer=getch();

      // if(
      //   KEY_RESIZE==answer
      // ){
      //   wprintw(w,"resize()\n");
      //   wrefresh(w);
      // }

      // resize
      if(KEY_F(5)==answer||KEY2_CTRL_R==answer){
        resize();
        echo2(PAIR_QUESTION,question,"? ");
        continue;
      }

      // quit
      if(false
        ||KEY_F(10)==answer
        ||'\n'==answer // nl() // getch(3x): KEY_ENTER is enter key on the numeric keypad
        ||KEY_BACKSPACE==answer
        ||KEY2_DOUBLEESC==answer
      )
        return;

      // garbage
      if(!(
        ('A'<=answer&&answer<='Z') ||
        ('a'<=answer&&answer<='z')
      )){
        // wprintw(w,"%d\n",answer);
        // wrefresh(w);
        continue;
      }

      // correct
      if(question==tolower(answer)-'a'){
        echo2(PAIR_CORRECT,question,"\n");
        break;
      }

      // incorrect
      echo2(PAIR_INCORRECT,tolower(answer)-'a'," ");

    }

  }

}

int main(const int argc,const char *const argv[]){

  if(2==argc&&0!=strcmp("-h",argv[1])&&0!=strcmp("--help",argv[1])){
    ;
  }else{
    help(argv[0]);
    exit(EXIT_FAILURE);
  }

  assert(0==access(argv[1],F_OK));
  assert(0==access(argv[1],R_OK)); // make sure argv[1] grants read permission
  assert(0==chmod(argv[1],00444));
  assert((dbf=gdbm_open(argv[1],0,GDBM_READER,00000,NULL)));

  initscr2();
  newwin2();

  srand(time(NULL));
  loop();

  assert(OK==delwin(w));w=NULL;
  assert(OK==endwin());w0=NULL;
  assert(TRUE==isendwin());
  assert(0==gdbm_close(dbf));dbf=NULL;
  return 0;

}
