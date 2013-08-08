#ifndef GSD_API_H_
#define GSD_API_H_

#include <fred/handler.h>
#include <stdint.h>
#include "discovery.h"

bool RegisterService(Service * serv, __tp(handler)* handler_th, uint16_t handler_id, void (* service_found_cb)(uint16_t,uint16_t));
uint16_t RequestService(char *);
bool StopProvidingService(uint16_t handler_id);
void GsdReceive(char *);

#endif /* GSD_API_H_ */