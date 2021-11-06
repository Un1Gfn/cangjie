// gcc -std=gnu11 -g -O0 -Wextra -Wall -Winline -Wshadow -fanalyzer $(pkg-config --cflags ncurses) -o key.out key.c $(pkg-config --libs ncurses) && ./key.out

#include <assert.h>
#include <stdio.h>
#include <curses.h>
#include <stdlib.h>
#include <locale.h>
#include <time.h>
#include <ctype.h>

enum {
  PAIR_DEFAULT=0,
  PAIR_KEYFUNC,
  PAIR_QUESTION,
  PAIR_INCORRECT,
  PAIR_CORRECT
};

static_assert(1<=TRUE);
static_assert(0==FALSE);

static WINDOW *w0=NULL;
static WINDOW *w=NULL;

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

static inline void keyboardcontrol(const char *const key,const char *const function){
  printw(key);
  assert(OK==attron(COLOR_PAIR(PAIR_KEYFUNC)));
  printw("%-6s",function);
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
  keyboardcontrol("F10","Quit");
  keyboardcontrol("Enter","Quit");
  keyboardcontrol("BKSP","Quit");
  // keyboardcontrol("F1","Help");
  move(0,0); // physical cursor not moved until refresh
  assert(OK==refresh());

}

static inline void echo2(const int pair,const char *const s){
  assert(OK==wattron(w,COLOR_PAIR(pair)));
  wprintw(w,"%s",s);
  assert(OK==wattroff(w,COLOR_PAIR(pair)));
}

static void loop(){

  for(;;){

    const char question=random()%('z'-'a'+1);

    echo2(PAIR_QUESTION,(const char[]){'A'+question,'?','\0'});
    wechochar(w,' ');
    // waddch(w,' ');
    // assert(OK==wrefresh(w));

    for(;;){

      const int answer=getch();

      if(false
        ||KEY_F(10)==answer
        ||'\n'==answer // nl() // getch(3x): KEY_ENTER is enter key on the numeric keypad
        ||KEY_BACKSPACE==answer
      )
        return;

      if('a'+question==tolower(answer)){
        echo2(PAIR_CORRECT,(const char[]){toupper(answer),'\n','\0'});
        wrefresh(w);
        break;
      }

      echo2(PAIR_INCORRECT,(const char[]){toupper(answer),'\0'});
      wechochar(w,' ');

    }

  }

}

int main(){

  initscr2();

  newwin2();

  srand(time(NULL));
  loop();

  assert(OK==delwin(w));w=NULL;
  assert(OK==endwin());w0=NULL;
  assert(TRUE==isendwin());
  return 0;

}
