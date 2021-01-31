#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}


static int cmd_si(char *args) {
  char *arg = strtok(NULL, " ");
  if (arg == NULL) {
    //default execute one instruction
    cpu_exec(1);
    return 0;
  }
  //printf("args is : %s\n",arg);

  char ch;
  int res = 0;
  while (*arg != '\0'){
    ch = *arg++;
    if (ch < '0' || ch > '9') {
      printf("Error: Input Error! Usage: si [N].\n");
      return 0;
    }
    res = res * 10 + (ch - '0');
  }
  if (res == 0) {
    res = 1;
  }
  //printf("res is:%d\n",res);
  cpu_exec(res);
  return 0;
}

static int cmd_info(char *args) {
  char *arg = strtok(NULL, " ");
  if (arg == NULL) {
    printf("Error:Input Error! Usage: info [r|w]. \n");
    return 0;
  }
  if (strcmp(arg,"r") == 0) {
    isa_reg_display();
  } else if (strcmp(arg,"w") == 0) {
    watchpoint_display();
  } else {
    printf("Error:Input Error! Usage: info [r|w]. \n");
  }
  return 0;
}

static char* delete_blank_expr() {
  char *arg = strtok(NULL," ");
  if (arg == NULL) {
    return NULL;
  }
  char *arg1 = strtok(NULL," ");
  while (arg1 != NULL) {
    strcat(arg,arg1);
    arg1 = strtok(NULL," ");
  }
  return arg;
}

static int cmd_x(char *args) {
  char *arg = strtok(NULL," ");
  if (arg == NULL) {
    printf("Error:Input Error! Usage: x [N] [EXPR].\n");
    return 0;
  }
  int n = 0;
  sscanf(arg,"%d",&n);
  char *check = delete_blank_expr();
  if (check == NULL) {
    printf("Error:Input Error! Usage: x [N] [EXPR].\n");
    return 0;
  }
  bool suc = true;
  //TODO:implements expr
  uint32_t addr = expr(check,&suc);
  if (suc == false) {
    return 0;
  } 
  printf("addr:0x%08x\n",addr);
  for (int i = 0 ; i < n; i++,addr +=4) {
    printf("0x%08x\n",paddr_read(addr,4));
  }
  return 0;
}

static int cmd_p (char *args) {
  //char *arg = strtok(NULL, " ");
  char *arg = delete_blank_expr();
  if (arg == NULL) {
    printf("Error: Input Error! Usage: p [EXPR].\n");
    return 0;
  }
  //printf("%s\n",arg);
  bool success = true;
  uint32_t ans = expr(args,&success);
  if (success == false) {
    printf("Error: Input illegal.\n");
  } else {
    printf("%u\n",ans);
  }
  return 0;
}

static int cmd_w(char *args) {
  //char *arg = strtok(NULL," ");
  char *arg = delete_blank_expr();
  //printf("arg: %s\n",arg);
  if (arg == NULL) {
    printf("Error: Input Error! Usage: w [EXPR].\n");
    return 0;
  }
  // WP *p = new_wp();
  // Assert(p, "Watchpoint is NULL.\n");
  // printf("p addr : %#lp\n",p);
  // printf("des addr : %#lp\n",p -> description);
  // strcpy(p -> description, arg);
  // printf("args is %s\n",arg);
  // bool success = true;
  // p -> value = expr(arg,&success);
  // if (success == false) {
  //   printf("Error: Set watchpoint failed!\n");
  //   free_wp(p);
  // }
  add_wp(arg);
  return 0;
}

static int cmd_d(char *args) {
  char *arg = strtok(NULL," ");
  if (arg == NULL) {
    return 0;
  }
  int n = 0;
  sscanf(arg,"%d",&n);
  bool success = false;
  delete_wp(n,&success);
  if (success) {
    printf("Delete watchpoint success!\n");
  } else {
    printf("Error: Delete watchpoint faild.Not exist!\n");
  }
  return 0;
}

static int cmd_help(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },

  /* Add necessary commands for PA1, commands description comes from guide doc*/
  { "si", "Execute program with [N](default 1) steps",cmd_si},
  { "info", "<r> print values of registers ,<w> print infomations of watchpoints",cmd_info},
  { "p", "Calculate the value if given <EXPR>",cmd_p},
  { "x", "Calculate the value of given <expression> and print [N] 4 bytes info starting from <expression>",cmd_x},

  { "w", "Set watchpoint at <EXPR>",cmd_w},
  { "d", "delete NO.[N] watchpoint",cmd_d}
  /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
