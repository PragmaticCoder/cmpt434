#ifndef __ROUTER_H__
#define __ROUTER_H__

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INF 16

typedef union
{
  struct
  {
    uint16_t dst;
    char name[2];
    uint16_t nxt_hop;
    char nxt_name[2];
    uint16_t cost;
  };

  uint16_t word[5];
  uint8_t byte[10];

} route_path_t;

typedef struct _router_entry
{
  int id;
  route_path_t* route;
  struct _router_entry* next;
} router_entry_t;

typedef struct
{
  router_entry_t* head;
  router_entry_t* tail;
} router_table_t;

uint16_t
form_packet(uint8_t*);

void
parse_data(uint8_t*, int, int);

route_path_t*
add_gateway(uint16_t, const char*);

void
add_router_entry(route_path_t*);

void
remove_table();

void
print_Table();

void
route_disconnected(int);
#endif
