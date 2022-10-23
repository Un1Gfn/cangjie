
// <a href="$h">$l</a>
static void a(struct khtmlreq *const r, const char *const h, const char *const l){
  assert(KCGI_OK==khtml_attr(r, KELEM_A, KATTR_HREF, h, KATTR__MAX));
  assert(KCGI_OK==khtml_puts(r, l));
  assert(KCGI_OK==khtml_closeelem(r, 1));
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
  size_t Nvalid=0;
  size_t Ninvalid=0;
  for(int i=0; i<KEY__MAX; ++i){
    fprintf(stderr, ": fieldmap[%d] ", i);
    if(r->fieldmap[i]){
      // inspect_kpair(r->fieldmap[i]);
      assert(r->fieldsz>Nvalid);
      assert(r->fieldmap[i]==&(r->fields[Nvalid++])); // QUERY_STRING r->fields r->fieldsz ?
      assert(!r->fieldnmap[i]);
    }else{
      fprintf(stderr, "NULL\n");
      if(r->fieldnmap[i]){
        assert(r->fieldnmap[i] && KPAIR_INVALID==r->fieldnmap[i]->state);
        ++Ninvalid;
      }
    }
  }
  assert(r->fieldsz==Nvalid+Ninvalid);

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
    fprintf(stderr, "\n");

  }

}
