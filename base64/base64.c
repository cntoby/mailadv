/**
 * Author: Toby Lee <cntoby@gmail.com>
 */
#include "base64.h"
char keyStr[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                 'w', 'x', 'y', 'z', '0', '1', '2', '3',
                 '4', '5', '6', '7', '8', '9', '+', '/',
                 '='
                };
char idxArr[] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                 -1, -1, -1,
                 62, -1, -1, -1, 63, 52, 53, 54, 55, 56,
                 57, 58, 59, 60, 61, -1, -1, -1, 64, -1,
                 -1, -1,  0,  1,  2,  3,  4,  5,  6,  7,
                  8,  9, 10, 11, 12, 13, 14, 15, 16, 17,
                 18, 19, 20, 21, 22, 23, 24, 25, -1, -1,
                 -1, -1, -1, -1, 26, 27, 28, 29, 30, 31,
                 32, 33, 34, 35, 36, 37, 38, 39, 40, 41,
                 42, 43, 44, 45, 46, 47, 48, 49, 50, 51
                };

void _clear_v(const char *s, char *t);

long int base64_len(char *s, int type)
{
    long int t;
    t = strlen(s);
    if ( type == 0)
        t = t / 3 * 4 +10;
    else
        t = t / 4 * 3;
    return t;
}

int base64_encode(const char *s, char *t, long int count)
{
    unsigned char *current = (unsigned char *)s;
    unsigned int i = 0, slen;
    slen = strlen(s);
    while (slen > 2) {
        t[i++] = keyStr[current[0] >> 2];
        t[i++] = keyStr[((current[0] & 0x03)<<4) | (current[1] >> 4)];
        t[i++] = keyStr[((current[1] & 0x0F)<<2) | (current[2] >> 6)];
        t[i++] = keyStr[current[2] & 0x3F];
        slen -= 3;
        current += 3;
    }
    if (slen != 0) {
        t[i++] = keyStr[current[0] >> 2];
        if (slen > 1) {
            t[i++] = keyStr[((current[0] & 0x03) << 4) | (current[1] >> 4)];
            t[i++] = keyStr[(current[1] &0x0F) << 2];
            t[i++] = '=';
        }else {
            t[i++] = keyStr[(current[0] & 0x03) << 4];
            t[i++] = '=';
            t[i++] = '=';
        }
    }
    t[i] = '\0';
    return i;
}

int base64_decode(char *s, char *t, long int count)
{
    unsigned char c1, c2, c3, e1, e2, e3, e4;
    unsigned int i = 0, sl = 0, x = 0;
    char *tmps = (char *)NULL, *tmpsc = (char *)NULL;
    if ((sl = strlen(s)) < 3) return -1;
    if (sl%4==3) sl += 2;
    tmps = (char *)malloc(sl);
    tmpsc = tmps;

    _clear_v(s,tmps); /* Clear chars which is not in base64 letters.*/

    do {
        e1 = idxArr[(int)*tmps++];
        e2 = idxArr[(int)*tmps++];
        e3 = idxArr[(int)*tmps++];
        e4 = idxArr[(int)*tmps++];

        c1 = (e1 << 2) | (e2 >> 4);
        c2 = ((e2 & 0x0F) << 4) | (e3 >> 2);
        c3 = ((e3 & 0x03) << 6) | e4;
        t[i++] = c1;
        if(e3!=64) t[i++] = c2;
        if(e4!=64) t[i++] = c3;
        c1 = c2 = c3 = e1 = e2 = e3 = e4 = 0;
        x += 4;
    } while(*tmps && x < (sl-3));
    tmps = tmpsc;
    free(tmps);

    return i;
}

void _clear_v (const char *s, char *t)
{
    unsigned int eqc = 0, sl = 0, mx = 0;
    do {/* drop the letters which is not in base64 */
        if(*s=='/'||*s=='='||*s=='+'||(*s>='A'&&*s<='Z')||(*s>='a'&&*s<='z')||(*s>='0'&&*s<='9'))
            *t++ = *s++;
        else
            s++;
        if ( *s=='=')
            eqc++;
        sl++;
    }while(*s);
    if(eqc>2) {
        t -= eqc - 2;
        sl -= eqc -2;
        eqc = 2;
    }
    mx = sl%4;
    t -= (mx&&eqc>=mx)?mx:0; /* the valiable base64 is can be divided by 4 with no remainder  */
    sl -= mx;
    if(mx==3&&eqc<2) *t++ = '=';
    *t = '\0';
}
/**
 * Split the string, Default length 76
 */
#ifdef T_CHUNK_USE_LEN
char * chunk_split(const char *src, ...)
#else
char * chunk_split(const char *src, char *dest)
#endif
{
    char end[] = "\r\n", *s, *d;
    short int endlen = 2;
    unsigned int slen = 0, clen = 76, restlen = 0;

#ifdef T_CHUNK_USE_LEN
    char *endtmp = (char *)NULL;
    va_list arg_ptr;
    va_start(arg_ptr, src);
    newlen = va_arg(arg_ptr, int);
    clen = newlen>0 ? newlen :clen;
    newlen = 0;

    endtmp = va_arg(arg_ptr, char *);
    va_end(arg_ptr);

    if(strlen(endtmp)>0) {
        strncpy(end, endtmp, 2);
        endlen = strlen(end);
    }
#endif

    slen = strlen(src);
    if (clen >= slen) return (char *)src;
    /*clen = (chunklen>0) ? chunklen : clen;*/

    restlen = slen % clen;
    /**
    newlen = (slen/clen - ((restlen==0)?1:0)) * endlen + slen + 1;
    dest = (char *)malloc(newlen);
    if (!dest) return (char *)NULL;
    */

    for ( s = (char *)src, d = (char *)dest; s < (src + slen - clen + 1) ; ) {
        memcpy(d, s, clen);
        d += clen;
        memcpy(d, end, endlen);
        d += endlen;
        s += clen;
    }
    if (restlen) {
        memcpy(d, s, restlen);
        d += restlen;
    }
    *d = '\0';
    return(dest);
}

unsigned int chunk_split_len(const char *src)
{
    int endlen = 2, slen, clen = 76;
    slen = strlen(src);
    return((slen/clen - ((slen%clen==0)?1:0)) * endlen + slen + 1);
}
