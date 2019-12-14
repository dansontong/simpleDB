/*! @file ccalc.c
 * @brief main program
 *********************************************************************
 * a simple calculator with variables
 * 
 * sample-files for a artikel in developerworks.ibm.com
 * Author: Christian Hagen, chagen@de.ibm.com
 * 
 * @par ccalc.c
 * main-program
 * 
 *********************************************************************
 */
#include "sqlparser.h"

#include <iostream>
#include "log.h"

/*
 * global variable
 */

int parser_debug = 0;
trivialtree *groot = NULL;

/*
 * local variable
 */
static FILE *file;
static int eof = 0;
static int nRow = 0;
static int nBuffer = 0;
static int lBuffer = 0;
static int nTokenStart = 0;
static int nTokenLength = 0;
static int nTokenNextStart = 0;
static int lMaxBuffer = 1000;
static char *buffer;

/*--------------------------------------------------------------------
 * dumpChar
 * 
 * printable version of a char
 *------------------------------------------------------------------*/
static char dumpChar(char c)
{
    if (isprint(c))
        return c;
    return '@';
}

/*--------------------------------------------------------------------
 * dumpString
 * 
 * printable version of a string upto 100 character
 *------------------------------------------------------------------*/
static char *dumpString(char *s)
{
    static char buf[101];
    int i;
    int n = strlen(s);

    if (n > 100)
        n = 100;

    for (i = 0; i < n; i++)
        buf[i] = dumpChar(s[i]);
    buf[i] = 0;
    return buf;
}

/*--------------------------------------------------------------------
 * DumpRow
 * 
 * dumps the contents of the current row
 *------------------------------------------------------------------*/
extern void DumpRow(void)
{
    if (nRow == 0)
    {
        int i;
        fprintf(stdout, "       |");
        for (i = 1; i < 71; i++)
            if (i % 10 == 0)
                fprintf(stdout, ":");
            else if (i % 5 == 0)
                fprintf(stdout, "+");
            else
                fprintf(stdout, ".");
        fprintf(stdout, "\n");
    }
    else
    {
        fprintf(stdout, "%6d |%.*s", nRow, lBuffer, buffer);
    }
}

/*--------------------------------------------------------------------
 * MarkToken
 * 
 * marks the current read token
 *------------------------------------------------------------------*/
extern void PrintError(char *errorstring, ...)
{
    static char errmsg[10000];
    va_list args;

    int start = nTokenStart;
    int end = start + nTokenLength - 1;
    int i;

    /*================================================================*/
    /* a bit more complicate version ---------------------------------*/
    if (eof)
    {
        fprintf(stdout, "...... !");
        for (i = 0; i < lBuffer; i++)
            fprintf(stdout, ".");
        fprintf(stdout, "^-EOF\n");
    }
    else
    {
        fprintf(stdout, "...... !");
        for (i = 1; i < start; i++)
            fprintf(stdout, ".");
        for (i = start; i <= end; i++)
            fprintf(stdout, "^");
        for (i = end + 1; i < lBuffer; i++)
            fprintf(stdout, ".");
        fprintf(stdout, "   token%d:%d\n", start, end);
    }

    /*================================================================*/
    /* print it using variable arguments -----------------------------*/
    va_start(args, errorstring);
    vsprintf(errmsg, errorstring, args);
    va_end(args);

    fprintf(stdout, "Error: %s\n", errmsg);
}

/*--------------------------------------------------------------------
 * getNextLine
 * 
 * reads a line into the buffer
 *------------------------------------------------------------------*/
static int getNextLine(void)
{
    int i;
    char *p;

    /*================================================================*/
    /*----------------------------------------------------------------*/
    nBuffer = 0;
    nTokenStart = -1;
    nTokenNextStart = 1;
    eof = false;

    /*================================================================*/
    /* read a line ---------------------------------------------------*/
    p = fgets(buffer, lMaxBuffer, file);
    if (p == NULL)
    {
        if (ferror(file))
            return -1;
        eof = true;
        return 1;
    }

    nRow += 1;
    lBuffer = strlen(buffer);
    DumpRow();

    return 0;
}

/*--------------------------------------------------------------------
 * GetNextChar
 * 
 * reads a character from input for flex
 *------------------------------------------------------------------*/
extern int GetNextChar(char *b, int maxBuffer)
{
    int frc;

    /*================================================================*/
    /*----------------------------------------------------------------*/
    if (eof)
        return 0;

    /*================================================================*/
    /* read next line if at the end of the current -------------------*/
    while (nBuffer >= lBuffer)
    {
        frc = getNextLine();
        if (frc != 0)
            return 0;
    }

    /*================================================================*/
    /* ok, return character ------------------------------------------*/
    b[0] = buffer[nBuffer];
    nBuffer += 1;

    if (parser_debug)
        printf("GetNextChar() => '%c'0x%02x at %d\n",
               dumpChar(b[0]), b[0], nBuffer);
    return b[0] == 0 ? 0 : 1;
}

/*--------------------------------------------------------------------
 * BeginToken
 * 
 * marks the beginning of a new token
 *------------------------------------------------------------------*/
extern void BeginToken(char *t)
{
    /*================================================================*/
    /* remember last read token --------------------------------------*/
    nTokenStart = nTokenNextStart;
    nTokenLength = strlen(t);
    nTokenNextStart = nBuffer; // + 1;
}

/*--------------------------------------------------------------------
 * PrintGrammar
 * 
 * print the sql grammar tree
 *------------------------------------------------------------------*/
void PrintGrammar(trivialtree *root)
{
    if (root && parser_debug)
    {
        fprintf(stdout, "       |  *** grammar tree ***  \n");
        root->print(0);
    }
}


// 调用语法分析接口（单个语句的）
// 传入sql语句的起始地址&语句长度
// 返回语法树的树根指针，如果出错则返回NULL
// 如果当前语句中有多个语句，则只返回最后一句的语法树
// 非线程安全！
trivialtree * sql_parser_single(char *sql, int sqllen)
{
    // 创建临时文件
    char tmp_filename[] = "./sqptmp.tmp";
    FILE *f1 = fopen(tmp_filename, "wb");

    // 将sql写入临时文件
    int r1 = fwrite(sql, sizeof(char), sqllen, f1);
    if (r1<sqllen){
        log_Error("write sql to temp file error.\n");
        return NULL;
    }
    fclose(f1);


    file = fopen(tmp_filename, "r");

    /*================================================================*/
    /*----------------------------------------------------------------*/
    buffer = (char *)malloc(lMaxBuffer);
    if (buffer == NULL)
    {
        printf("cannot allocate %d bytes of memory\n", lMaxBuffer);
        fclose(file);
        return NULL;
    }

    // 调用语法解析器
    groot = NULL;
    DumpRow();
    if (getNextLine() == 0)
        yyparse();

    /*================================================================*/
    /* ending... -----------------------------------------------------*/
    free(buffer);
    fclose(file);

    // 删除临时文件
    int res = remove(tmp_filename);
    if (res != 0){
        log_Error("remove sql temp file error.\n");
    }
    return groot;
}