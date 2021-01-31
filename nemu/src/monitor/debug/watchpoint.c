#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#include <stdlib.h>

#define NR_WP 32
#define MAX_DES 255
static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp() {
  if (free_ == NULL) {
    printf("Error: No memory to set breakpoint(wp_pool size 32).\n");
    assert(0);
    //return;
  }
  WP* tail = free_;
  free_ = free_ -> next;
  tail -> next = head;
  head = tail;
  head -> description = (char *)malloc(MAX_DES);
  memset(head -> description,'\0',MAX_DES);
  // printf("head addr : %#lp\n",head);
  // printf("head des addr : %#lp\n",head -> description);
  return head;
}

void free_wp(WP *p) {
  if (head == NULL) {
    printf("Error: No watchpoint to free!\n");
    return;
  }
  if (p == head) {
    free(p -> description);
    head = head -> next;
    p -> next = free_;
    free_ = p;
    return;
  }

  WP *q = head;
  while (q -> next != p) {
    q = q -> next;
  } 
  free (q -> next -> description);
  q -> next = p -> next;
  p -> next = free_;
  free_ = p;
}

void delete_wp(int n,bool *success) {
  WP *p = head;
  *success = false;
  while (p != NULL) {
    if (p -> NO == n) {
      free_wp(p);
      *success = true;
      break;
    } else {
      p = p -> next;
    }
  }
}

void detect_wp (bool *success) {
  *success = false;
  WP *p = head;
  while (p != NULL) {
    bool suc = true;
    uint32_t val = expr (p -> description,&suc);
    assert(suc);
    if (val != p -> value) {
      printf("Watchpoint %d : %s has changed!\n",p -> NO,p -> description);
      printf("Old value : %d \t new value : %d \n",p -> value, val);
      p -> value = val;
      *success = true;
    }
    p = p -> next;
  }
}

void watchpoint_display () {
  if (head == NULL) {
    printf("There is no watchpoint.\n");
    return;
  }
  WP *p = head;
  while (p != NULL) {
    printf("%d\t%s\t%#x\n",p -> NO,p -> description,p -> value);
    p = p -> next;
  }

}

void add_wp (char *args) {
  WP *p = new_wp();
  Assert(p, "Watchpoint is NULL.");
  strcpy(p -> description, args);
  bool success = true;
  p -> value = expr(args,&success);
  if (success == false) {
    printf("Error: Set watchpoint failed!\n");
    free_wp(p);
  }
}

