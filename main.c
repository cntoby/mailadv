#include "main.h"

char * subencode(const char *src, char *dest, unsigned int len);
unsigned int filesize(char *filename);
unsigned int fileline(char *filename);
unsigned int seekline(FILE *fp, unsigned int line);
void usage(int type);

int main(int argc, char *argv[])
{
    char from[50], to[50], maillist[30], content[30], subject[80], log[30];
    char *mcontent=(char *)NULL, *mb=(char *)NULL;
    short int cont = 0, hasto = 0, sret = 0, tolen = 0;
    unsigned int curline = 0, flen = 0, mllen = 0;
    FILE *fp, *lfp;
    char hdrs[] = "From: %s\r\nReply-To: %s\r\nX-Mailer: tobycn.org mailler 0.9\r\nMIME-Version: 1.0\r\nContent-Type: text/html; charset=UTF-8\r\nContent-Transfer-Encoding: base64\r\n";
    char hdr[256];
    t_option arglist[] = {
                    {'f', "from",     "A.D.S <ads@ads.snowpear.com>", TOPT_HAS_VAL},
                    {'m', "maillist", "maillist",  TOPT_HAS_VAL},
                    {'b', "content",  "mail.htm", TOPT_HAS_VAL},
                    {'s', "subject",  "", TOPT_HAS_VAL},
                    {'l', "log",       "maillog", TOPT_HAS_VAL},
                    {'t', "to",        "", TOPT_HAS_VAL},
                    {'r', "continue", "", TOPT_NO_VAL},
                    {'h', "help",      "", TOPT_NO_VAL},
                    {0, "", "",0}
    };
    t_option *tmp = (t_option *)NULL;

    init_opt(argc, argv, arglist);
    tmp = get_opt("help");
    if( argc <= 1 ) {
        usage(0);
        return -1;
    }else if (TOPT_ISSET(tmp)) {
        usage(1);
        return -1;
    }

    tmp = get_opt("from");
    strcpy(from, tmp->value);
    tmp = get_opt("maillist");
    strcpy(maillist, tmp->value);
    tmp = get_opt("content");
    strcpy(content, tmp->value);
    tmp = get_opt("log");
    strcpy(log, tmp->value);
    /* encode the subject */
    tmp = get_opt("subject");
    if (TOPT_ISSET(tmp)) {
        //strcpy(subject, tmp->value);
        subencode(tmp->value, subject, 80);
    }else {
        printf("subject is needed.\n");
        exit(-1);
    }
    tmp = get_opt("to");
    if (TOPT_ISSET(tmp)) {
        strcpy(to, tmp->value);
        hasto = 1;
    }
    tmp = get_opt("continue");
    if (TOPT_ISSET(tmp)) cont = 1;
    free_opt(); /* release memory of opts */
    /* check mail body file */
    if ((flen = filesize(content))==0) {
        printf("%s is no exists or it's empty.\n", content);
        exit(-1);
    }
    mcontent = (char *)malloc(flen+1);
    if (!mcontent) {
        printf("%d: Can not alloc memory, exit.\n", __LINE__);
        exit(-1);
    }
    fp = fopen(content, "r");
    if(fread(mcontent, flen, 1, fp)!=1) {
        printf("%s can not read..\n", content);
        fclose(fp);
        exit(-1);
    }else {
        /* read mail content, and encode it. */
        flen = base64_len(mcontent, 0);
        mb = (char *)malloc(flen+1);
        if (!mb) {
            printf("%d: Can not alloc memory, exit.\n", __LINE__);
            exit(-1);
        }
        base64_encode(mcontent, mb, flen);
        free(mcontent);
        flen = chunk_split_len(mb);
        mcontent = (char *)malloc(flen+1);
        chunk_split(mb, mcontent);
        free(mb);
    }
    fclose(fp);
    /* check mail list file */
    if (!hasto && ((mllen = fileline(maillist))==0)) {
        printf("%s is not exists or it's empty, and -t is not set.\n", maillist);
        exit(-1);
    }
    if (mllen) {
        fp = fopen(maillist, "r");
    }else {
        fp = (FILE *)NULL;
    }
    /* check mail log file */
    if (cont) {
        lfp = fopen(log, "a+");
        curline = fileline(log);
        seekline(fp, curline);
    }else {
        lfp = fopen(log, "w+");
    }

    sprintf(hdr, hdrs, from, from);

    if (hasto) {
        if ((sret = sendmail(to, subject, mcontent, T_HEADER|T_HTML, hdr))==T_EMOK)
            fprintf(stdout, "%s\t1\n", to);
        else
            fprintf(stdout, "%s\t%d\n", to, sret);
    }
    if (mllen && mllen > curline) {
        printf("Current MISSION will send %d mails.\n", mllen);
        if (curline > 0) printf("continue from %d\n", curline);
        while(fgets(to, 50, fp)) {
            ++curline;
            printf("Current: %d / %d\r", curline, mllen);
            tolen = strlen(to);
            tolen = tolen > 0 ? tolen-1 : tolen;
            if (to[tolen]=='\n') to[tolen] = '\0';
            if ( (sret = sendmail(to, subject, mcontent, T_HEADER|T_HTML, hdr))==T_EMOK)
                fprintf(lfp, "%s\t1\n", to);
            else
                fprintf(lfp, "%s\t%d\n", to, sret);
        }
        printf("\nMission Complete\n");
    }else if(mllen <= curline) {
        printf("The mail addresses all sended or maillist file is invalid.\n");
    }
    if (fp) fclose(fp);
    if (lfp) fclose(lfp);
    free(mcontent);
    /*
    tmp = get_opt("0");
    if(tmp) {
        printf("The First name: %s\n", tmp->value);
        if(strlen(tmp->value)>0) {
            len = base64_len(tmp->value, 0);
            t = (char *)malloc(len);
            rlen = base64_encode(tmp->value, t, len);
            printf("Base64 is: %d (%d)\n%s\n", rlen, len, t);
            len = chunk_split_len(t);
            sp = (char *)malloc(len);
            sp = chunk_split(t, sp);
            printf("Base64 chunked: %d\n%s\n", len, sp);
            free(t);
            //free(sp);
        }
    }
    */
    return 0;
}

void usage(int type) {
    t_option *tmp = (t_option *)NULL;
    tmp = get_opt("_");
    printf("\nUsage: %s [-r] [-h] [-f <from mail>] [-t <mail addr>] [-m <maillist>] [-b <mail file>] [-s <mail subject>] [-l <logfile>]\n\n", tmp->value);
    if (type > 0) {
        printf("  Mail Adv Tool %s (Toby <cntoby@gmail.com>)\n\n", VERSION);
        printf("  %-6s%-12s%s%s\n", "-f", "--from", "from mail addr. Default: ", "A.D.S <ads@ads.snowpear.com>");
        printf("  %-6s%-12s%s\n", "-t", "--to", "to this mail addr.");
        printf("  %-6s%-12s%s%s\n", "-m", "--maillist", "mail list file. Default: ", "maillist");
        printf("  %-6s%-12s%s%s\n", "-b", "--content", "mail body. Default: ", "mail.htm");
        printf("  %-6s%-12s%s\n", "-s", "--subject", "subject of the mail.");
        printf("  %-6s%-12s%s%s\n", "-l", "--log", "log file, if you want continue the process, this will needed. Default: ", "maillog");
        printf("  %-6s%-12s%s\n", "-r", "--continue", "continue the last process, need -l.");
        printf("  %-6s%-12s%s\n\n", "-h", "--help", "Show this message.");
    }
}

char * subencode(const char *src, char *dest, unsigned int len)
{
    char encode[20] = "", *b;
    unsigned slen = 0;
    unsigned short i = 0;
    if (getenv("LANG")) strcpy(encode, getenv("LANG"));
    else strcpy(encode, "GBK"); /* if environment LANG is not set or is empty, set default encode to GBK */
    if (strchr(encode, '.')) {
        while (encode[i] && (encode[i] != '.'))
            ++i;
        ++i;
    }
    slen = base64_len((char *)src, 0);
    b = (char *)malloc(slen);
    base64_encode(src, b, len);
    if (encode[i]) {
        sprintf(dest, "=?%s?B?%s?=", &encode[i], b);
    }else {
        sprintf(dest, "=?%s?B?%s?=", encode, b);
    }
    return dest;
}
unsigned int filesize(char *filename)
{
    FILE *fp;
    unsigned int c = 0;
    fp = fopen(filename, "r");
    if (!fp) return 0;
    fseek(fp, 0, SEEK_END);
    c = ftell(fp);
    fclose(fp);
    return c;
}

unsigned int fileline(char *filename)
{
    FILE *fp;
    char tmpl[1025];
    unsigned int c = 0;
    fp = fopen(filename, "r");
    if (!fp) return 0;
    while(fgets(tmpl, 1024, fp))
        ++c;
    fclose(fp);
    return c;
}
unsigned int seekline(FILE *fp, unsigned int line)
{
    unsigned char x;
    if(!fp) return 0;
    do {
        x = fgetc(fp);
        if (x=='\n') --line;
    } while(!feof(fp) && line>0);
    return 1;
}
