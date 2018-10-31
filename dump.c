/*----------------------------------------------------------------------*\

  dump.c - Small file dumper

  1991-03-08/thoni@rabbit

\*----------------------------------------------------------------------*/

#include <stdio.h>
#include <ctype.h>


#define ISPRINT(c) ((c>=0x20 && c<0x7f) || (c>0xa0 && c<0xff))

/* Types */

typedef int Boolean;
#define TRUE (0==0)
#define FALSE (!TRUE)


typedef enum ModeKind {
  HEXMODE,
  OCTMODE,
  DECMODE
} ModeKind;


/* Parameters */

static char *file;		/* Pointer to file name */

static int nWords = 16;		/* Number of words per line */
static int wSize = 1;		/* Size of words */
static Boolean wordAddressing = 1; /* Addresses to words or byte addresses */
static Boolean hex, oct, dec;
static Boolean longWords, shortWords, byteWords;


/* Private data */
static ModeKind mode = HEXMODE;	/* Output format */
static int nBytes;		/* Bytes per line */


/* SPA Option handling */

#include "spa.h"

SPA_FUN(usage)
{
  printf("Usage: DUMP [-help] [options] [file]\n");
}


SPA_ERRFUN(paramError)
{
  usage(NULL, NULL, 0);
  exit(-1);
}

SPA_FUN(xit) {exit(0);}

SPA_DECLARE(arguments)
     SPA_STRING("file", "name of file to dump, if not present take from stdin",
          file, NULL, NULL)
     SPA_FUNCTION("", "extra argument", paramError)
SPA_END

SPA_DECLARE(options)
     SPA_HELP("help", "this help", usage, xit)
     SPA_INTEGER("size", "size of words in bytes (1, 2 or 4)", wSize, 4, NULL)
     SPA_FLAG("addressing", "word addressing instead of byte addresses",
          wordAddressing, FALSE, NULL)
     SPA_INTEGER("words", "number of words to display per line", nWords, 4, NULL)
     SPA_FLAG("hex", "dump in hexadecimal (default)", hex, FALSE, NULL)
     SPA_FLAG("oct", "dump in octal (NYI)", oct, FALSE, NULL)
     SPA_FLAG("dec", "dump in decimal (NYI)", dec, FALSE, NULL)
     SPA_FLAG("long", "dump long words, same as -size 4 -addressing", longWords, FALSE, NULL)
     SPA_FLAG("word", "dump words, same as -size 2 -addressing", shortWords, FALSE, NULL)
     SPA_FLAG("byte", "dump bytes, same as -size 1", byteWords, FALSE, NULL)
SPA_END



static void printHex(digit)
     int digit;
{
  printf("%c", digit<10? digit+'0': digit-10+'a');
}


static void printByte(byte)
     int byte;
{
  switch (mode) {
  case HEXMODE:
    printHex(byte>>4);
    printHex(byte&0xf);
    break;
  }
}


static printLong(l)
     int l;
{
  printHex((l>>28)&0x0f);
  printHex((l>>24)&0x0f);
  printHex((l>>20)&0x0f);
  printHex((l>>16)&0x0f);
  printHex((l>>12)&0x0f);
  printHex((l>>8)&0x0f);
  printHex((l>>4)&0x0f);
  printHex((l>>0)&0x0f);
}


int main(argc, argv)
     int argc;			/* IN - Argument count */
     char **argv;		/* IN - Arguments */
{
  int eof = 0;			/* End of file found ? */
  int address;			/* Which adress are we on */
  unsigned char buffer[256];	/* Input buffer */
  int in, out;
  FILE *fp;
  int nArgs;

  nArgs = spaProcess(argc, argv, arguments, options, paramError);
  switch (nArgs) {
  case 0:
    fp = stdin;
    break;
  case 1:
    if ((fp = fopen(file, "rb")) == NULL) {
      printf("Could not open file '%s'\n", file);
      exit(0);
    }
    break;
  }

  if (byteWords + shortWords + longWords > 1) {
    printf("Error: only one of -long, -words, -byte allowed.\n");
    exit(0);
  }

  wordAddressing = wordAddressing || byteWords || shortWords || longWords;
  if (byteWords)
    wSize = 1;
  else if (shortWords)
    wSize = 2;
  else if (longWords)
    wSize = 4;

  nBytes = nWords*wSize;

  address = 0;
  while (!eof) {
    if ((in = fread(buffer, 1, nBytes, fp)) == 0)
      eof = 1;
    printLong(address);
    printf(":  ");
    for (out = 0; out < in; out++) {
      printByte(buffer[out]);
      if (out%wSize == wSize-1)
    printf(" ");
    }
    printf("    ");
    for (out = 0; out < in; out++) {
      if (ISPRINT(buffer[out]))
    printf("%c", buffer[out]);
      else
    printf(".");
    }
    printf("\n");
    if (wordAddressing)
      address += nWords;
    else
      address += nBytes;
  }
}
