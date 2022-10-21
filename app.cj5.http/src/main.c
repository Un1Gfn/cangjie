// https://kristaps.bsd.lv/kcgi/
// --> sample.c
// --> https://kristaps.bsd.lv/kcgi/sample.c.html

// include.base
#include <assert.h>
#include <stdlib.h> // free EXIT_FAILURE
#include <string.h> // memset
#include <stdio.h> // fprintf(stderr, ...)

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
  PAGE__MAX
};

/*
 * All of the keys (input field names) we accept. 
 * The key names are in the "keys" array.
 * See sendindex() for how these are used.
 */
enum key {
  KEY_INTEGER, 
  KEY__MAX
};

static const struct kvalid keys[KEY__MAX] = {
  { kvalid_int, "rvkjm9pqcg8o3" }, // KEY_INTEGER
};

/* 
 * Page names (as in the URL component) mapped from the first name part
 * of requests, e.g., /sample.cgi/defpage.html -> defpage -> PAGE_DEFPAGE.
 */
static const char *const pages[PAGE__MAX] = {
  "defpage", // PAGE_DEFPAGE
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
  enum kmime mime = (KMIME__MAX==req->mime) ? KMIME_APP_OCTET_STREAM : req->mime;
  khttp_head(req, kresps[KRESP_STATUS], "%s", khttps[http]);
  khttp_head(req, kresps[KRESP_CONTENT_TYPE], "%s", kmimetypes[mime]);
  khttp_body(req);
}

/*
 * Demonstrates how to use GET and POST forms and building with the HTML
 * builder functions.
 * Returns HTTP 200 and HTML content.
 */
static void sendindex(struct kreq *req){

  char *page=NULL;
  kasprintf(&page, "%s/%s", req->pname, pages[PAGE_DEFPAGE]);

  struct khtmlreq r={};
  resp_open(req, KHTTP_200);
  khtml_open(&r, req, 0);
  khtml_elem(&r, KELEM_DOCTYPE);
  khtml_elem(&r, KELEM_HTML);{

    khtml_elem(&r, KELEM_HEAD);{
      khtml_elem(&r, KELEM_TITLE); khtml_puts(&r, "Welcome!"); khtml_closeelem(&r, 1);
      khtml_closeelem(&r, 1);
    } // </head>

    khtml_elem(&r, KELEM_BODY);{

      khtml_elem(&r, KELEM_BR);

      void addlink(const char *const href, const char *const label){
        assert(KCGI_OK==khtml_attr(&r, KELEM_A, KATTR_HREF, href, KATTR__MAX));
        assert(KCGI_OK==khtml_puts(&r, label));
        assert(KCGI_OK==khtml_closeelem(&r, 1));
        khtml_elem(&r, KELEM_BR);
      }
      addlink("/cgi-bin/cj5.cgi/defpage", "clear");
      khtml_elem(&r, KELEM_BR);

      khtml_puts(&r, "Welcome!");
      khtml_elem(&r, KELEM_BR);
      khtml_elem(&r, KELEM_BR);

      khtml_attr(&r, KELEM_FORM,
        KATTR_METHOD, "post",
        KATTR_ENCTYPE, "multipart/form-data",
        KATTR_ACTION, page,
        KATTR__MAX
      ); {

        khtml_elem(&r, KELEM_FIELDSET);{

          khtml_elem(&r, KELEM_LEGEND); khtml_puts(&r, "Post (multipart)"); khtml_closeelem(&r, 1);

          khtml_elem(&r, KELEM_P);{
            khtml_attr(&r, KELEM_INPUT,
              KATTR_TYPE, "number",
              KATTR_NAME, keys[KEY_INTEGER].name,
              KATTR_VALUE, (req->fieldmap[KEY_INTEGER] ? req->fieldmap[KEY_INTEGER]->val : ""), KATTR__MAX
            );
            if(req->fieldmap[KEY_INTEGER]){
              khtml_puts(&r, " incr: ");
              khtml_printf(&r, "%ld", 1+req->fieldmap[KEY_INTEGER]->parsed.i);
            }
            khtml_closeelem(&r, 1);
          }

          khtml_elem(&r, KELEM_P);{
            khtml_attr(&r, KELEM_INPUT,
              KATTR_TYPE, "submit",
              KATTR__MAX);
            khtml_closeelem(&r, 1);
          }

          khtml_closeelem(&r, 1);

        } // </fieldset>

        khtml_closeelem(&r, 1);

      } // </form>

      khtml_closeelem(&r, 1);

    } // </body>

    khtml_closeelem(&r, 1);

  } // </html>

  assert(0==khtml_elemat(&r));
  khtml_close(&r);

  free(page); page=NULL;

}

/*
 * We'll use this to route pages by creating an array indexed by our
 * page.
 * Then when the page is parsed, we'll route directly into it.
 */
static void (*const disps[PAGE__MAX])(struct kreq*) = {
  sendindex, // PAGE_DEFPAGE
};

// struct kpair defined in /usr/include/kcgi.h
void inspect_kpair(const struct kpair *const p){
  assert(p);
  assert(!p->keypos); // fprintf(stderr, "keypos=%zu ", p->keypos);
  assert(0==strcmp("rvkjm9pqcg8o3", p->key)); // fprintf(stderr, "key=%s ", p->key);
  assert(!p->next); // fprintf(stderr, "next@%p ", p->next);
  {
    assert(KPAIR_INTEGER==p->type); // fprintf(stderr, "type=%d ", p->type);
    assert(0==strcmp("text/plain", p->ctype)); // fprintf(stderr, "ctype=%s ", p->ctype);
    assert(KPAIR_VALID==p->state); // fprintf(stderr, "state=%d ", p->state);
    assert(p->valsz==strlen(p->val)); // fprintf(stderr, "valsz=%zu ", p->valsz);
    fprintf(stderr, "val='%s' ", p->val);
    fprintf(stderr, "parsed=%ld ", p->parsed.i);
  }
  fprintf(stderr, "file@%p ", p->file);
  assert(14==p->ctypepos); // fprintf(stderr, "ctypepos=%zu ", p->ctypepos);
  fprintf(stderr, "xcode@%p ", p->xcode);
  fprintf(stderr, "\n");
}

// void inspect_kreq(const struct kreq *const r){

// }

int main(){

  struct kreq req={};
  // req.port=9513u;
  // main HTTP context.
  assert(KCGI_OK==khttp_parse(
    &req,                          // input fields and HTTP context parsed from the CGI environment
    keys, KEY__MAX,                // input and validation fields
    pages, PAGE__MAX, PAGE_DEFPAGE // recognised pathnames and fallback page if no default landing page is specified
  ));
  // req.port=9513u;

  // private application data
  assert(!req.arg);

  // auto
  assert(KAUTH_NONE==req.auth);

  // cookie
  assert(req.cookiemap); assert(req.cookienmap);
  for(int i=0; i<KEY__MAX; ++i){
    assert(!req.cookiemap[i]); assert(!req.cookienmap[i]);
  }
  assert(!req.cookies);
  assert(!req.cookiesz);

  assert(req.fieldnmap);
  assert(req.fieldmap);
  size_t cur=0;
  for(int i=0; i<KEY__MAX; ++i){
    assert(!req.fieldnmap[i]);
    fprintf(stderr, ": [%d] %s ", i, "fieldmap");
    if(req.fieldmap[i]){
      inspect_kpair(req.fieldmap[i]);
      assert(req.fieldsz>cur);
      assert(req.fieldmap[i]==&req.fields[cur++]); // QUERY_STRING req.fields req.fieldsz ?
    }else{
      fprintf(stderr, "NULL\n");
    }
  }
  assert(req.fieldsz==cur);

  // PATH_INFO
  fprintf(stderr, ": fullpath=%s\n", req.fullpath);

  // accept HTTP method GET/POST/OPTIONS only
  switch(req.method){
    case KMETHOD_OPTIONS:
      khttp_head(&req, kresps[KRESP_ALLOW], "OPTIONS GET POST");
      resp_open(&req, KHTTP_200); // 200 OK
      break;
    case KMETHOD_GET:
    case KMETHOD_POST:
      // restrict to text/html and a valid page
      if(PAGE__MAX <= req.page || KMIME_TEXT_HTML != req.mime)
        resp_open(&req, KHTTP_404); // 404 Not Found
      else
        (*disps[req.page])(&req); // dispatch array to send to the page handlers
      break;
    default:
      resp_open(&req, KHTTP_405); // 405 Method Not Allowed
      break;
  }

  khttp_free(&req); req=(struct kreq){};

  return 0;

}
