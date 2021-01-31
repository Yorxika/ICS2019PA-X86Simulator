#include "monitor/expr.h"

int init_monitor(int, char *[]);
void ui_mainloop(int);

void expr_test() {
  FILE *fp = fopen("./tools/gen-expr/input", "r");
  printf("Testing expr:\n");

  uint32_t real_ans,my_ans;
  char gen_expr[65000];
  bool success = true;

  while (fscanf(fp,"%u %s",&real_ans,gen_expr) != EOF) {
    //printf("%s\n",gen_expr);
    my_ans = expr(gen_expr,&success);
    if (!success || my_ans != real_ans) {
      printf("Error: Test failed at:%s,real ans is %u, but my ans is %u\n",gen_expr,real_ans,my_ans);
      break;
    }
  }
  if (success) {
    printf("Test expr passed.\n");
  }
}

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
  int is_batch_mode = init_monitor(argc, argv);

  //expr_test();

  /* Receive commands from user. */
  ui_mainloop(is_batch_mode);

  return 0;
}
