#include "fs.h"

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);
size_t fb_write(const void *buf, size_t offset, size_t len);
size_t dispinfo_read(void *buf, size_t offset, size_t len);
size_t events_read(void *buf, size_t offset, size_t len);
size_t serial_write(const void *buf, size_t offset, size_t len);
size_t fbsync_write(const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  size_t open_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin", 0, 0, 0, invalid_read, invalid_write},
  {"stdout", 0, 0, 0, invalid_read, serial_write},
  {"stderr", 0, 0, 0, invalid_read, serial_write},
#include "files.h"
  {"/dev/events", 0, 0, 0, events_read, invalid_write},
  {"/dev/fb", 0, 0, 0, invalid_read, fb_write},
  {"/proc/dispinfo", 0, 0, 0, dispinfo_read, invalid_write},
  {"/dev/fbsync", 0, 0, 0, invalid_read, fbsync_write},
  {"/dev/tty", 0, 0, 0, invalid_read, serial_write}
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  // TODO: initialize the size of /dev/fb
  Log("Initializing file system...");
  int fd = fs_open("/dev/fb", 0, 0);
  file_table[fd].size = sizeof(uint32_t) * screen_height() * screen_width();
  fs_close(fd);

}

int fs_open(const char *pathname, int flags, int mode) {
  for (int i = 0; i < NR_FILES; i++) {
    if (strcmp(pathname, file_table[i].name) == 0) {
      Log("Open file %s success.", pathname);
      file_table[i].open_offset = 0;
      return i;
    }
  }
  panic("File %s not exist.", pathname);
  return -1;
}

int fs_close(int fd) {
  file_table[fd].open_offset = 0;
  Log("file %s close.", file_table[fd].name);
  return 0;
}

size_t fs_read(int fd, void *buf, size_t len){
  assert(fd >= 0 && fd < NR_FILES);
	int read_len = len;
	if(file_table[fd].size > 0 && file_table[fd].open_offset + len > file_table[fd].size) {
		read_len = file_table[fd].size - file_table[fd].open_offset;
	}
	
	size_t length = 0;
	if(file_table[fd].read == NULL) {
		length = ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, read_len);
    //Log("fs_read disk_offset: %d , open_offset: %d , read_len: %d, length: %d",file_table[fd].disk_offset, file_table[fd].open_offset, read_len, length);
	}else{
		length = file_table[fd].read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, read_len);
	}
	
	file_table[fd].open_offset += length;
	return length;
}

size_t fs_write(int fd, const void *buf, size_t len){
  assert(fd >= 0 && fd < NR_FILES);
	int write_len = len;
	if(file_table[fd].size > 0 && file_table[fd].open_offset + len > file_table[fd].size) {
		write_len = file_table[fd].size - file_table[fd].open_offset;
	}
	
	size_t length = 0;
  //Log("fs_write disk_offset: %d , open_offset: %d , write_len: %d",file_table[fd].disk_offset, file_table[fd].open_offset, write_len);
	if(file_table[fd].write == NULL) {
		length = ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, write_len);
	}else{
		length = file_table[fd].write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, write_len);
	}
	
	file_table[fd].open_offset += length;
	return length;
}

size_t fs_lseek(int fd, size_t offset, int whence){
	assert(fd >= 0 && fd < NR_FILES);
	size_t open_offset = file_table[fd].open_offset;
  //Log("file: %s open_offset:%d, offset: %d, whence:%d ", file_table[fd].name, open_offset, offset, whence);
	switch (whence) {
		case SEEK_SET: 
			open_offset = offset;
			break;
		case SEEK_CUR:
			open_offset += offset;
			break;
		case SEEK_END:
			open_offset = file_table[fd].size + offset;
			break;
		default: 
      panic("lseek whence error!");
	}
	file_table[fd].open_offset = open_offset;
	return open_offset;
}