#!/bin/bash

{

cat <<"EOF"
#include <assert.h>
#include <stddef.h>
#include "./kre.h"

const D d[]={
EOF

   Rmax=0
   Emax=0
   Nmax=0
   R=0
   E=0
   N=0
   Tprev=0
   I=0
   while IFS= read -r i; do
      [[ "$i" =~ ^Cj([re])5{0,1}m-5{0,1}([a-wy])[0-9]*(-[0-9]|).svg$ ]] || { echo; echo "err: malformed '$i'"; break; }
      T="${BASH_REMATCH[1]}"
      K="${BASH_REMATCH[2]}"
      { [[ "$T" =~ [re] ]] && [[ "$K" = [a-wy] ]]; } || { echo; echo "err: ill match"; }
      if [ "$Tprev" = "$T" ]; then
         :
      elif [ "$Tprev" != r ] && [ "$T" = r ]; then
         if [ "$Tprev" = 0 ]; then
            printf "  {\n"
         else
            printf "NULL}\n"
            printf "  },{\n"
         fi
         printf "    .k='%s', // [%03d]\n" "$K" "$I"
         { ((N>Nmax)) && ((Nmax=N)); }; N=0
         R=0; ((E>Emax)) && ((Emax=E))
         ((++I))
         printf "    .r=(const char *const[]){"
      elif [ "$Tprev" = r ] && [ "$T" = e ]; then
         E=0; ((R>Rmax)) && ((Rmax=R))
         printf "NULL},\n"
         printf "    .e=(const char *const[]){"
      else
         echo
         printf "err Tprev=%s T=%s i=%s \n" "$Tprev" "$T" "$i"
         break
      fi
      printf '"%s",' "$i"
      ((++R));((++E));((++N))
      Tprev="$T"
   done < <(cut -d'/' -f3- paths.txt)
   printf "NULL}\n"
   printf "  }\n"
   printf "};\n"

cat <<EOF

const int dN=${I};
static_assert(${I}==sizeof(d)/sizeof(D));

const double rw=$((768/Rmax));
const double ew=$((768/Emax));
EOF

exec 1>&2
printf "\e[34m[%s]\e[0m %s * \e[34m%s\e[0m = %s\n" Rmax "$(bc<<<"scale=1;768/$Rmax")" "$Rmax" 768
printf "\e[34m[%s]\e[0m %s * \e[34m%s\e[0m = %s\n" Emax "$(bc<<<"scale=1;768/$Emax")" "$Emax" 768
printf "\e[34m[%s]\e[0m %s * \e[34m%s\e[0m = %s\n" Nmax "$(bc<<<"scale=1;768/$Nmax")" "$Nmax" 768

}; exit
