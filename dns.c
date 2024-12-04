#include "dns.h"

/*
 * 读取文件
 * params:
 *  - filename: 文件名
 *  - domains: 存储域名的数组
 *  - ips: 存储IP地址的数组
 * return
 *  成功返回 0，失败返回 -1
 */
int read_domains_and_ips(const char* filename,
                         char domains[MAX_ENTRIES][MAX_DOMAIN_LENGTH],
                         char ips[MAX_ENTRIES][MAX_IP_LENGTH], int* count) {
  FILE* file = fopen(filename, "r");
  if (file == NULL) {
    return -1;
  }

  char line[512];
  *count = 0;

  while (fgets(line, sizeof(line), file)) {
    char* domain = strtok(line, " ");
    char* ip = strtok(NULL, "\n");

    if (domain != NULL && ip != NULL) {
      if (strlen(domain) < MAX_DOMAIN_LENGTH && strlen(ip) < MAX_IP_LENGTH) {
        strcpy(domains[*count], domain);
        strcpy(ips[*count], ip);
        (*count)++;
      }
    }

    if (*count >= MAX_ENTRIES) {
      break;
    }
  }

  fclose(file);
  return 0;
}

/*
 * 创建节点
 * params:
 *  - new_node: 存放新插入节点指针的地址
 *  - sub_domain: 新插入节点的子域名
 *  - ip: 新插入节点的 IP 地址
 * return:
 *  成功返回 0，失败返回 -1
 */
int create_node(node** new_node, char* sub_domain, char* ip) {
  *new_node = (node*)malloc(sizeof(node));
  if (NULL == new_node) {
    return -1;
  }

  memset((*new_node)->sub_domain_, '\0', sizeof((*new_node)->sub_domain_));
  memset((*new_node)->ip_, '\0', sizeof((*new_node)->ip_));
  memcpy((*new_node)->sub_domain_, sub_domain, sizeof(sub_domain));
  memcpy((*new_node)->ip_, ip, strlen(ip));
  (*new_node)->left_child_ = NULL;
  (*new_node)->right_sibling_ = NULL;

  return 0;
}

/*
 * 初始化N叉树
 * params:
 *  - tree: 初始化好的N叉树指针的指针，
 *      传二级指针的原因是我要返回函数中的局部变量，而这个局部变量恰好也是个指针
 * return:
 *  成功返回 0，失败返回 -1
 */
int init_tree(nary_tree** tree) {
  node* root = NULL;
  assert(0 == create_node(&root, "", ""));

  *tree = (nary_tree*)malloc(sizeof(nary_tree));
  if (NULL == (*tree)) {
    return -1;
  }

  (*tree)->root_ = root;

  return 0;
}

/*
 * 在 pos 下面添加 new_node 节点
 * params:
 *  - pos: new_node的父节点
 *  - new_node: 要插入的节点
 * return:
 *  成功返回 0，失败返回 -1
 */
int insert_node(node* pos, node* new_node) {
  if (NULL == pos || NULL == new_node) {
    return -1;
  }

  if (pos->left_child_ == NULL) {
    pos->left_child_ = new_node;
  } else {
    node* temp = pos->left_child_;
    while (temp->right_sibling_ != NULL) {
      temp = temp->right_sibling_;
    }
    temp->right_sibling_ = new_node;
  }

  return 0;
}

/*
 * 通过子域名和ip创建并插入节点，相当于对上面的 create_node 和 insert_node
 * 再做了一层封装 params:
 *  - pos: 新插入节点的父节点
 *  - ret_node: 存放新插入节点指针的地址
 *  - sub_domain: 新插入节点的子域名
 *  - ip: 新插入节点的 IP 地址
 * return:
 *  成功返回 0，不成功进程会直接被干掉
 */
int insert_node_by_kv(node* pos, node** ret_node, char* sub_domain, char* ip) {
  node* new_node = NULL;
  assert(0 == create_node(&new_node, sub_domain, ip));
  assert(0 == insert_node(pos, new_node));
  *ret_node = new_node;

  return 0;
}

/*
 * 在 sub_root 的子节点中查找 sub_domain_ == sub_domain 的成员
 * params:
 *  - sub_root: 要查找的子树的根节点
 *  - pos: 用于记录答案，*pos指针会指向查找到的子节点
 *  - sub_domain: 查找的子域名
 * return:
 *  查找不成功，返回 -1；找不到符合条件的子节点，并置 *pos 为 NULL，返回 0；
 *  找到符合条件的子节点，则为 *pos 赋值，并返回 1
 */
int search_node(node* sub_root, node** pos, char* sub_domain) {
  if (NULL == sub_root) {
    return -1;
  }

  node* temp = sub_root->left_child_;
  while (NULL != temp && 0 != strcmp(sub_domain, temp->sub_domain_)) {
    temp = temp->right_sibling_;
  }

  *pos = temp;
  if (NULL == temp) {
    return 0;
  } else {
    return 1;
  }
}

/*
 * 暴露给用户的接口，domain是完整的域名，ip是对应的IP地址
 * params:
 *  - tree: 要添加节点的树
 *  - domain: 要添加的域名
 *  - ip: 要添加的 IP 地址
 * return:
 *  添加成功返回 0，失败返回 -1
 */
int insert(nary_tree* tree, char* u_domain, char* ip) {
  char domain[MAX_DOMAIN_LENGTH];
  memset(domain, '\0', sizeof(domain));
  memcpy(domain, u_domain, strlen(u_domain));
  int len = strlen(domain);
  int idx = len;
  int ret = 0;
  node* root = tree->root_;
  node* pos = root;
  char sub_domain[20];

  int orig_len = len;
  while (len > 0) {
    while (len > 0 && domain[--len] != '.')
      ;
    memset(sub_domain, '\0', sizeof(sub_domain));
    idx = (len == 0) ? 0 : len + 1;
    int sub_len = orig_len - idx;  // 计算子域名的长度
    orig_len = len;
    memcpy(sub_domain, domain + idx, sub_len);
    sub_domain[sub_len] = '\0';  // 确保子域名以 '\0' 结尾

    domain[len] = '\0';  // 截取domain中剩余的有效片段
    // printf("%s\n", sub_domain);
    ret = search_node(root, &pos, sub_domain);
    if (0 == ret) {  // 没有找到 sub_domain 对应的节点
      node* new_node = NULL;
      if (len == 0) {  // 插入最后一级域名，添加IP
        assert(0 == insert_node_by_kv(root, &new_node, sub_domain, ip));
      } else {  // 否则不添加IP
        assert(0 == insert_node_by_kv(root, &new_node, sub_domain, NONE_IP));
      }
      root = new_node;      // 更新root指针
    } else if (1 == ret) {  // 找到了 sub_domain 对应的节点
      root = pos;
    } else {  // 查找失败
      return -1;
    }
  }

  return 0;
}

/*
 * 查找domain对应的ip，并存放到ret_ip中去
 * params:
 *  - tree: 树
 *  - domain: 域名
 *  - ret_ip: 存放找到的IP
 */
int search(nary_tree* tree, char* u_domain, char* ret_ip) {
  char domain[MAX_DOMAIN_LENGTH];
  memset(domain, '\0', sizeof(domain));
  memcpy(domain, u_domain, strlen(u_domain));
  int len = strlen(domain);
  int idx = len;
  int ret = 0;
  node* root = tree->root_;
  node* pos = root;
  char sub_domain[20];

  int orig_len = len;
  while (len > 0) {
    while (len > 0 && domain[--len] != '.')
      ;
    memset(sub_domain, '\0', sizeof(sub_domain));
    idx = (len == 0) ? 0 : len + 1;
    int sub_len = orig_len - idx;  // 计算子域名的长度
    orig_len = len;
    memcpy(sub_domain, domain + idx, sub_len);
    sub_domain[sub_len] = '\0';  // 确保子域名以 '\0' 结尾

    domain[len] = '\0';  // 截取domain中剩余的有效片段

    ret = search_node(root, &pos, sub_domain);
    if (1 == ret) {  // 找到了 sub_domain 对应的节点
      if (len > 0) {
        root = pos;
      } else {
        memcpy(ret_ip, pos->ip_, strlen(pos->ip_));
      }
    } else {  // 查找失败
      return -1;
    }
  }

  return 0;
}

/*
 * 修改domain对应的IP
 * params:
 *  - tree: 要修改的树
 *  - domain: 要修改IP的域名
 *  - ip: 要重写的IP
 */
int modify(nary_tree* tree, char* u_domain, char* ip) {
  char domain[MAX_DOMAIN_LENGTH];
  memset(domain, '\0', sizeof(domain));
  memcpy(domain, u_domain, strlen(u_domain));
  int len = strlen(domain);
  int idx = len;
  int ret = 0;
  node* root = tree->root_;
  node* pos = root;
  char sub_domain[20];

  int orig_len = len;
  while (len > 0) {
    while (len > 0 && domain[--len] != '.')
      ;
    memset(sub_domain, '\0', sizeof(sub_domain));
    idx = (len == 0) ? 0 : len + 1;
    int sub_len = orig_len - idx;  // 计算子域名的长度
    orig_len = len;
    memcpy(sub_domain, domain + idx, sub_len);
    sub_domain[sub_len] = '\0';  // 确保子域名以 '\0' 结尾
    domain[len] = '\0';          // 截取domain中剩余的有效片段

    ret = search_node(root, &pos, sub_domain);
    if (1 == ret) {  // 找到了 sub_domain 对应的节点
      if (len > 0) {
        root = pos;
      } else {
        memcpy(pos->ip_, ip, strlen(ip));
      }
    } else {  // 查找失败
      return -1;
    }
  }

  return 0;
}

/*
 * 先序遍历的接口
 * params:
 *  - tree: 要遍历的树
 */
void traverse_nary_tree(nary_tree* tree) {
  if (tree == NULL || tree->root_ == NULL) {
    return;
  }
  traversal(tree->root_);
}

/*
 * 先序遍历检查树结构是否正确（自己写单测的时候可以用）
 * params:
 *  - root: 要遍历的子树的根节点
 * return:
 */
void traversal(node* root) {
  if (root == NULL) {
    return;
  }

  printf("sub_domain: %s, IP: %s\n", root->sub_domain_, root->ip_);
  if (NULL != root->left_child_) {
    traversal(root->left_child_);
    node* sibling = root->left_child_->right_sibling_;
    while (sibling != NULL) {
      traversal(sibling);
      sibling = sibling->right_sibling_;
    }
  }
}

/*
 * 释放内存的接口
 * params:
 *  - tree: 要释放的树
 * return:
 *  总是成功，返回 0
 */
int free_tree(nary_tree* tree) {
  if (tree == NULL || tree->root_ == NULL) {
    return 0;
  }
  destroy_tree(tree->root_);
}

/*
 * 后序遍历释放内存
 * params:
 *  - root: 要遍历的子树的根节点
 * return:
 */
void destroy_tree(node* root) {
  if (root == NULL) {
    return;
  }

  if (NULL != root->left_child_) {
    destroy_tree(root->left_child_);
    node* sibling = root->left_child_->right_sibling_;
    while (sibling != NULL) {
      destroy_tree(sibling);
      sibling = sibling->right_sibling_;
    }
  }
  // printf("destory -> sub_domain: %s, IP: %s\n", root->sub_domain_,
  // root->ip_);
  free(root);
}