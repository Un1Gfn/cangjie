# #include <uchar.h>
# -fextended-identifiers for \unnnn and \U00nnnnnn notations
CFLAGS:=-std=gnu17 -g -O0 -Wextra -Wall -Winline -Wshadow -fanalyzer -fextended-identifiers
LDLIBS:=-lgdbm
