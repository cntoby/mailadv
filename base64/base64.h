/**
 * Author: Toby Lee <cntoby@gmail.com>
 */
#ifndef _BASE64_H
#define _BASE64_H
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
long int base64_len(char *s, int type);
int base64_encode(const char *s, char *t, long int count);
int base64_decode(char *s, char *t, long int count);

#ifdef T_CHUNK_USE_LEN
char * chunk_split(const char *src, ...);
#else
char * chunk_split(const char *src, char *dest);
unsigned int chunk_split_len(const char *src);
#endif

#endif
