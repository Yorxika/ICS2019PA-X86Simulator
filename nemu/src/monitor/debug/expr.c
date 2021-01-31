#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ,TK_NEQ,
  TK_PLUS, TK_SUB, TK_MUL, TK_DIV,
  TK_LB,TK_RB,
  TK_AND,TK_OR,TK_NOT,
  TK_NUMBER,TK_HEX,
  TK_REG,TK_POINTER
  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", TK_PLUS},     // plus
  {"==", TK_EQ},        // equal
  {"!=", TK_NEQ},       // neq
  {"-", TK_SUB},				// sub
  {"\\*", TK_MUL},			// mul
  {"/", TK_DIV},				// div
  {"0[Xx][0-9a-fA-F]+", TK_HEX},	//hex
  {"[0-9]+", TK_NUMBER},  // num
  {"\\(", TK_LB},					// lb
  {"\\)", TK_RB},					// rb
  {"&&", TK_AND},					// and
  {"\\|\\|", TK_OR},				// or
  {"!", TK_NOT},          // not
  {"\\$(eax|EAX|ebx|EBX|ecx|ECX|edx|EDX|ebp|EBP|esp|ESP|esi|ESI|edi|EDI|eip|EIP|([ABCD][HLX])|([abcd][hlx]))", TK_REG},			// register
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

#define TOKENS_NUM 255

static Token tokens[TOKENS_NUM] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        /*Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);*/
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        if (substr_len >= 32) {
          printf("The length of substring (%.*s) is out of size(32).\n",substr_len,substr_start);
          return false;
        }
        if (nr_token >= TOKENS_NUM) {
          printf("The number of tokens is out of count(%d).\n",TOKENS_NUM);
          return false;
        }
        switch (rules[i].token_type) {
          case TK_NOTYPE: 
              break;
          case TK_NUMBER:
          case TK_HEX:
          case TK_REG:
              strncpy(tokens[nr_token].str,substr_start,substr_len);
              tokens[nr_token].str[substr_len] = '\0';
          default: 
              tokens[nr_token].type = rules[i].token_type;
              nr_token++;
        }

        break;
      }
    }

    // for(int i = 0 ; i < nr_token; ++i) {
    //   printf("str: %s, type: %d\n",tokens[i].str,tokens[i].type);
    // }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

char type2str(int type) {
  char ch = ' ';
  switch (type) {
    case 259:
      ch = '+';
      break;
    case 260:
      ch = '-';
      break;
    case 261:
      ch = '*';
      break;
    case 262:
      ch = '/';
      break;
    case 263:
      ch = '(';
      break;
    case 264:
      ch = ')';
      break;
    case 271:
      ch = 'p';
      break;
    default:
      break;
  }
  return ch;
}

int get_priority (int op) {
  int pri;
	switch(op){
		case TK_OR:
			pri = 0;
			break;
		case TK_AND:
			pri = 1;
			break;
		case TK_EQ:
    case TK_NEQ:
			pri = 2;
			break;
		case TK_PLUS:
		case TK_SUB :
			pri = 3;
			break;
		case TK_MUL:
		case TK_DIV:
			pri = 4;
			break;
    case TK_NOT:
      pri = 5;
      break;
		case TK_POINTER:
			pri = 6;
			break;
		default:
			pri = -1;
	}
	return pri;
}

bool check_parenthese(int left,int right) {
  bool ans = true;
  if (tokens[left].type == TK_LB && tokens[right].type == TK_RB) {
    int nums = 0;
    for (int i = left + 1 ; i < right ;i++) {
      if (tokens[i].type == TK_LB) {
        nums++;
      } else if (tokens[i].type == TK_RB) {
        nums--;
      }
      if (nums < 0) {
        ans = false;
        break;
      }
    }
  } else {
    ans = false;
  }
  return ans;
}

int cmp_pri(int start,int end) {
  int arg1 = get_priority(tokens[start].type);
  int arg2 = get_priority(tokens[end].type);
  return arg1 < arg2 ? -1 : arg1 == arg2 ? 0 : 1;
}

bool is_op(int ch) {
	return ch == TK_PLUS || ch == TK_SUB || ch == TK_MUL || ch == TK_DIV 
    || ch == TK_AND || ch == TK_OR || ch == TK_NOT 
    ||ch == TK_EQ || ch == TK_NEQ 
    || ch == TK_POINTER;
}

int find_op(int start,int end) {
  int cnts = 0,pos = -1;
  for (int i = start; i <= end ; i++) {
    int type = tokens[i].type;
    if (cnts == 0 && is_op(type)) {
      if (pos == -1) {
        pos = i;
      } else if (cmp_pri(i,pos) <= 0) {
        pos = i;
      }
    } else if (type == TK_LB) {
      cnts++;
    } else if (type == TK_RB) {
      cnts--;
    }
  }
  return pos;
}

uint32_t eval(int start,int end,bool *success) {
  if (start > end) {
    printf("Error: start index greater than end index.\n");
    return -1;
  }
  if (*success == false) {
    return -1;
  }
  if (start == end) {
    int ans = 0;
    switch (tokens[start].type) {
    case TK_NUMBER:
      sscanf(tokens[start].str,"%d",&ans);
      break;
    case TK_HEX:
      sscanf(tokens[start].str,"%x",&ans);
      break;
    case TK_REG:
      ans = isa_reg_str2val(tokens[start].str + 1, success);
      if (*success == false) {
        ans = -1;
        printf("Error: Unknown register: %s \n",tokens[start].str);
      }
      break;
    default:
      *success = false;
      printf("Error: Unknown token type: %s \n",tokens[start].str);
      ans = -1;
      break;
    }
    return ans;
  } else if (check_parenthese(start,end)){
    return eval(start + 1, end - 1,success);
  } else {
    int pos = find_op(start,end);
    if (pos < 0) {
      printf("Error: Input Error.\n");
      *success = false;
      return 0;
    }
    if (pos == start || tokens[pos].type == TK_NOT || tokens[pos].type == TK_POINTER) {
      uint32_t ans = eval(start + 1,end,success);
      switch(tokens[pos].type) {
        case TK_NOT:
          ans = !ans;
          break;
        case TK_POINTER:
          ans = paddr_read(ans,4);
          break;
        default:
          *success = false;
          printf("Error: Unknown token type: %s \n",tokens[start].str);
          ans = -1;
          break;
      }
      return ans;
    }
    uint32_t arg1 = 0 ,arg2 = 0 ,ans = 0;
    if (tokens[pos].type != TK_POINTER) {
      arg1 = eval(start,pos - 1,success);
    }
    if (*success == false) {
      return 0;
    }
    arg2 = eval(pos + 1,end ,success);
    if (*success == false) {
      return 0;
    }
    switch(tokens[pos].type) {
      case TK_PLUS:
        ans = arg2 + arg1;
        break;
      case TK_SUB:
        ans = arg1 - arg2;
        break;
      case TK_MUL:
        ans = arg2 * arg1;
        break;
      case TK_DIV:
        if (arg2 == 0) {
          printf("Error: divide 0 !\n");
          *success = false;
          return -1;
        }
        ans = arg1 / arg2;
        break;
      case TK_AND:
        ans = arg2 && arg1;
        break;
      case TK_OR:
        ans = arg1 || arg2;
        break;
      case TK_EQ:
        ans = arg2 == arg1;
        break;
      case TK_NEQ:
        ans = arg2 != arg1;
        break;
      default:
         *success = false;
         printf("Error: Unknown token type: %s \n",tokens[start].str);
         ans = -1;
         break;
    }
    return ans;
  }
}



uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */

  //expr contains '-'  
  for (int i = 0 ; i < nr_token; i++) {
    int type = tokens[i].type;
    if (type == TK_PLUS || type == TK_SUB) {
      int j = i;
      bool isNeg = false;
      while (j < nr_token && (type == TK_PLUS || type == TK_SUB)) {
        isNeg = type == TK_PLUS ? isNeg : !isNeg;
        type = tokens[++j].type;
      }
      if (j - i > 1) {
        tokens[i].type = isNeg ? TK_SUB : TK_PLUS;
        int right = j - i - 1;
        //delete not necessary sub or plus
        for (int left = i + 1 ; left < nr_token - right; left++) {
          tokens[left] = tokens[left + right];
        }
        nr_token -= right;
      }
    }

    //if pointer
    if (tokens[i].type == TK_MUL && (i == 0 || is_op(tokens[i - 1].type) || tokens[i - 1].type == TK_LB)) {
      if (i == 0 || tokens[i - 1].type != TK_POINTER) {
        tokens[i].type = TK_POINTER;
      }
    }
  }
  
  // for(int i = 0 ; i < nr_token; ++i) {
  //   printf("str: %s, type: %c\n",tokens[i].str,type2str(tokens[i].type));
  // }

  return eval(0,nr_token - 1,success);
}


