#include "getopt.h"
#include <stdarg.h>
#include <string.h>

void _emptyopts(int count);
int _tsetval(char *k, char *v, short islname);

t_option *opts = (t_option *)NULL; /* variables is just can defined out of function, i think so. */
/*opts = (t_option *)calloc(10, sizeof(t_option) );*/
/*t_option *opts = malloc(1024);*/
/**
 * Initilization arguments to a t_option array.
 */
#ifdef T_AUTOPARSE
int init_opt(int argc, char *argv[], short autoparse, ...)
#else
int init_opt(int argc, char *argv[], ...)
#endif
{
    short has3 = 0, i = 0, ti = 0, preopt = 0;
    char *t = (char *)NULL;
    char od[2] = {'\0', '\0'};
    va_list arg_ptr;
#ifdef T_AUTOPARSE
    va_start(arg_ptr, autoparse);
#else
    va_start(arg_ptr, argv);
#endif
    t_option *tmps = (t_option *)NULL, *tmparg = (t_option *)NULL;

    /** check arguments that if has 3th arg for init args */
    tmps = va_arg(arg_ptr, t_option *);
    if ( CHECKOPT(tmps) ) {
        has3 = 1;
    }
    va_end(arg_ptr);

    /** preopts num */
    while(CHECKOPT(tmps)) {
        ++preopt;
        ++tmps;
    }
    tmps -= preopt;
    preopt = (preopt > argc)?preopt:argc;
    /* set buf length to preopt+1 */
    ++preopt;
    opts = (t_option *)calloc(preopt, sizeof(t_option) );
    if(opts) _emptyopts(preopt);
    else return 0;
    /* set program name to the first node of opts */
    opts->sname = '_';
    strcpy(opts->longname, "_progname");
    strcpy(opts->value, argv[0]);
    opts->isset = TOPT_HAS_VAL;

    tmparg = opts;
    tmparg++;

    if ( has3 ) {
        while (CHECKOPT(tmps)) {
            //
            tmparg->sname = tmps->sname;
            strcpy(tmparg->longname, tmps->longname);
            strcpy(tmparg->value, tmps->value);
            tmparg->isset = tmps->isset;

            tmparg++;
            tmps++;
        }
    }
    for ( i = 1; i < argc; i++ ) {
        if(argv[i][0]=='-') {
            if(argv[i][1]=='-'&&argv[i][2]!='\0') {
                if ( i+1<argc && argv[i+1][0]!='-' ) {
                    if(_tsetval(argv[i]+2, argv[i+1], 1)==TOPT_HAS_VAL)
                        ++i;
                }else {
                    _tsetval(argv[i]+2, "", 0);
                }
            }else if (argv[i][1]!='\0' && argv[i][2]=='\0') {
                if (_tsetval(argv[i]+1, argv[i+1], 0)==TOPT_HAS_VAL)
                    ++i;
            }else if (argv[i][1]!='\0') {
                t = argv[i]+1;
                while(*t++)
                    _tsetval(t, "", 0);
            }
        }else {
            od[0] = ti + '0';
            ++ti;
            _tsetval(od, argv[i], 0);
        }
    }

    return 1;
}
/**
 * Get option which name is giving.
 * This will return a pointer to t_option.
 */
t_option * get_opt(const char *name)
{
    t_option *tmp = opts;
    while(CHECKOPT(tmp)) {
        if((name[1] && strcmp(tmp->longname, name)==0) || (name[1]=='\0' && tmp->sname==name[0])) {
            return tmp;
        }
        ++tmp;
    }
    return (t_option *)0;
}
/**
 * Free options memory
 */
void free_opt(void)
{
    if(opts)
        free(opts);
}
/**
 * Set option value
 */
int _tsetval(char *k, char *v, short islname)
{
    /* scan opts */
    t_option *tmp = opts;
    while(CHECKOPT(tmp)) {
        if((islname&&(strcmp(tmp->longname, k)==0)) || (!islname && (k[0]==tmp->sname) ) ) {
            break;
        }
        ++tmp;
    }
    if(islname) {
        strcpy(tmp->longname, k);
    }else {
        tmp->sname = k[0];
    }
    TOPT_SET(tmp);
    if( ( TOPT_ISSETV(tmp) || k[0] < 'A' ) && strlen(v)>0) {
        strcpy(tmp->value, v);
        return TOPT_HAS_VAL;
    }
    return TOPT_NO_VAL;
}
/* empty the global variable opts */
void _emptyopts(int count)
{
    t_option *tmp = opts;
    for( ; --count >= 0 ; ++tmp ) {
        tmp->sname = 0;
        strcpy(tmp->longname, "");
        strcpy(tmp->value, "");
        tmp->isset = 0;
    }
}
