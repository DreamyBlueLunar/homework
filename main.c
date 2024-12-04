#include "dns.h"

int main(void) {
  char domains[MAX_ENTRIES][MAX_DOMAIN_LENGTH];
  char ips[MAX_ENTRIES][MAX_IP_LENGTH];
  int count = 0;

  assert(0 == read_domains_and_ips("domain_and_ip", domains, ips, &count));

  nary_tree* tr = NULL;
  init_tree(&tr);
  for (int i = 0; i < MAX_ENTRIES && domains[i][0] != '\0'; i++) {
    insert(tr, domains[i], ips[i]);
  }
  char domain[MAX_DOMAIN_LENGTH] = "mxfzwkui.vgcykv.edu";
  char ret[MAX_IP_LENGTH] = {'\0'};
  assert(0 == search(tr, domain, ret));
  printf("domain: mxfzwkui.vgcykv.edu, IP: %s\n", ret);
  char ip[MAX_IP_LENGTH] = "192.168.114.156";
  assert(0 == modify(tr, "mxfzwkui.vgcykv.edu", ip));
  assert(0 == search(tr, domain, ret));
  printf("domain: mxfzwkui.vgcykv.edu, IP: %s\n", ret);
  free_tree(tr);

  return 0;
}