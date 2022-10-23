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


// <a href="$h">$l</a>
static void khtml_attr_a_2(struct khtmlreq *const r, const char *const h, const char *const l){
  assert(KCGI_OK==khtml_attr(r, KELEM_A, KATTR_HREF, h, KATTR__MAX));
  assert(KCGI_OK==khtml_puts(r, l));
  assert(KCGI_OK==khtml_closeelem(r, 1));
  khtml_elem(r, KELEM_BR);
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
    }; khtml_closeelem(&r, 1); // </head>

    khtml_elem(&r, KELEM_BODY); {

      khtml_elem(&r, KELEM_BR);

      khtml_attr_a_2(&r, "/cgi-bin/cj5.cgi/defpage", "clear");
      khtml_elem(&r, KELEM_BR);

      khtml_puts(&r, "Welcome!");
      khtml_elem(&r, KELEM_BR);
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

          khtml_elem(&r, KELEM_P); {
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

          khtml_elem(&r, KELEM_P); {
            khtml_attr(&r, KELEM_INPUT,
              KATTR_TYPE, "submit",
              KATTR__MAX);
            khtml_closeelem(&r, 1);
          }

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

// struct kpair defined in /usr/include/kcgi.h
// env MANPAGER="less -R +% -J -j.5 -p 'char \*ctype'" man khttp_parse.3
void inspect_kpair(const struct kpair *const p){

  assert(p&&!p->next); // fprintf(stderr, "next@%p ", p->next);
  assert(KEY_INTEGER==p->keypos); // fprintf(stderr, "keypos=%zu ", p->keypos);
  assert(0==strcmp("rvkjm9pqcg8o3", p->key)); // fprintf(stderr, "key=%s ", p->key);

  // MIME content transfer encoding (e.g. base64)
  assert(p->xcode&&!(p->xcode[0])); // fprintf(stderr, "xcode@%p ", p->xcode);
  assert(p->file&&!(p->file[0])); // fprintf(stderr, "file@%p ", p->file);

  {

    assert(KPAIR_INTEGER==p->type); // fprintf(stderr, "type=%d ", p->type);
    assert(KMIME_TEXT_PLAIN==p->ctypepos); // fprintf(stderr, "ctypepos=%zu ", p->ctypepos);
    assert(0==strcmp("text/plain", p->ctype)); // fprintf(stderr, "ctype=%s ", p->ctype);
    assert(KPAIR_VALID==p->state); // fprintf(stderr, "state=%d ", p->state);

    assert(p->valsz==strlen(p->val)); // fprintf(stderr, "valsz=%zu ", p->valsz);
    fprintf(stderr, "val='%s' ", p->val);
    fprintf(stderr, "parsed=%ld ", p->parsed.i);
    fprintf(stderr, "\n");

    for(size_t i=0; i<p->valsz; ++i)
      fprintf(stderr, "   %c ", p->val[i]);
    fprintf(stderr, "\n");
    for(size_t i=0; i<p->valsz; ++i)
      fprintf(stderr, "0x%02X ", p->val[i]);

  }

}

void inspect_kreq(const struct kreq *const r){

  // misc
  assert(!r->arg);
  assert(r->kdata);
  assert(keys==r->keys);
  assert(KEY__MAX==r->keysz);
  assert(KMIME_TEXT_HTML==r->mime);

  // tcp
  fprintf(stderr, ": HTTP_HOST=%s\n", r->host);
  fprintf(stderr, ": REMOTE_ADDR=%s\n", r->remote);
  assert(80u==r->port); // fprintf(stderr, ": SERVER_PORT=%u\n", r->port);

  // authentication
  assert(KAUTH_NONE==r->auth);
  assert(0==memcmp(&(struct khttpauth){}, &(r->rawauth), sizeof(struct khttpauth)));

  // request headers
  assert(KSCHEME_HTTP==r->scheme);
  assert(r->suffix&&!(r->suffix[0])); // fprintf(stderr, ": suffix@%p\n", r->suffix);
  assert(3<=r->reqsz); // fprintf(stderr, ": reqsz=%zu\n", r->reqsz);
  for(size_t i=0; i<r->reqsz; ++i){
    fprintf(stderr, "*%26s = %s\n", r->reqs[i].key, r->reqs[i].val);
  }

  // POST/GET request
  assert(KMETHOD_OPTIONS==r->method||KMETHOD_GET==r->method||KMETHOD_POST==r->method);
  assert(r->fieldnmap);
  assert(r->fieldmap);
  size_t cur=0;
  for(int i=0; i<KEY__MAX; ++i){
    assert(!r->fieldnmap[i]);
    fprintf(stderr, ": [%d] %s ", i, "fieldmap");
    if(r->fieldmap[i]){
      inspect_kpair(r->fieldmap[i]);
      assert(r->fieldsz>cur);
      assert(r->fieldmap[i]==&(r->fields[cur++])); // QUERY_STRING r->fields r->fieldsz ?
    }else{
      fprintf(stderr, "NULL\n");
    }
  }
  assert(r->fieldsz==cur);

  // path
  assert(PAGE_DEFPAGE==r->page);
  assert(r->pname&&0==strcmp("/cgi-bin/cj5.cgi", r->pname)); // fprintf(stderr, ": SCRIPT_NAME='%s'\n", r->pname);
  assert(r->pagename); // fprintf(stderr, ": pagename='%s'\n", r->pagename);
  assert(r->fullpath); // fprintf(stderr, ": PATH_INFO='%s'\n", r->fullpath);
  assert(r->pagename&&r->fullpath);
  assert(
    ((!(r->pagename[0])&&!(r->fullpath[0]))) ||
    (0==strcmp("defpage", r->pagename)&&0==strcmp("/defpage", r->fullpath)) ||
  false);

  // cookie
  assert(!r->cookies);
  assert(!r->cookiesz);
  assert(r->cookiemap); assert(r->cookienmap);
  for(int i=0; i<KEY__MAX; ++i){
    assert(!r->cookiemap[i]); assert(!r->cookienmap[i]);
  }

}

int main(){

  fprintf(stderr, "\n");
  fprintf(stderr, ": main.c\n");

  struct kreq req={/*.port=9513u*/};

  // initialize main http context
  assert(KCGI_OK==khttp_parse(
    &req,                          // input fields and HTTP context parsed from the CGI environment
    keys, KEY__MAX,                // input and validation fields
    pages, PAGE__MAX, PAGE_DEFPAGE // recognised pathnames and fallback page if no default landing page is specified
  ));
  // req.port=9513u;

  inspect_kreq(&req);

  // accept HTTP method GET/POST/OPTIONS only
  switch(req.method){
    case KMETHOD_OPTIONS:
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
