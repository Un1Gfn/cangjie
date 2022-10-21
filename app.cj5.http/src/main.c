// https://kristaps.bsd.lv/kcgi/
// --> sample.c
// --> https://kristaps.bsd.lv/kcgi/sample.c.html

// include.base
#include <assert.h>
#include <stdlib.h> // EXIT_FAILURE atoi free
#include <string.h> // memset

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
  PAGE_INDEX,
  PAGE__MAX
};

/*
 * All of the keys (input field names) we accept. 
 * The key names are in the "keys" array.
 * See sendindex() for how these are used.
 */
enum key {
  KEY_INTEGER, 
  KEY_PAGECOUNT,
  KEY_PAGESIZE,
  KEY__MAX
};

/*
 * We'll use this to route pages by creating an array indexed by our
 * page.
 * Then when the page is parsed, we'll route directly into it.
 */
typedef void (*disp)(struct kreq*);

static void sendindex(struct kreq*);

static const disp disps[PAGE__MAX] = {
  sendindex,    // PAGE_INDEX
};

static const struct kvalid keys[KEY__MAX] = {
  { kvalid_int, "integer" }, // KEY_INTEGER
  { kvalid_uint, "count" },  // KEY_PAGECOUNT
  { kvalid_uint, "size" },   // KEY_PAGESIZE
};

/* 
 * Page names (as in the URL component) mapped from the first name part
 * of requests, e.g., /sample.cgi/index.html -> index -> PAGE_INDEX.
 */
static const char *const pages[PAGE__MAX] = {
  "index",    // PAGE_INDEX
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
  kasprintf(&page, "%s/%s", req->pname, pages[PAGE_INDEX]);

  struct khtmlreq r={};
  resp_open(req, KHTTP_200);
  khtml_open(&r, req, 0);{

    khtml_elem(&r, KELEM_DOCTYPE);

    khtml_elem(&r, KELEM_HTML);

    khtml_elem(&r, KELEM_HEAD);{
      khtml_elem(&r, KELEM_TITLE);{
        khtml_puts(&r, "Welcome!");
        khtml_closeelem(&r, 2);
      }
    }

    khtml_elem(&r, KELEM_BODY);{

      khtml_elem(&r, KELEM_BR);

      void addlink(const char *const href, const char *const label){
        assert(KCGI_OK==khtml_attr(&r, KELEM_A, KATTR_HREF, href, KATTR__MAX));
        assert(KCGI_OK==khtml_puts(&r, label));
        assert(KCGI_OK==khtml_closeelem(&r, 1));
        khtml_elem(&r, KELEM_BR);
      }
      addlink("/cgi-bin/cj5.cgi/index.html", ".cgi/index");
      khtml_elem(&r, KELEM_BR);

      khtml_puts(&r, "Welcome!");
      khtml_elem(&r, KELEM_BR);
      khtml_elem(&r, KELEM_BR);

      khtml_attr(&r, KELEM_FORM,
        KATTR_METHOD, "post",
        KATTR_ENCTYPE, "multipart/form-data",
        KATTR_ACTION, page,
        KATTR__MAX);
      khtml_elem(&r, KELEM_FIELDSET);
      khtml_elem(&r, KELEM_LEGEND); khtml_puts(&r, "Post (multipart)"); khtml_closeelem(&r, 1);

      khtml_elem(&r, KELEM_P);{

        khtml_attr(&r, KELEM_INPUT,
          KATTR_TYPE, "number",
          KATTR_NAME, keys[KEY_INTEGER].name,
          KATTR_VALUE, (req->fieldmap[KEY_INTEGER] ? req->fieldmap[KEY_INTEGER]->val : ""), KATTR__MAX
        );

        if(req->fieldmap[KEY_INTEGER]){
          khtml_puts(&r, " incr: ");
          khtml_printf(&r, "%d", 1+atoi(req->fieldmap[KEY_INTEGER]->val));
          // khtml_puts(&r, "");
        }

        khtml_closeelem(&r, 1);

      }

      khtml_elem(&r, KELEM_P);{
        khtml_attr(&r, KELEM_INPUT,
          KATTR_TYPE, "submit",
          KATTR__MAX);
        khtml_closeelem(&r, 0);
      }

    }

    khtml_close(&r);
  }

  free(page); page=NULL;

}

int main(){

  struct kreq req={};

  // Set up our main HTTP context.
  assert(KCGI_OK==khttp_parse(&req, keys, KEY__MAX, pages, PAGE__MAX, PAGE_INDEX));

  /* 
   * Accept only GET, POST, and OPTIONS.
   * Restrict to text/html and a valid page.
   * If all of our parameters are valid, use a dispatch array to
   * send us to the page handlers.
   */
  switch(req.method){
    case KMETHOD_OPTIONS:
      khttp_head(&req, kresps[KRESP_ALLOW], "OPTIONS GET POST");
      resp_open(&req, KHTTP_200); // 200 OK
      break;
    case KMETHOD_GET:
    case KMETHOD_POST:
      if(PAGE__MAX <= req.page || KMIME_TEXT_HTML != req.mime)
        resp_open(&req, KHTTP_404); // 404 Not Found
      else
        (*disps[req.page])(&req);
      break;
    default:
      resp_open(&req, KHTTP_405); // 405 Method Not Allowed
      break;
  }

  khttp_free(&req);
  return 0;

}
