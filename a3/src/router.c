/**
 * Name: Alvi Akbar
 * NSID: ala273
 * Student Number: 11118887
 */

#include <arpa/inet.h>
#include <dbg.h>
#include <router.h>

#define min(x, y) (x) < (y) ? (x) : (y)

static router_table_t router_table = { NULL, NULL };

/**
 * @brief  Stores the port number and name in struct route
 * @note
 * @param  port: servers port number
 * @param  name: server name
 * @retval
 */
route_path_t*
add_gateway(uint16_t port, const char* name)
{
  route_path_t* route = (route_path_t*)malloc(sizeof(route_path_t));

  memset(route, 0, sizeof(route_path_t));
  route->dst = port;
  memcpy(route->name, name, 2);

  return route;
}

/**
 * @brief  Create space for the list to be linked with router path received
 * from the neighbour routers input arguments - route_path received from the
 * neighbour routers
 * @note
 * @param  route_path: route path info
 * @retval
 */
route_path_t*
add_route_path(route_path_t* route_path)
{
  route_path_t* route = (route_path_t*)malloc(sizeof(route_path_t));
  memcpy(route, route_path, sizeof(route_path_t));

  return route;
}

/**
 * @brief  Creates space for the list to be linked with router path returned
 * by the add_gateway function input argument - route_path structure returned by
 * add_gateway function
 * @note
 * @param  route: route path info
 * @retval
 */
void
add_router_entry(route_path_t* route)
{
  router_entry_t* route_path = (router_entry_t*)malloc(sizeof(router_entry_t));
  route_path->route = route;

  if (router_table.tail != NULL)
    router_table.tail->next = route_path;

  router_table.tail = route_path;

  if (router_table.head == NULL)
    router_table.head = route_path;
}

/**
 * @brief  mark the given path as unreachable with the infinite cost which is
 * choosen as 16
 * @note
 * @param  id: ID of the broken link
 * @retval
 */
void
route_disconnected(int id)
{
  for (router_entry_t* elem = router_table.head; elem != NULL;
       elem = elem->next) {

    if (elem->id == id)
      elem->route->cost = INF;
  }
}

/**
 * @brief search the entire table for the given router destination if it present
 * return its address else return NULL
 * @note
 * @param  route: route path info
 * @retval
 */
router_entry_t*
search_table(route_path_t* route)
{
  for (router_entry_t* elem = router_table.head; elem != NULL;
       elem = elem->next) {

    if (elem->route->dst == route->dst)
      return elem;
  }
  return NULL;
}

/**
 * @brief  prints all the entry in the router table
 * @note
 * @retval
 */
void
print_Table()
{
  printf("-----------------------------------------\n");
  printf("| %5s | %5s | %5s | %5s | %5s |\n",
         "Name",
         "Port",
         "GWay",
         "Port",
         "Cost");
  printf("-----------------------------------------\n");

  for (router_entry_t* elem = router_table.head; elem != NULL;
       elem = elem->next) {

    route_path_t* path = elem->route;

    printf("| %5.2s | %5d | %5.2s | %5d | %5d |\n",
           path->name,
           path->dst,
           path->nxt_name,
           path->nxt_hop,
           path->cost);
  }

  printf("-----------------------------------------\n\n");
}

/**
 * @brief  clears the router table and frees all the resources
 * @note
 * @retval
 */
void
remove_table()
{
  router_entry_t* elem = router_table.head;
  router_entry_t* next_elem;

  if (elem == NULL)
    return;

  do {
    next_elem = elem->next;

    free(elem->route);
    free(elem);

    elem = next_elem;

  } while (elem != NULL);
}

/**
 * @brief  Updates the old entry or creates new entry if needed
 * @note
 * @param  route: route of the router
 * @param  gateway: neighbouring route to this route is connected
 * @param  id: id of the connection useful to mark when disconneted
 * @retval
 */
void
update_Table(route_path_t* route, route_path_t* gateway, int id)
{
  router_entry_t* route_entry;
  if (route->cost == 0)
    memcpy(gateway, route, sizeof(route_path_t)); /* mark it as gateway */

  /* search the table for the route->dst */
  if ((route_entry = search_table(route)) == NULL) {
    route->cost += 1;

    /* next hop to neighouring router which send this message */
    route->nxt_hop = gateway->dst;
    *(uint16_t*)route->nxt_name = *(uint16_t*)gateway->name;

    add_router_entry(add_route_path(route));
    router_table.tail->id = id;

  } else if (route_entry->route->cost != 0) {

    /* check if its own port address */
    if (route_entry->route->nxt_hop == gateway->dst)
      route_entry->route->cost = min((route->cost + 1), INF);

    else if (route_entry->route->cost > (route->cost + 1)) {

      route_entry->route->nxt_hop = gateway->dst;
      route_entry->route->cost = min((route->cost + 1), INF);

      *(uint16_t*)route_entry->route->nxt_name = *(uint16_t*)gateway->name;
      route_entry->id = id;
    }
  }
}

/**
 * @brief  Reads the bytes received over tcp and convert them to route_path
 * structure and updates the table
 * @note
 * @param  buffer: address of the byte array where the received bytes are stored
 * @param  size: size of the received bytes
 * @param  id: id of the connection
 * @retval
 */
void
parse_data(uint8_t* buffer, int size, int id)
{
  uint16_t* packet = (uint16_t*)buffer;
  route_path_t gateway;
  int i;

  /* find the header packet */
  for (i = 0; packet[i] == 0xFFFF; i++)
    ;

  /* find the number of the route path from this message */
  uint16_t length = ntohs(packet[i++]);

  while (length-- > 0) {
    route_path_t* route = (route_path_t*)&packet[i];

    for (int idx = 0; idx < (sizeof(route_path_t) / sizeof(uint16_t)); idx++)
      route->word[idx] = ntohs(route->word[idx]);

    i += (sizeof(route_path_t) / sizeof(uint16_t));
    update_Table(route, &gateway, id);
  }
}

/**
 * @brief  Converts the router table to a serial buffer bytes to be sent over
 * TCP
 * @note
 * @param  buffer: address of the byte array
 * @retval size of the array
 */

uint16_t
form_packet(uint8_t* buffer)
{
  int length = 0;
  uint16_t* packet = (uint16_t*)buffer;
  packet[0] = 0xFFFF;

  /* loop through all the elements in the table and convert to network byte */
  /* order and store in the buffer */
  for (router_entry_t* elem = router_table.head; elem != NULL;
       elem = elem->next) {

    for (int i = 0; i < (sizeof(route_path_t) / sizeof(uint16_t)); i++) {
      packet[i + (length * (sizeof(route_path_t) / sizeof(uint16_t))) + 2] =
        htons(elem->route->word[i]);
    }

    length++;
  }

  packet[1] = htons(length);
  return ((length * sizeof(route_path_t)) + 4);
}
