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

/*
 * Function to Store the Port Number and Name in route structure used to link it
 * in linked list input argument port - port number to store name - same of the
 * server
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

/*
 * Function create space for the list to be linked with router path received
 * from the neighbour routers input arguments - route_path received from the
 * neighbour routers
 */
route_path_t*
add_route_path(route_path_t* route_path)
{
  route_path_t* route = (route_path_t*)malloc(sizeof(route_path_t));
  memcpy(route, route_path, sizeof(route_path_t));
  return route;
}

/*
 * Function creates space for the list to be linked with router path returned
 * by the add_gateway function input argument - route_path structure returned by
 * add_gateway function
 */
void
add_router_entry(route_path_t* route)
{
  router_entry_t* route_path = (router_entry_t*)malloc(sizeof(router_entry_t));
  route_path->route = route;

  if (router_table.tail != NULL) {
    router_table.tail->next = route_path;
  }
  router_table.tail = route_path;

  if (router_table.head == NULL) {
    router_table.head = route_path;
  }
}

/*
 * Function mark the given path as unreachable with the infinite cost which is
 * choosen as 16 input argument ID of the broken link
 */
void
route_disconnected(int id)
{
  for (router_entry_t* elem = router_table.head; elem != NULL;
       elem = elem->next) {
    if (elem->id == id) {
      elem->route->cost = INF;
    }
  }
}

/*
 * Function search the entire table for the given router dst if it present
 * return its address else return NULL
 */

router_entry_t*
search_table(route_path_t* route)
{
  for (router_entry_t* elem = router_table.head; elem != NULL;
       elem = elem->next) {
    if (elem->route->dst == route->dst) {
      return elem;
    }
  }
  return NULL;
}

/*
 *   Function prints all the entry in the router table
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

/*
 *  Function clears the router table and frees all the resources
 */
void
remove_table()
{
  router_entry_t* elem = router_table.head;
  router_entry_t* next_elem;
  if (elem == NULL) {
    return;
  }
  do {
    next_elem = elem->next;
    free(elem->route);
    free(elem);
    elem = next_elem;
  } while (elem != NULL);
}

/*
 * Function updates the old entry or creates new entry if needed
 * input argument route - route of the router
 *                gateway - neighbouring route to this route is connected
 *                id  - id of the connection useful to mark when disconneted
 * returns none
 */
void
update_Table(route_path_t* route, route_path_t* gateway, int id)
{
  router_entry_t* route_entry;
  if (route->cost == 0) { // check if its neighouring router
    memcpy(gateway, route, sizeof(route_path_t)); // mark it as gateway
  }

  if ((route_entry = search_table(route)) ==
      NULL) {         // search the table for the route->dst
    route->cost += 1; // if not found then its add new destion path add +1 for
                      // hopping this router to neighouring router
    route->nxt_hop =
      gateway->dst; // next hop to neighouring router which send this message
    *(uint16_t*)route->nxt_name = *(uint16_t*)gateway->name;
    add_router_entry(add_route_path(route)); // create new router path
    router_table.tail->id = id;              // store the id
  }                                          // if the route is already exist
  else if (route_entry->route->cost != 0) {  // check if its own port address
    if (route_entry->route->nxt_hop ==
        gateway->dst) { // else check if its a which neighouring router if same
      route_entry->route->cost =
        min((route->cost + 1),
            INF); // just copy the cost because the broken link have higher cost
    } else if (route_entry->route->cost >
               (route->cost +
                1)) { // else different neighour, check if cost is lower
      route_entry->route->nxt_hop =
        gateway->dst; // if lower copy the neighour address
      route_entry->route->cost = min((route->cost + 1), INF); // update the cost
      *(uint16_t*)route_entry->route->nxt_name = *(uint16_t*)gateway->name;
      route_entry->id = id; // store the id
    }
  }
}

/*
 * Function read the bytes received over tcp and convert them to route_path
 * structure and updates the table input argument buffer - address of the byte
 * array where the received bytes are stored size  - size of the received bytes
 *                id    - id of the connection
 * returns none
 */
void
parse_data(uint8_t* buffer, int size, int id)
{
  uint16_t* packet = (uint16_t*)buffer;
  route_path_t gateway;
  int i;

  // while(size > (sizeof(route_path_t) + 4)) {
  for (i = 0; packet[i] == 0xFFFF; i++)
    ; // find the header packet
  uint16_t length =
    ntohs(packet[i++]); // find the number of the route path from this message
  // printf("length : %d\n", length);
  // int j = 0;
  // size -= (length * (sizeof(route_path_t))) + 4;
  while (length-- > 0) { // loop through all the routes
    route_path_t* route = (route_path_t*)&packet[i]; // cast the array buffer to
                                                     // route_path structure
    for (int idx = 0; idx < (sizeof(route_path_t) / sizeof(uint16_t)); idx++) {
      route->word[idx] = ntohs(
        route->word[idx]); // convert network byte order to host byte order
    }
    // printf("%d. DST : %d(%.2s) Nxt_Hop : %d(%.2s) cost : %d\n",++j,
    // route->dst, route->name, route->nxt_hop,
    //                                                           route->nxt_name,
    //                                                           route->cost);
    i +=
      (sizeof(route_path_t) / sizeof(uint16_t)); // increment to next structure
    update_Table(route, &gateway, id);           // update the table
  }

  //}
}

/*
 * Function convert the router table to a serial buffer bytes to be sent over
 * the TCP input argument buffer - address of the byte array returns size of the
 * array
 */
uint16_t
form_packet(uint8_t* buffer)
{
  int length = 0;
  uint16_t* packet = (uint16_t*)buffer;
  packet[0] = 0xFFFF; // packet header
  // loop through all the elements in the table and convert to network byte
  // order and store in the buffer
  for (router_entry_t* elem = router_table.head; elem != NULL;
       elem = elem->next) {
    for (int i = 0; i < (sizeof(route_path_t) / sizeof(uint16_t)); i++) {
      packet[i + (length * (sizeof(route_path_t) / sizeof(uint16_t))) + 2] =
        htons(elem->route->word[i]);
    }
    length++;
  }

  packet[1] = htons(length);                    // number of routes
  return ((length * sizeof(route_path_t)) + 4); // size of the byte array
}
