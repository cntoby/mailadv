#include "sendmail.h"
/*#include "sys/socket.h";*/
#include <error.h>

#define T_IFSMTP(extra)     ( (extra) & T_SSMTP )
#define T_IFHTML(extra)     ( (extra) & T_HTML )
#define T_IFHEADER(extra)   ( (extra) & T_HEADER )

/*int sendmail(char *to, char *from, char *subject, char *body, short extra, ...); */
#ifdef T_WITH_FROM
int sendmail(char *to, char *from, char *subject, char *body, short extra, ...)
#else
int sendmail(char *to, char *subject, char *body, short extra, ...)
#endif
{
    int ret = 0;
    FILE *sm;
    va_list arg_ptr;
    char *hdr = (char *)NULL;
    /* socket *skt = (socket *)NULL; */

#ifdef T_WITH_FROM
    if (strlen(to) < 5 || strlen(from) < 5 ) return T_EMADDRE;
#else
    if (strlen(to) < 5 ) return T_EMADDRE;
#endif
    /* if ( extra ) { // check extra tag */
        va_start(arg_ptr, extra);
        if (T_IFHEADER(extra)) {
            hdr = va_arg(arg_ptr, char *);
            if(!hdr) return T_EMEXTRA_H;
        }
        /*
       if (T_IFSMTP(extra)) {
            skt = va_arg(arg_ptr, socket *);
            if(!skt) return T_EMEXTRA_S;
        }
        */
        va_end(arg_ptr);
    /* } */
    if ( !T_IFSMTP(extra) ) {
        sm = popen(SENDMAIL_PATH, "w");
        if (sm) {
            fprintf(sm, "To: %s\n", to);
#ifdef T_WITH_FROM
            fprintf(sm, "From: %s\n", from);
#endif
            fprintf(sm, "Subject: %s\n", subject);
            if (hdr!=NULL) fprintf(sm, "%s\n", hdr);
            fprintf(sm, "\n%s\n", body);
            ret = pclose(sm);
            return ret;
        }else {
            return T_EMOPENSM;
        }
    }else {
        // SMTP SEND MAIL
        return T_EMEXTRA_S;
    }
}
