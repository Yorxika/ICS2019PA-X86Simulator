#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

#define LENGTH 255

const char LOWER[] = "0123456789abcdef";
const char UPPER[] = "0123456789ABCDEF";

int print_char(char **out, char ch) {
  if (out == NULL) {
    _putc(ch);
  } else {
    **out = ch;
    (*out)++;
  }
  return 1;
}

int print_string(char **out,char *in) {
  int count = 0;
  while (*in != '\0') {
    count += print_char(out,*in);
    in++;
  }
  return count;
}

void reverse(char *src, int neg, int len) {
  assert(src);
  if (neg == 1) {
    src[len++] = '-';
  }
  int start = 0 , end = len - 1;
  while (end > start) {
    char ch = src[start];
    src[start] = src[end];
    src[end] = ch;
    start++;
    end--;
  }
  src[len] = '\0';
}

int print_number(char **out,int num, int base, int sign, int upper) {
  if (num >= 0 && num < 10) {
    return print_char(out,LOWER[num]);
  }
  char buf[LENGTH];
  const char *digit = upper ? UPPER : LOWER;
  int neg = 0,index = 0;
  if (sign == 1) {
    if (num < 0) {
      neg = 1;
      num = -num;
    }
    while (num != 0) {
      buf[index++] = digit[num % base];
      //put_ch(1,buf[index-1]);
      num /= base;
    }
  } else {
    unsigned int number = *((unsigned int *)&num);
    while (number != 0) {
      buf[index++] = digit[num % base];
      num /= base;
    }
  }
  if (base == 16) {
    buf[index++] = 'x';
    buf[index++] = '0';
  }
  reverse(buf, neg, index);
  return print_string(out,buf);
}

void put_ch(int flag, char ch) {
  _putc('(');
  if (flag == 1) {
    _putc(ch);
    _putc(')');
  }
}

int check_args (char ch) {
  return ch == 'c' || ch == 'o' || ch == 'd' || ch == 'D' || \
   ch == 'u' || ch == 'p' || ch == 'x' || ch == 'X' || ch == 's';
}

/* TODO: fix fomat like "%02d"
   this func is use to ignore that
 */
char* check_format (char *fmt) {
  while (*fmt) {
    char ch = *fmt;
    if (check_args(ch) == 1) {
      break;
    } else {
      fmt++;
    }
  }
  //put_ch(1, *fmt);
  return fmt;
}

int print_helper(char **out, const char *fmt, va_list ap) {
  char *f = fmt;
  const char *t = fmt;
  // int flag = 0;
  // _putc('t');_putc('e');_putc('s');_putc('t');_putc(':');
  // while (*t)
  // {
  //   _putc(*t);
  //   if (*t == '%' && t[1] == 'p') {
  //     flag = 1;
  //   }
  //   t++;
  // }
  // _putc('e');_putc('n');_putc('d');_putc('\n');
  int len = 0;
  char ch;
  while (*f) {
    //put_ch(flag, *f);
    if (*f != '%') {
      len += print_char(out,*f);
    } else {
      f++;
      f = check_format(f);
      //put_ch(flag, *f);
      switch (*f) {
        case 'c':
          ch = (char)va_arg(ap, int);
          len += print_char(out,ch);
          break;
        case 'o':
          len += print_number(out, va_arg(ap, int), 8, 1, 0);
          break;
        case 'd':
        case 'D':
          len += print_number(out, va_arg(ap, int), 10, 1, 0);
          break;
        case 'u':
          len += print_number(out, va_arg(ap, int), 10, 0, 0);
          break;
        case 'p':
        case 'x':
          len += print_number(out, va_arg(ap, int), 16, 1, 0);
          break;
        case 'X':
          len += print_number(out, va_arg(ap, int), 16, 1, 1);
          break;
        case 's':
          len += print_string(out, va_arg(ap, char *));
          break;
        default:
          break;
      }
    }
    f++;
  }
  if (out != NULL) {
    **out = '\0';
  }
  return len;
}

int printf(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int len = print_helper(NULL, fmt, ap);
  va_end(ap);
  return len;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  return print_helper(&out, fmt, ap);
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  // char *p = out;
  int count = print_helper(&out, fmt, ap);
  va_end(ap);
  // while (*p != '\0') {
  //   _putc(*p++);
  // }
  return count;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}

#endif
