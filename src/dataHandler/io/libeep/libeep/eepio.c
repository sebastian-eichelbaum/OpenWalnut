/********************************************************************************
 *                                                                              *
 * this file is part of:                                                        *
 * libeep, the project for reading and writing avr/cnt eeg and related files    *
 *                                                                              *
 ********************************************************************************
 *                                                                              *
 * LICENSE:Copyright (c) 2003-2009,                                             *
 * Advanced Neuro Technology (ANT) B.V., Enschede, The Netherlands              *
 * Max-Planck Institute for Human Cognitive & Brain Sciences, Leipzig, Germany  *
 *                                                                              *
 ********************************************************************************
 *                                                                              *
 * This library is free software; you can redistribute it and/or modify         *
 * it under the terms of the GNU Lesser General Public License as published by  *
 * the Free Software Foundation; either version 3 of the License, or            *
 * (at your option) any later version.                                          *
 *                                                                              *
 * This library is distributed WITHOUT ANY WARRANTY; even the implied warranty  *
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the              *
 * GNU Lesser General Public License for more details.                          *
 *                                                                              *
 * You should have received a copy of the GNU Lesser General Public License     *
 * along with this program. If not, see <http://www.gnu.org/licenses/>          *
 *                                                                              *
 *******************************************************************************/

#include <string.h>
#include <ctype.h>
#include <errno.h>

#include <assert.h>

#define _GNU_SOURCE

#include <stdio.h>
#include <stdarg.h>

#include <eep/eepio.h>
#include <eep/eepmem.h>

char RCS_eepio_h[] = RCS_EEPIO_H;
char RCS_eepio_c[] = "$RCSfile: eepio.c,v $ $Revision: 2415 $";

int eepio_quiet = 0;
int eepio_doslf = 0;

int eepstdout(const char *fmt, ...)
{
  int n = 0;
  va_list va;

  if (!eepio_quiet) {
    va_start(va, fmt);
    n = vprintf(fmt, va);
    va_end(va);
  }
  return n;
}

int eepstderr(const char *fmt, ...)
{
  int n = 0;
  va_list va;

  if (!eepio_quiet) {
    va_start(va, fmt);
    n = vfprintf(stderr, fmt, va);
    va_end(va);
  }
  return n;
}

/* 
  configure output and keep track of output state 
*/
int     EEPDebug = 0;
int     EEPBar = 1;
int     EEPBarOn = 0;
slen_t  EEPBarTotal;
slen_t  EEPBarCurrent;
int     eepmess_log = 0;
char    messorigin[32] = "libeep: ";

/* printf wrappers --------------------------------------- */

void eeplog(const char *fmt, ...)
{
  va_list va;
  
  if (eepio_quiet)
    return;
    
  va_start(va, fmt);
  vprintf(fmt, va);
  va_end(va);
  if (eepmess_log) {
    va_start(va, fmt);
    vfprintf(stderr, fmt, va);
    va_end(va);
  }
}

void eepdebug(const char *fmt, ...)
{
  va_list va;

  if (EEPDebug) {
    va_start(va, fmt);
    vfprintf(stderr, fmt, va);
    va_end(va);
  }
}

void eepstatus(const char *fmt, ...)
{
  va_list va;

  if (eepio_quiet)
    return;
  if (EEPBarOn)
    free_eep_bar();

  fputs(messorigin, stderr);
  va_start(va, fmt);
  vfprintf(stderr, fmt, va);
  va_end(va);
}

void sysstatus(const char *fmt, ...)
{
  va_list va;

  if (eepio_quiet)
    return;
  if (EEPBarOn)
    free_eep_bar();

  fputs(messorigin, stderr);
  va_start(va, fmt);
  vfprintf(stderr, fmt, va);
  va_end(va);
  if(errno) {
    fputs(messorigin, stderr);
    perror("errno");
   }  
}

void eeperror(const char *fmt, ...)
{
  va_list va;

  if (!eepio_quiet) {
    if (EEPBarOn)
      free_eep_bar();

    fputs(messorigin, stderr);
    va_start(va, fmt);
    vfprintf(stderr, fmt, va);
    va_end(va);
  }

  arv_fclear();
  exit(1);
}

void syserror(const char *fmt, ...)
{
  va_list va;

  if (!eepio_quiet) {
    if (EEPBarOn)
      free_eep_bar();

    fputs(messorigin, stderr);
    va_start(va, fmt);
    vfprintf(stderr, fmt, va);
    va_end(va);
    if(errno) {
      fputs(messorigin, stderr);
      perror("");
     } 
  }

  arv_fclear();
  exit(1);
}

/* the status indicator -------------------------------------------- */

void init_eep_bar(slen_t total)
{
  char line[50];
  
  EEPBarCurrent = 0;
  EEPBarTotal = total;
  if (EEPBar) {
    EEPBarOn = 1;
    sprintf(line, "%sprocessing\n  ", messorigin);
    eepstderr(line);
  }
}

void show_eep_bar(slen_t current)
{
  slen_t i;
  slen_t newval, old;
  
  old = EEPBarCurrent * 73 / EEPBarTotal;
  newval = current * 73 / EEPBarTotal;
  EEPBarCurrent = current;
  if (EEPBar) {
    for (i = old; i < newval; i++) {
      eepstderr("*");
    }
  }
}

void free_eep_bar(void)
{
  if (EEPBar) {
    eepstderr("\n");
    EEPBarOn = 0;
  }
}

/* 
  the eepmess.h *open funcs register files for "auto remove" here 
  the *error functions above must be able to cleanup
*/
int    ar_filec    = 0;
FILE **ar_file     = NULL;
char **ar_filename = NULL;

void arv_fclear(void)
{
  int i;
  
  for (i = 0; i < ar_filec; i++) {
    if (fflush(ar_file[i])) {
      sysstatus("cannot flush file \"%s\"!\n", ar_filename[i]);
      exit(1);
    }
    if (fclose(ar_file[i])) {
      sysstatus("cannot close file \"%s\"!\n", ar_filename[i]);
      exit(1);
    }

    if (!remove(ar_filename[i])) {
      eepstatus("incomplete file \"%s\" removed\n", ar_filename[i]);
    }

    v_free(ar_filename[i]);
  }
  v_free(ar_filename); ar_filename = NULL;
  v_free(ar_file); ar_file = NULL;
  ar_filec = 0;
}

/* configuration preprocessing -----------------------------------------*/

char *cfg_line_norm(char *line)
{
  char *tmp;
  char *buf;
  int i = 0;
  int j = 0;
  int c;

  buf = (char *) malloc(strlen(line) + 1);
  strcpy(buf,line);                               /* make a copy */
  if ((tmp = (char *) strchr(buf, CFG_SEP)))
    *tmp='\0';                                    /* cut comment if exists */
  while ((c = buf[i++]))
    if (!isspace(c)) 
      line[j++] = toupper(c);                     /* copy valid characters */
  line[j]='\0';
  free(buf);

  return line;
}

char *cfg_line_norm_cs(char *line)
{
  char *tmp;
  char *buf;
  int i = 0;
  int j = 0;
  int c;

  buf = (char *) malloc(strlen(line) + 1);
  strcpy(buf,line);                               /* make a copy */
  if ((tmp = (char *) strchr(buf, CFG_SEP)))
    *tmp='\0';                                    /* cut comment if exists */
  while ((c = buf[i++]))
    if (!isspace(c)) 
      line[j++] = c;                              /* copy valid characters */
  line[j]='\0';
  free(buf);

  return line;
}

void eep_print_wrap(FILE* out, const char* text, int len)
{
  int count;

  count = 0;
  while(*text)
  {
    if(count >= len && *text != '\n')
    {
      fputc('\n', out);
      count = 0;
    }

    if(*text == '\n')
    {
      fputc('\n',out);
      count = 0;
    }
    else if(*text == '\r')
    {;}
    else if(*text == '\t')
    {
      count += 8; 
      fputc('\t', out);
    }
    else
    {
      fputc(*text, out);
      count ++;
    }
    text++;
  }
}
/*****************************
 * swap 8 bytes in long long *
 *****************************/
__int64 eep_byteswap_8(__int64 arg) {
  char *in, *out;
  __int64 result;
  assert(sizeof(__int64) == 8);
  assert(sizeof(char) == 1);
  in=(char *) &arg;
  out=(char *) &result;
  out[0]=in[7];
  out[1]=in[6];
  out[2]=in[5];
  out[3]=in[4];
  out[4]=in[3];
  out[5]=in[2];
  out[6]=in[1];
  out[7]=in[0];
  
  // printf("%s converted 0x%x to 0x%x\n", __FUNCTION__, arg, result);
  return result;
}
/************************
 * swap 4 bytes in long *
 ************************/
long eep_byteswap_4(long arg) {
  char *in, *out;
  long result;
  assert(sizeof(long) == 4);
  assert(sizeof(char) == 1);
  in=(char *) &arg;
  out=(char *) &result;
  out[0]=in[3];
  out[1]=in[2];
  out[2]=in[1];
  out[3]=in[0];
  
  // printf("%s converted 0x%08x to 0x%08x\n", __FUNCTION__, arg, result);
  return result;
}
/*************************
 * swap 2 bytes in short *
 *************************/
short eep_byteswap_2(short arg) {
  char *in, *out;
  short result;
  assert(sizeof(short) == 2);
  assert(sizeof(char) == 1);
  in=(char *) &arg;
  out=(char *) &result;
  out[0]=in[1];
  out[1]=in[0];
  
  // printf("%s converted 0x%04x to 0x%04x\n", __FUNCTION__, arg, result);
  return result;
}
/**************************
 * swap 8 bytes in double *
 **************************/
double eep_byteswap_8_double(double arg) {
  char *in, *out;
  double result;
  assert(sizeof(double) == 8);
  assert(sizeof(char) == 1);
  in=(char *) &arg;
  out=(char *) &result;
  out[0]=in[7];
  out[1]=in[6];
  out[2]=in[5];
  out[3]=in[4];
  out[4]=in[3];
  out[5]=in[2];
  out[6]=in[1];
  out[7]=in[0];
  
  // printf("%s converted 0x%x to 0x%x\n", __FUNCTION__, arg, result);
  return result;
}
/***********************************
 * only swap on big endian machine *
 ***********************************/
__int64 eep_byteswap_8_safe(__int64 arg) {
  if(EEP_BYTE_ORDER==EEP_BIG_ENDIAN) {
    return eep_byteswap_8(arg);
  }
  return arg;
}
long eep_byteswap_4_safe(long arg) {
  if(EEP_BYTE_ORDER==EEP_BIG_ENDIAN) {
    return eep_byteswap_4(arg);
  }
  return arg;
}
short eep_byteswap_2_safe(short arg) {
  if(EEP_BYTE_ORDER==EEP_BIG_ENDIAN) {
    return eep_byteswap_2(arg);
  }
  return arg;
}
double eep_byteswap_8_double_safe(double arg) {
  if(EEP_BYTE_ORDER==EEP_BIG_ENDIAN) {
    return eep_byteswap_8_double(arg);
  }
  return arg;
}
