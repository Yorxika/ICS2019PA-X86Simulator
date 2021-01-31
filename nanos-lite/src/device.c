#include "common.h"
#include <amdev.h>

#define KEYDOWN_MASK 0x8000

size_t serial_write(const void *buf, size_t offset, size_t len) {
  char *out = (char *)buf;
  for (int i = 0 ; i < len; i++) {
    _putc(*out++);
  }
  return len;
}

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t offset, size_t len) {
  int keycode = read_key();
  //Log("keycode is : %d .", keycode);
  if (keycode == _KEY_NONE) {
    sprintf(buf, "t %d\n", uptime());
  } else if (keycode & KEYDOWN_MASK) {
    keycode ^= KEYDOWN_MASK;
    sprintf(buf, "kd %s\n", keyname[keycode]);
    Log("Receive event: %s ", buf);
  } else {
    sprintf(buf, "ku %s\n", keyname[keycode & ~KEYDOWN_MASK]);
    Log("Receive event: %s ", buf);
  }
  return strlen(buf);
}

static char dispinfo[128] __attribute__((used)) = {};

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  len = (len + offset) > strlen(dispinfo + offset) ? strlen(dispinfo + offset) - offset : len;
  signed long _len = (signed long) len;
  len = _len > 0 ? len : 0;
  //printf("len: %d, offset: %d\n", len, offset);
  strncpy(buf,dispinfo + offset, len);
  return len;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  offset /= 4;
  int x = offset % screen_width(); 
  int y = offset / screen_width();
  draw_rect((uint32_t*)buf, x, y, len / 4, 1);
  return len;
}

size_t fbsync_write(const void *buf, size_t offset, size_t len) {
  //fb_write(buf, offset, len);
  draw_sync();
  return 0;
}

void init_device() {
  Log("Initializing devices...");
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
  memset(dispinfo, '\0', 128);
  sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d\n", screen_width(), screen_height());
  //printf("%s",dispinfo);
}
