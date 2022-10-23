// https://kristaps.bsd.lv/kcgi/
// --> sample.c
// --> https://kristaps.bsd.lv/kcgi/sample.c.html

// include.base
#include <assert.h>
#include <gdbm.h>
#include <stdbool.h>
#include <stdio.h> // fprintf(stderr, ...)
#include <stdlib.h> // free mbstowcs EXIT_FAILURE
#include <string.h> // memset
#include <unistd.h> // access
#include <sys/stat.h> // chmod
#include <locale.h> // setlocale

// include.kcgi.requirements
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include <kcgi.h>

// include.kcgihtml
#include <kcgihtml.h>

#define RPK 5
// fullwidth space
#define SEP "\u3000"
#define DB "/home/darren/cangjie/app.cj5.mkdict/Cangjie5.gdbm"

GDBM_FILE dbf=NULL;

/* Recognised page requests.  See pages[]. */
enum page {
  PAGE_DEFPAGE,
  PAGE__MAX,
};

// page names in URL component, mapped from the first name part of requests
// e.g., "/sample.cgi/defpage.html" -> "defpage" -> PAGE_DEFPAGE.
static const char *const pages[PAGE__MAX] = {
  "defpage", // PAGE_DEFPAGE
};

// keys - input field names
enum key {
  KEY_INTEGER,
  KEY_STRING,
  KEY__MAX,
};

static const struct kvalid keys[KEY__MAX] = {
  { kvalid_int, "rvkjm9pqcg8o3" }, // KEY_INTEGER
  { kvalid_stringne, "hjxrgq9ig35lz" }, // KEY_STRING
};

/*
 * Open an HTTP response with a status code and a particular
 * content-type, then open the HTTP content body.
 * You can call khttp_head(3) before this: CGI doesn't dictate any
 * particular header order.
 */
static void resp_open(struct kreq *const req, enum khttp http){
  /*
   * If we've been sent an unknown suffix like '.foo', we won't
   * know what it is.
   * Default to an octet-stream response.
   */
  assert(KCGI_OK==khttp_head(req, kresps[KRESP_STATUS], "%s", khttps[http]));
  assert(KCGI_OK==khttp_head(req, kresps[KRESP_CONTENT_TYPE], "%s", kmimetypes[(KMIME__MAX==req->mime) ? KMIME_APP_OCTET_STREAM : req->mime]));
  // plain
  assert(KCGI_OK==khttp_body(req));
  // assert(KCGI_OK==khttp_body_compress(req, 0));
  // compressed
  // assert(KCGI_OK==http_head(req, KRESP_CONTENT_ENCODING, "..."))
  // assert(KCGI_OK==khttp_body_compress(req, 1));
}

static const char *letter2radical(const char c){
  assert('a'<=c&&c<='z');
  static const char *const keyboard[]={
    "\u65E5", 
    "\u6708", 
    "\u91D1", 
    "\u6728", 
    "\u6C34", 
    "\u706B", 
    "\u571F", 
    "\u7AF9", 
    "\u6208", 
    "\u5341", 
    "\u5927", 
    "\u4E2D", 
    "\u4E00", 
    "\u5F13", 
    "\u4EBA", 
    "\u5FC3", 
    "\u624B", 
    "\u53E3", 
    "\u5C38", 
    "\u5EFF", 
    "\u5C71", 
    "\u5973", 
    "\u7530", 
    "\u96E3", 
    "\u535C", 

    "\u91CD", 
  };
  _Static_assert(26=='z'-'a'+1, "");
  _Static_assert(26==sizeof(keyboard)/sizeof(char*), "");
  // for(int i=0; i<26; ++i)
  //   fprintf(stderr, "%c %s\n", 'a'+i, keyboard[i]);
  return keyboard[c-'a'];
}

static void lookup(struct khtmlreq *const rp, const char *const s){

  wchar_t wcs[1024]={};
  size_t n=0;

  // khtml_printf(rp, "%s", s); khtml_elem(rp, KELEM_BR);

  // wchar.h:mbsrtowcs
  // stdlib.h:mbstowcs
  n=mbstowcs(wcs, s, 1024);
  // assert(1==n);
  assert(1<=n && n<1024);
  // fprintf(stderr, "%zu\n", n);
  // fprintf(stderr, "%zu\n", n+1);
  // return;

  for(size_t i=0; wcs[i]; ++i){

    assert(i<1024);

    char pmb[MB_CUR_MAX+1]; bzero(pmb, MB_CUR_MAX+1);
    n=wctomb(pmb, wcs[i]);
    assert(1<=n && n<=MB_CUR_MAX);
    khtml_printf(rp, "%s", pmb);

    datum v=gdbm_fetch(dbf,(datum){
      .dptr=(void*)(&wcs[i]),
      .dsize=sizeof(wchar_t)
    });
    if(v.dptr){
      assert(v.dsize);
    }else{
      assert(!v.dsize);
      khtml_elem(rp, KELEM_BR);
      continue;
    }

    // fprintf(stderr, "%lc\n", wcs[0]);
    // fwrite(d.dptr, 1, d.dsize,stdout);

    for(int j=0; j<v.dsize; j+=RPK+1){
      khtml_printf(rp, SEP);
      for(int k=0; k<RPK; ++k){
        const char c=v.dptr[j+k];
        if('\0'==c){
          break;
        }else{
          khtml_printf(rp, "%s", letter2radical(c));
        }
      }
    }
    khtml_elem(rp, KELEM_BR);

  }

}

/*
 * Demonstrates how to use GET and POST forms and building with the HTML
 * builder functions.
 * Returns HTTP 200 and HTML content.
 */
static void sendindex(struct kreq *req){

  struct khtmlreq r={};

  khtml_open(&r, req, 0);

  khtml_elem(&r, KELEM_DOCTYPE);
  
  khtml_elem(&r, KELEM_HTML); {

    khtml_elem(&r, KELEM_HEAD); {
      khtml_elem(&r, KELEM_TITLE); khtml_puts(&r, "Welcome!"); khtml_closeelem(&r, 1);
      khtml_attr(&r, KELEM_META, KATTR_CHARSET, "utf-8", KATTR__MAX); // <meta charset="utf-8">
      khtml_attr(&r, KELEM_LINK, KATTR_REL, "stylesheet", KATTR_HREF, "/cj5.css", KATTR__MAX); // <link rel="stylesheet" href="mystyle.css">
    }; khtml_closeelem(&r, 1); // </head>

    khtml_elem(&r, KELEM_BODY); {

      khtml_elem(&r, KELEM_BR);

      khtml_puts(&r, "Welcome!");khtml_elem(&r, KELEM_BR);
      khtml_elem(&r, KELEM_BR);

      char *page=NULL;
      kasprintf(&page, "%s/%s", req->pname, pages[PAGE_DEFPAGE]);
      khtml_attr(&r, KELEM_FORM,
        KATTR_METHOD, "post",
        KATTR_ENCTYPE, "multipart/form-data",
        KATTR_ACTION, page,
        KATTR__MAX
      );
      explicit_bzero(page, 1+strlen(page));
      free(page); page=NULL; {

        khtml_elem(&r, KELEM_FIELDSET); {

          khtml_elem(&r, KELEM_LEGEND); khtml_puts(&r, "Post (multipart)"); khtml_closeelem(&r, 1);

          // p.input.buttons
          khtml_elem(&r, KELEM_P); {
            khtml_attr(&r, KELEM_A, KATTR_HREF, "/cgi-bin/cj5.cgi/defpage", KATTR__MAX); khtml_attr(&r, KELEM_INPUT, KATTR_TYPE, "button", KATTR_VALUE, "\u274C", KATTR__MAX) /* "\u232B" "\U0001F5D1" */; khtml_closeelem(&r, 1);
            khtml_puts(&r, " ");
            // https://www.compart.com/en/unicode/block/U+2190
            // https://unicode-table.com/en/blocks/arrows/
            khtml_attr(&r, KELEM_INPUT, KATTR_TYPE, "reset", KATTR_VALUE, "\u21BA", KATTR__MAX); // "\u27F3" "\u21A9"
            khtml_puts(&r, " ");
            khtml_attr(&r, KELEM_INPUT, KATTR_TYPE, "submit", KATTR_VALUE, "\U0001F50E", KATTR__MAX);
          } khtml_closeelem(&r, 1);

          // p.input.incr
          khtml_elem(&r, KELEM_P); {
            khtml_attr(&r, KELEM_INPUT,
              KATTR_TYPE, "number",
              KATTR_NAME, keys[KEY_INTEGER].name,
              KATTR_VALUE, (req->fieldmap[KEY_INTEGER] ? req->fieldmap[KEY_INTEGER]->val : ""), // default value for input.reset
            KATTR__MAX);
            if(req->fieldmap[KEY_INTEGER]){
              khtml_puts(&r, " incr: ");
              khtml_printf(&r, "%ld", 1+req->fieldmap[KEY_INTEGER]->parsed.i);
            }
          }; khtml_closeelem(&r, 1);

          // p.input.cj5
          khtml_elem(&r, KELEM_P); {
            struct kpair *const p=req->fieldmap[KEY_STRING];
            khtml_attr(&r, KELEM_INPUT,
              KATTR_TYPE, "search", // "text"
              KATTR_NAME, keys[KEY_STRING].name,
              KATTR_VALUE, p?p->val:"", // default value for input.reset
            KATTR__MAX);
            if(p){
              khtml_elem(&r, KELEM_P); {
                assert(p->val && p->val==p->parsed.s);
                const size_t n=strnlen(p->val, 1024*8); assert(n<1024*8); assert(p->val[n-1]&&!p->val[n]);
                lookup(&r, p->val);
              }; khtml_closeelem(&r, 1);
            }
          }; khtml_closeelem(&r, 1);


        }; khtml_closeelem(&r, 1); // </fieldset>

      }; khtml_closeelem(&r, 1); // </form>    

    }; khtml_closeelem(&r, 1); // </body>

  }; khtml_closeelem(&r, 1); // </html>

  assert(0==khtml_elemat(&r));
  khtml_close(&r);

}

/*
 * We'll use this to route pages by creating an array indexed by our
 * page.
 * Then when the page is parsed, we'll route directly into it.
 */
static void (*const disps[PAGE__MAX])(struct kreq*) = {
  sendindex, // PAGE_DEFPAGE
};

int main(){

  // fprintf(stderr, "\n");
  // fprintf(stderr, ": main.c\n");

  assert(setlocale(LC_ALL,"zh_TW.UTF-8"));

  assert(0==access(DB,F_OK));
  assert(0==chmod(DB,00444));
  assert(0==access(DB,R_OK));
  assert((dbf=gdbm_open(DB,0,GDBM_READER,0,NULL)));

  struct kreq req={/*.port=9513u*/};

  // initialize main http context
  assert(KCGI_OK==khttp_parse(
    &req,                          // input fields and HTTP context parsed from the CGI environment
    keys, KEY__MAX,                // input and validation fields
    pages, PAGE__MAX, PAGE_DEFPAGE // recognised pathnames and fallback page if no default landing page is specified
  ));
  // req.port=9513u;

  // inspect_kreq(&req);

  // accept HTTP method GET/POST/OPTIONS only
  switch(req.method){
    case KMETHOD_OPTIONS: // curl -vX OPTIONS
      khttp_head(&req, kresps[KRESP_ALLOW], "OPTIONS GET POST");
      resp_open(&req, KHTTP_200); // 200 OK
      break;
    case KMETHOD_GET:
    case KMETHOD_POST:
      // restrict to text/html and a valid page
      if(PAGE__MAX <= req.page || KMIME_TEXT_HTML != req.mime){
        resp_open(&req, KHTTP_404); // 404 Not Found
      }else{
        resp_open(&req, KHTTP_200);
        (*disps[req.page])(&req); // dispatch array to send to the page handlers
      }
      break;
    default:
      fprintf(stderr, "req.method=KMETHOD_ACL+%d\n", req.method-KMETHOD_ACL);
      resp_open(&req, KHTTP_405); // 405 Method Not Allowed
      break;
  }

  khttp_free(&req); req=(struct kreq){};

  assert(0==gdbm_close(dbf)); dbf=NULL;

  return 0;

}
