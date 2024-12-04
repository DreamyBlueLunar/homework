#pragma once

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DOMAIN_LENGTH 256
#define MAX_IP_LENGTH 20
#define MAX_ENTRIES 1000
#define NONE_IP ((char*)"")

typedef struct node node;
typedef struct nary_tree nary_tree;

struct node {
  char sub_domain_[20];
  char ip_[20];
  struct node* left_child_;
  struct node* right_sibling_;
};

struct nary_tree {
  node* root_;
};

int read_domains_and_ips(const char* filename,
                         char domains[MAX_ENTRIES][MAX_DOMAIN_LENGTH],
                         char ips[MAX_ENTRIES][MAX_IP_LENGTH], int* count);
int init_tree(nary_tree** tree);
int free_tree(nary_tree* tree);
void destroy_tree(node* root);
int create_node(node** new_node, char* sub_domain, char* ip);
int insert_node(node* pos, node* new_node);
int insert_node_by_kv(node* pos, node** ret_node, char* sub_domain, char* ip);
int search_node(node* sub_root, node** pos, char* sub_domain);
int insert(nary_tree* tree, char* domain, char* ip);
int search(nary_tree* tree, char* domain, char* ret_ip);
int modify(nary_tree* tree, char* domain, char* ip);
void traversal(node* root);
void traverse_nary_tree(nary_tree* tree);