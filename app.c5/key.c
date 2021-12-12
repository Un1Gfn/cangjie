// instructions in sphinx.public/cangjie.rst

#include <assert.h>
#include <ctype.h>
#include <curses.h>
#include <gdbm.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h> // SLIST_*
#include <sys/stat.h> // chmod()
#include <time.h>
#include <unistd.h> // access()

// #define DEBUG_ENABLED
#include "./debug.h"
#include "./l2r.h"
#include "./rpk.h"

#define KEY2_EOT 4
#define KEY2_CTRL_R 18
#define KEY2_DOUBLEESC 27

#define ADV(PP) PP=&((*PP)->next)

enum {
  PAIR_DEFAULT=0,
  PAIR_KEYFUNC,
  PAIR_QUESTION,
  PAIR_INCORRECT,
  PAIR_CORRECT
};

typedef struct _Node {
  struct _Node *next;
  int val; // indices in v.dptr
} Node;

static Node *head=NULL;

static_assert(1<=TRUE);
static_assert(0==FALSE);

static GDBM_FILE dbf=NULL;
static datum k={.dptr=NULL,.dsize=0};
static datum v={.dptr=NULL,.dsize=0};

static WINDOW *w0=NULL;
static WINDOW *w=NULL;

static gdbm_count_t n_kanji=0;

static const char *const keyboard_explained[]={
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

static_assert(26*sizeof(void*)==sizeof(keyboard_explained));

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
  keyboardcontrol((const char *const []){"F2","Enter",NULL},"Skip");
  keyboardcontrol((const char *const []){"F5","^R",NULL},"Resize");
  keyboardcontrol((const char *const []){"F10","^D","ESCESC","BKSP",NULL},"Quit");
  move(0,0); // physical cursor not moved until refresh
  assert(OK==refresh());

}

static inline void echo2(const int pair,const int index,const char *const postfix){
  assert(0<=index&&index<26);
  assert(OK==wattron(w,COLOR_PAIR(pair)));
  wprintw(w,"%s%s",keyboard_explained[index],postfix);
  assert(OK==wattroff(w,COLOR_PAIR(pair)));
  assert(OK==wrefresh(w));
}

static void ask(){
  wprintw(w,"%lc",*((wint_t*)k.dptr));
  for(int i=0;i<v.dsize;i+=RPK+1){
    // wprintw(w,"[%s]",v.dptr+i);
    waddch(w,'[');
    for(int j=0;j<RPK+1;++j){
      const char c=v.dptr[i+j];
      if('a'<=c&&c<='z')
        wprintw(w,"%s",letter2radical(c));
      else
        assert('\0'==c);
    }
    waddch(w,']');
  }
  assert(OK==wechochar(w,'\n'));
}

static void kv_random(){
  // random key
  const long index=random()%n_kanji;
  k=gdbm_firstkey(dbf);
  assert(k.dptr);
  for(long i=0;i<index;++i){
    char *p=k.dptr;
    k=gdbm_nextkey(dbf,k);
    free(p);p=NULL;
    assert(k.dptr);
  }
  // value of random key
  v=gdbm_fetch(dbf,k);
  assert(v.dptr);
}

static void kv_free(){
  // wprintw(w,"kv_free()\n");wrefresh(w);
  free(k.dptr);k.dptr=NULL;
  free(v.dptr);v.dptr=NULL;
}

static void slist_build_from_v(){

  // init

  // append
  // https://stackoverflow.com/a/14697764/
  // struct TYPE *elm=&((*M)=(struct TYPE){NULL,7});
  #define M ((Node*)calloc(1,sizeof(Node)))
  assert(6<=v.dsize);
  assert(0==v.dsize%(RPK+1));
  int i=0;
  Node **pp=&head;
  while(i<v.dsize){    
    *pp=M;
    assert(*pp);
    (*pp)->val=i;
    ADV(pp);
    i+=RPK+1;
  }

  // show
  DEBUG(
    for(Node *p=head;p;p=p->next){
      wprintw(w,"[%d]%s ",p->val,v.dptr+(p->val));
    }
    assert(OK==wechochar(w,'\n'));
  )

}

enum {
  FILTER_CONTINUE=0,
  FILTER_DEPLETED,
  FILTER_MATCHED
};

static int slist_filter_out(const int c){
  // DEBUG(wprintw(w,"p->val=%d ",head->val);wrefresh(w);)
  DEBUG(wprintw(w,"slist_filter_out()\n");wrefresh(w);)
  assert(head);
  Node *prev=NULL;
  Node *p=head;
  for(;;){
    if(!p)
      break;
    DEBUG(wprintw(w,"X\n");wrefresh(w);)
    // DEBUG(wprintw(w,"c=%c ",c);wrefresh(w);)
    DEBUG(wprintw(w,"p->val=%d ",head->val);wrefresh(w);)
    // DEBUG(wprintw(w,"[p->val]=%c ",v.dptr[p->val]);wrefresh(w);)
    // DEBUG(wprintw(w,"[p->val+1]=%c\n",('\0'==v.dptr[(p->val)+1])?'0':v.dptr[(p->val)+1]);wrefresh(w);)
    if(c==v.dptr[p->val]){
      p->val+=1;
      if('\0'==v.dptr[p->val]){
        return FILTER_MATCHED;
      }
      prev=p;
      p=p->next;
    }else{
      if(prev){ // remove body
        assert(head!=p);
        prev->next=p->next;
        free(p);
        p=prev->next;
      }else{ // sever head
        assert(head==p);
        head=p->next;
        free(p);
        p=head;
      }
    }
  }
  if(!head)
    return FILTER_DEPLETED;
  return FILTER_CONTINUE;
}

static void slist_free(){
  // wprintw(w,"slist_free()\n");wrefresh(w);
  Node *p=head;
  while(p){
    Node *t=p;
    p=p->next;
    free(t);
  }
  head=NULL;
}

static void loop(){

  for(;;){ // each pass asks for a kanji

    kv_random();
    ask();
    // wrefresh(w);

    for(;;){ // each pass is an attempt to assemble the kanji

      // wechochar(w,'>');
      wprintw(w,"> ");
      assert(OK==wrefresh(w));
      slist_build_from_v();
      DEBUG(wprintw(w,"p->val=%d ",head->val);wrefresh(w);)

      for(;;){ // each pass types a radical

        DEBUG(wprintw(w,"p->val=%d ",head->val);wrefresh(w);)
        int answer=getch();

        // skip
        if(KEY_F(2)==answer||'\n'==answer){ // nl() // getch(3x): KEY_ENTER is enter key on the numeric keypad
          wechochar(w,'\n');
          slist_free();
          goto LBmatched;
        }

        // resize
        if(KEY_F(5)==answer||KEY2_CTRL_R==answer){
          delwin(w);w=NULL;
          newwin2();
          ask();
          // assert(OK==clearok(w,TRUE));
          // wrefresh(w);
          // echo2(PAIR_QUESTION,question,"? ");
          break; // discard slist and attempt again
        }

        // quit
        if(false
          ||KEY_F(10)==answer
          ||KEY2_EOT==answer
          ||KEY2_DOUBLEESC==answer
          ||KEY_BACKSPACE==answer
        ){
          slist_free();
          kv_free();
          return;
        }

        // stray key
        if(false
          || answer<'A'
          || ('Z'<answer&&answer<'a')
          || 'z'<answer
        ){
          wprintw(w,"%d\n",answer);
          wrefresh(w);
          continue;
        }

        answer=tolower(answer);
        wprintw(w,"%s ",letter2radical(answer));
        wrefresh(w);

        const int n=slist_filter_out(answer);

        if(FILTER_MATCHED==n){
          wechochar(w,'\n');
          slist_free();
          goto LBmatched;
        }

        if(FILTER_DEPLETED==n){
          wechochar(w,'\n');
          break;
        }

        assert(FILTER_CONTINUE==n);

        // echo2(PAIR_CORRECT,tolower(answer)-'a',"\n");
        // echo2(PAIR_INCORRECT,tolower(answer)-'a'," ");

      }

      slist_free();

    }

    LBmatched:
    kv_free();

  }

}

int main(const int argc,const char *const argv[]){

  {/*

    SLIST_INIT(&head);
    assert(SLIST_EMPTY(&head));
    assert(!SLIST_FIRST(&head));

    struct TYPE *e=M;
    SLIST_INSERT_HEAD(&head,e,next);

    struct TYPE *p=SLIST_FIRST(&head);
    struct TYPE *t=p;
    SLIST_REMOVE(&head,p,TYPE,next);
    free(t);t=NULL;

    assert(SLIST_EMPTY(&head));
    assert(!SLIST_FIRST(&head));

    exit(0);

  */}

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

  // srand(time(NULL));
  srand(0);
  assert(0==gdbm_count(dbf,&n_kanji));
  static_assert(16384==2<<(14-1));
  static_assert(131072==2<<(17-1));
  assert(16384<n_kanji&&n_kanji<131072);
  // wprintw(w,"%d\n",n_kanji);
  loop();

  assert(OK==delwin(w));w=NULL;
  assert(OK==endwin());w0=NULL;
  assert(TRUE==isendwin());
  assert(0==gdbm_close(dbf));dbf=NULL;
  return 0;

}
