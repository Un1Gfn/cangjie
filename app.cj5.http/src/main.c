// https://kristaps.bsd.lv/kcgi/
// --> sample.c
// --> https://kristaps.bsd.lv/kcgi/sample.c.html

// include.base
#include <assert.h>
#include <stdlib.h> // free EXIT_FAILURE
#include <string.h> // memset
#include <stdio.h> // fprintf(stderr, ...)
#include <stdbool.h>

// include.kcgi.requirements
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include <kcgi.h>

// include.kcgihtml
#include <kcgihtml.h>

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

static void lookup(struct khtmlreq *const rp, const char *const s){
  khtml_printf(rp, "'%s'", s);
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
          khtml_elem(&r, KELEM_P);

            assert(KCGI_OK==khtml_attr(&r, KELEM_A, KATTR_HREF, "/cgi-bin/cj5.cgi/defpage", KATTR__MAX));
              // khtml_attr(&r, KELEM_INPUT, KATTR_TYPE, "button", KATTR_VALUE, "\U0001F5D1", KATTR__MAX);
              khtml_attr(&r, KELEM_INPUT, KATTR_TYPE, "button", KATTR_VALUE, "\u274C", KATTR__MAX);
            assert(KCGI_OK==khtml_closeelem(&r, 1));
            // a(&r, "/cgi-bin/cj5.cgi/defpage", "\u232B");
            // a(&r, "/cgi-bin/cj5.cgi/defpage", "\U0001F5D1");

            khtml_puts(&r, " ");
            // https://www.compart.com/en/unicode/block/U+2190
            // https://unicode-table.com/en/blocks/arrows/
            // khtml_attr(&r, KELEM_INPUT, KATTR_TYPE, "reset", KATTR_VALUE, "\u27F3", KATTR__MAX);
            khtml_attr(&r, KELEM_INPUT, KATTR_TYPE, "reset", KATTR_VALUE, "\u21BA", KATTR__MAX);
            // khtml_attr(&r, KELEM_INPUT, KATTR_TYPE, "reset", KATTR_VALUE, "\u21A9", KATTR__MAX);
            khtml_puts(&r, " ");
            khtml_attr(&r, KELEM_INPUT, KATTR_TYPE, "submit", KATTR_VALUE, "\U0001F50E", KATTR__MAX);
            // khtml_attr(&r, KELEM_INPUT, KATTR_TYPE, "submit", KATTR_VALUE, "\xF0\x9F\x94\x8E", KATTR__MAX);
          khtml_closeelem(&r, 1);

          // p.input.cj5
          khtml_elem(&r, KELEM_P); {
            struct kpair *const p=req->fieldmap[KEY_STRING];
            khtml_attr(&r, KELEM_INPUT,
              KATTR_TYPE, "search", // "text"
              KATTR_NAME, keys[KEY_STRING].name,
              KATTR_VALUE, p?p->val:"", // default value for input.reset
            KATTR__MAX);
            if(p){
              khtml_puts(&r, " result: ");
              assert(p->val && p->val==p->parsed.s);
              const size_t n=strnlen(p->val, 1024*8); assert(n<1024*8); assert(p->val[n-1]&&!p->val[n]);
              lookup(&r, p->val);
            }
          }; khtml_closeelem(&r, 1);

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

  return 0;

}
