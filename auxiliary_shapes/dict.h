#pragma once

typedef struct {
  const char k; // radical
  const char *const *const r; // auxilary shapes
  const char *const *const e; // examples
} D;

extern const D d[];
extern const int dN;

extern const double rw;
extern const double ew;
