#ifndef DEBUG_H
#define DEBUG_H

#ifdef DEBUG_ENABLED
  #define DEBUG(X) X
#else
  #define DEBUG(X) ;  
#endif

#endif
