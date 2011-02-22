#ifndef T_SENDMAIL_H
#define T_SENDMAIL_H
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#define T_SSMTP      1
#define T_HTML       2
#define T_HEADER     4

#define T_EMOK       0
#define T_EMERR     -1
#define T_EMADDRE   -2
#define T_EMEXTRA_H -3
#define T_EMEXTRA_S -4
#define T_EMOPENSM  -5

#define SENDMAIL_PATH   "/usr/sbin/sendmail -t -i"
#ifdef T_WITH_FROM
int sendmail(char *to, char *from, char *subject, char *body, short extra, ...);
#else
int sendmail(char *to, char *subject, char *body, short extra, ...);
#endif
#endif /* T_SENDMAIL_H */
