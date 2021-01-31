#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t len = 0;
  if (s == NULL) {
    return len;
  }
  while (s[len] != '\0') {
    len++;
  }
  return len;
}

char *strcpy(char* dst,const char* src) {
  if (dst == NULL || src == NULL) {
    return dst;
  }
  size_t i = 0;
  for (;src[i] != '\0'; i++) {
    dst[i] = src[i];
  }
  dst[i] = '\0';
  return dst;
}

char* strncpy(char* dst, const char* src, size_t n) {
  if (dst == NULL || src == NULL) {
    return dst;
  }
  size_t i = 0;
  for (;i < n && src[i] != '\0'; i++) {
    dst[i] = src[i];
  }
  for (;i < n; i++) {
    dst[i] = '\0';
  }
  //printf("src: [%s] -> dst: [%s]\n", src, dst);
  return dst;
}

char* strcat(char* dst, const char* src) {
  if (dst == NULL || src == NULL) {
    return dst;
  }
  size_t len = strlen(dst);
  size_t i = 0;
  for (;src[i] != '\0'; i++) {
    dst[len + i] = src[i];
  }
  dst[len + i] = '\0';
  return dst;
}

int strcmp(const char* s1, const char* s2) {
  assert(s1 != NULL && s2 != NULL);
  size_t i = 0;
  for (; s1[i] == s2[i]; i++) {
    if (s1[i] == '\0') {
      break;
    }
  }
  int rtn = ((unsigned char *)s1)[i] - ((unsigned char *)s2)[i];
  rtn = rtn > 0 ? 1: (rtn < 0 ? -1 : 0);
  //_putc('c');_putc('m');_putc('p');_putc((char)(rtn + '0'));_putc('\n');
  return rtn;
}

int strncmp(const char* s1, const char* s2, size_t n) {
  assert(s1 != NULL && s2 != NULL);
  size_t i = 0;
  for (;i < n && s1[i] == s2[i]; i++) {
    if (s1[i] == '\0') {
      break;
    }
  }
  if (i == n) {
    return 0;
  }
  int rtn = ((unsigned char *)s1)[i] - ((unsigned char *)s2)[i];
  return rtn > 0 ? 1: (rtn < 0 ? -1 : 0);
}

void* memset(void* v,int c,size_t n) {
  if (v == NULL) {
    return v;
  }
  char *dst = (char *)v;
  for (size_t i = 0 ; i < n; i++) {
    dst[i] = c;
  }
  return v;
}

void* memcpy(void* out, const void* in, size_t n) {
  assert(out <= in || out >= in + n );
  char *dst = (char *)out;
  char *src = (char *)in;
  for (size_t i = 0 ; i < n ; i++) {
    dst[i] = src[i];
  }
  return dst;
}

int memcmp(const void* s1, const void* s2, size_t n){
  assert(s1 != NULL && s2 != NULL);
  char *v1 = (char *)s1;
  char *v2 = (char *)s2;
  int rtn = strncmp(v1, v2, n);
  // int rtn = 0;
  // for (size_t i = 0 ; i < n ; i++) {
  //   if (v1[i] != v2[i]) {
  //     rtn = v1[i] - v2[i];
  //     break;
  //   }
  // }
  //_putc('c');_putc('m');_putc('p');_putc((char)(rtn + '0'));_putc('\n');
  return rtn;
}

#endif
