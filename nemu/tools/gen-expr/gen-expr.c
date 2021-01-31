#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

#define MAX_NUM 100  // to control number

// this should be enough
static char buf[65000];
static char op[] = "+-*/";
int len = 0;

static void put_bracket(char ch) {
  buf[len++] = ch;
}

static void put_number() {
  int num = rand() % MAX_NUM + 1;
  len += sprintf(buf + len,"%d",num);
  buf[len++] = op[rand() % 4];
  num = rand() % MAX_NUM + 1;
  len += sprintf(buf + len,"%d",num);
}

static void put_op() {
  char ch;
  ch =  op[rand() % 4];
  buf[len++] = ch;
}

static inline void gen_rand_expr() {
  if (len > MAX_NUM * MAX_NUM) {
    return;
  }
  put_number();
  put_op();
  put_bracket('(');
  //gen_rand_expr();
  put_number();
  put_bracket(')');
}

static char code_buf[65536];
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    if (i != 0) {
      put_op();
    } 
    gen_rand_expr();

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    if(fscanf(fp, "%d", &result)) {

    }
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
