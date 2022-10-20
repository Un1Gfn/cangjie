// https://kristaps.bsd.lv/kcgi/
// sample.c
// https://kristaps.bsd.lv/kcgi/sample.c.html

#include <assert.h>
#include <stdarg.h> // va_list
#include <stddef.h> // NULL
#include <stdint.h> // int64_t
#include <stdlib.h> // EXIT_FAILURE atoi free
#include <string.h> // memset
#include <sys/types.h> // size_t, ssize_t

#include <kcgi.h>
#include <kcgihtml.h>

/*
 * Simple CGI application.
 * Compile it with `make samples` (or using gmake) and install it into
 * your web server's /cgi-bin.
 * The "template.xml" file should be in the /cgi-bin directory as well
 * and readable by the server process.
 * (Obviously this is just for a sample.)
 *
 * Assuming localhost/cgi-bin, the script is localhost/cgi-bin/sample.
 * The pages recognised are:
 *
 *   - /cgi-bin/sample/index.html
 *   - /cgi-bin/sample/template.html
 *   - /cgi-bin/sample/senddata.html
 *
 * See the sendindex et al. functions for what these do.
 */

/* Recognised page requests.  See pages[]. */
enum page {
  PAGE_INDEX,
  PAGE_TEMPLATE,
  PAGE_SENDDATA,
  PAGE__MAX
};

/*
 * All of the keys (input field names) we accept. 
 * The key names are in the "keys" array.
 * See sendindex() for how these are used.
 */
enum key {
  KEY_INTEGER, 
  KEY_FILE,
  KEY_PAGECOUNT,
  KEY_PAGESIZE,
  KEY__MAX
};

/*
 * The elements in our template file.
 * The element key names are in the "templs" array.
 * See sendtemplate() for how this is used.
 */
enum templ {
  TEMPL_TITLE,
  TEMPL_NAME,
  TEMPL_REMOTE_ADDR,
  TEMPL__MAX
};

/*
 * We need a structure because we can't get the "r" from the request.
 * This is used by our template callback.
 */
struct tstrct {
  struct khtmlreq req;
  struct kreq *r;
};

/*
 * We'll use this to route pages by creating an array indexed by our
 * page.
 * Then when the page is parsed, we'll route directly into it.
 */
typedef void (*disp)(struct kreq*);

static void senddata(struct kreq*);
static void sendindex(struct kreq*);
static void sendtemplate(struct kreq*);

static const disp disps[PAGE__MAX] = {
  sendindex,    // PAGE_INDEX
  sendtemplate, // PAGE_TEMPLATE
  senddata,     // PAGE_SENDDATA
};

static const struct kvalid keys[KEY__MAX] = {
  { kvalid_int, "integer" }, // KEY_INTEGER
  { NULL, "file" },          // KEY_FILE
  { kvalid_uint, "count" },  // KEY_PAGECOUNT
  { kvalid_uint, "size" },   // KEY_PAGESIZE
};

/*
 * Template key names (as in @@TITLE@@ in the file).
 */
static const char *const templs[TEMPL__MAX] = {
  "title",       // TEMPL_TITLE
  "name",        // TEMPL_NAME
  "remote_addr", // TEMPL_REMOTE_ADDR
};

/* 
 * Page names (as in the URL component) mapped from the first name part
 * of requests, e.g., /sample.cgi/index.html -> index -> PAGE_INDEX.
 */
static const char *const pages[PAGE__MAX] = {
  "index",    // PAGE_INDEX
  "template", // PAGE_TEMPLATE
  "senddata"  // PAGE_SENDDATA
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
 * Callback for filling in a particular template part.
 * Let's just be simple for simplicity's sake.
 */
static int template(size_t key, void *arg){
  struct tstrct *const p = arg;
  switch (key) {
    case TEMPL_TITLE:
      khtml_puts(&p->req, "title"); return 1; break;
    case TEMPL_NAME:
      khtml_puts(&p->req, "name"); return 1; break;
    case TEMPL_REMOTE_ADDR:
      khtml_puts(&p->req, p->r->remote); return 1; break;
    default:
      return 0;
  }
  assert(0);
}

/*
 * Demonstrates how to use templates.
 * Returns HTTP 200 and the template content.
 */
static void sendtemplate(struct kreq *const req){
  struct tstrct p={
    .r = req,
  };
  struct ktemplate t={
    .key = templs,
    .keysz = TEMPL__MAX,
    .arg = &p,
    .cb = template,
  };
  resp_open(req, KHTTP_200);
  khtml_open(&p.req, req, 0);
  khttp_template(req, &t, "template.xml");
  khtml_close(&p.req);
}

/*
 * Send a random amount of data.
 * Requires KEY_PAGECOUNT (optional), KEY_PAGESIZE (optional).
 * Page count is the number of times we flush a page (with the given
 * size) to the wire.
 * Returns HTTP 200 and the random data.
 */
static void senddata(struct kreq *req){

  int64_t nm = req->fieldmap[KEY_PAGECOUNT] ? req->fieldmap[KEY_PAGECOUNT]->parsed.i : 1024*1024;
  nm = nm ? nm : 1;

  int64_t sz = req->fieldmap[KEY_PAGESIZE] ? req->fieldmap[KEY_PAGESIZE]->parsed.i : 1;  
  sz = (sz&&(SIZE_MAX>=(uint64_t)sz)) ? sz : 1;

  char *const buf = kmalloc(sz);
  resp_open(req, KHTTP_200);
  for (int64_t i = 0; i < nm; i++) {
    for (int64_t j = 0; j < sz; j++)
      #ifndef __linux__
        buf[j] = 65 + arc4random_uniform(24);
      #else
        buf[j] = 65 + (random() % 24);
      #endif
    khttp_write(req, buf, sz);
  }
  free(buf);

}

/*
 * Demonstrates how to use GET and POST forms and building with the HTML
 * builder functions.
 * Returns HTTP 200 and HTML content.
 */
static void sendindex(struct kreq *req){

  const char *cp = req->fieldmap[KEY_INTEGER] ? req->fieldmap[KEY_INTEGER]->val : "";

  char *page=NULL;
  kasprintf(&page, "%s/%s", req->pname, pages[PAGE_INDEX]);

  struct khtmlreq r={};
  resp_open(req, KHTTP_200);
  khtml_open(&r, req, 0);
  khtml_elem(&r, KELEM_DOCTYPE);
  khtml_elem(&r, KELEM_HTML);
  khtml_elem(&r, KELEM_HEAD);
  khtml_elem(&r, KELEM_TITLE);
  khtml_puts(&r, "Welcome!");
  khtml_closeelem(&r, 2);
  khtml_elem(&r, KELEM_BODY);
  khtml_elem(&r, KELEM_BR);

  void addlink(const char *const href, const char *const label){
    assert(KCGI_OK==khtml_attr(&r, KELEM_A, KATTR_HREF, href, KATTR__MAX));
    assert(KCGI_OK==khtml_puts(&r, label));
    assert(KCGI_OK==khtml_closeelem(&r, 1));
    khtml_elem(&r, KELEM_BR);
  }
  addlink("/cgi-bin/02-sample.cgi/index.html",    ".cgi/index");
  addlink("/cgi-bin/02-sample.cgi/template.html", ".cgi/template");
  addlink("/cgi-bin/02-sample.cgi/senddata.html", ".cgi/senddata");
  khtml_elem(&r, KELEM_BR);

  khtml_puts(&r, "Welcome!");

  khtml_attr(&r, KELEM_FORM,
    KATTR_METHOD, "post",
    KATTR_ENCTYPE, "multipart/form-data",
    KATTR_ACTION, page,
    KATTR__MAX);
  khtml_elem(&r, KELEM_FIELDSET);
  khtml_elem(&r, KELEM_LEGEND);
  khtml_puts(&r, "Post (multipart)");
  khtml_closeelem(&r, 1);
  khtml_elem(&r, KELEM_P);
  khtml_attr(&r, KELEM_INPUT,
    KATTR_TYPE, "number",
    KATTR_NAME, keys[KEY_INTEGER].name,
    KATTR_VALUE, cp, KATTR__MAX);
  khtml_closeelem(&r, 1);
  khtml_elem(&r, KELEM_P);
  khtml_attr(&r, KELEM_INPUT,
    KATTR_TYPE, "file",
    KATTR_MULTIPLE, "",
    KATTR_NAME, keys[KEY_FILE].name,
    KATTR__MAX);

  if (NULL != req->fieldmap[KEY_INTEGER]) {
    // khtml_puts(&r, "integer: ");
    // khtml_puts(&r, req->fieldmap[KEY_INTEGER]->val);
    khtml_puts(&r, "incr: ");
    khtml_printf(&r, "%d", 1+atoi(req->fieldmap[KEY_INTEGER]->val));
    khtml_puts(&r, " ");
  }
  if (NULL != req->fieldmap[KEY_FILE]) {
    if (NULL != req->fieldmap[KEY_FILE]->file) {
      khtml_puts(&r, "file: ");
      khtml_puts(&r, req->fieldmap[KEY_FILE]->file);
      khtml_puts(&r, " ");
    } 
    if (NULL != req->fieldmap[KEY_FILE]->ctype) {
      khtml_puts(&r, "ctype: ");
      khtml_puts(&r, req->fieldmap[KEY_FILE]->ctype);
    } 
  }

  khtml_closeelem(&r, 1);
  khtml_elem(&r, KELEM_P);
  khtml_attr(&r, KELEM_INPUT,
    KATTR_TYPE, "submit",
    KATTR__MAX);
  khtml_closeelem(&r, 0);
  khtml_close(&r);

  free(page); page=NULL;

}

int main(){

  struct kreq r={};

  // Set up our main HTTP context.
  assert(KCGI_OK==khttp_parse(&r, keys, KEY__MAX, pages, PAGE__MAX, PAGE_INDEX));

  /* 
   * Accept only GET, POST, and OPTIONS.
   * Restrict to text/html and a valid page.
   * If all of our parameters are valid, use a dispatch array to
   * send us to the page handlers.
   */
  if (KMETHOD_OPTIONS == r.method) {
    khttp_head(&r, kresps[KRESP_ALLOW], "OPTIONS GET POST");
    resp_open(&r, KHTTP_200);
  }else if(KMETHOD_GET != r.method && KMETHOD_POST != r.method){
    resp_open(&r, KHTTP_405);
  }else if(PAGE__MAX == r.page ||  KMIME_TEXT_HTML != r.mime){
    resp_open(&r, KHTTP_404);
  }else
    (*disps[r.page])(&r);

  khttp_free(&r);
  return 0;

}
