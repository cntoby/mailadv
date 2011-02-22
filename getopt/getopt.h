/**
 * Author: toby <cntoby@gmail.com>
 * get command line option simple
 * Example:
 * 1. Include this header file.
 * 2. In the main(int argc, char **argv), if your option have key and value,
 *    you must set options like this, if your options are not have value,you
 *    can skip this.
 *    t_option args[] = {{'f', "first", "", TOPT_HAS_VAL},
 *                   {'s', "second", "", TOPT_HAS_VAL},
 *                   {0, "", "", 0}
 *      };
 * 3. Initiliztion arguments.
 *    init_opt(argc, argv, args);
 *    If all options have no value, you can
 *    init_opt(argc, argv);
 * 4. Get option
 *    t_option *tmpopt = (t_option *)NULL;
 *    tmpopt = get_opt("f"); or tmpopt = get_opt("from");
 * 5. Use option
 *    Check if option is set: TOPT_ISSET(tmpopt);
 *    Check if option has value: TOPT_ISSETV(tmpopt);
 *    Use option's value: char *v = tmpopt->value;
 * 6. Free memory
 *    free_opt();
 */
#ifndef _T_GETOPT_H
#define _T_GETOPT_H
#include <stdio.h>
#include <stdlib.h>

#define CHECKOPT(toption)       ((toption->sname>0)?1:0)                /* check if toption was set or preset */
#define TOPT_ISSET(toption)     ((toption->isset)&1)                    /* check if toption was set */
#define TOPT_ISSETV(toption)    ((toption->isset)&TOPT_HAS_VAL)         /* check if toption has value */
#define TOPT_SET(toption)       ((toption->isset) = (toption->isset)|1) /* set toption */

#define _T_MAX_NLEN 20
#define _T_MAX_VLEN 80
#define TOPT_NO_VAL 0
#define TOPT_HAS_VAL 2

typedef struct t_option {
    char sname;
    char longname[_T_MAX_NLEN+1];
    char value[_T_MAX_VLEN+1];
    short isset:2;
} t_option;

int init_opt(int argc, char *argv[], ...);
t_option * get_opt(const char *name);
void free_opt(void);
#endif
