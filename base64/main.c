/**
 * Author: Toby Lee <cntoby@gmail.com>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "base64/base64.h"

int main(int argc, char *argv[])
{
    long int sl = 0, retl = 0;
    char *ret = (char *)NULL, type = 0;
    if (argc!=3) {
        printf("Usage: %s -<e|d> string\n", argv[0]);
        return -1;
    } else {
        if(argv[1][0]=='-') {
            type = argv[1][1];
        }else {
            type = argv[1][0];
        }
        if(type=='e') type = 1;
        else if(type=='d') type = 2;
        else type = 0;

        if(type==0) {
            printf("Operator Type Error.\nUsage: %s -<e|d> string\n", argv[0]);
            return -1;
        }

        sl = base64_len(argv[2], type-1);
        sl = (sl < 20)?20:sl;
        ret = (char *)malloc(sl);
        if(type==1)
            retl = base64_encode(argv[2], ret, sl);
        else 
            retl = base64_decode(argv[2], ret, sl);
        if(retl>0) {
            printf("Source: %s\nBase64: %s\nMalloc: %ld Used: %ld\n",argv[2], ret, sl, retl);
        }else {
            printf("Source: %s\nError!!!\n", argv[2]);
        }
        free(ret);
    }
    return 0;
}
