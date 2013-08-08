#ifndef LOCATION_JSON_H_
#define LOCATION_JSON_H_

#include "location.h"

bool generate_JSON(LocationPacket * packet, unsigned char ** response, size_t * length);

bool generate_packet_from_JSON(char * data, LocationPacket * packet);

int CompareNodes(const void * key1, const void * key2);

void PrintLocation(void * info);

void PrintKey(const void * key);

#endif /* LOCATION_JSON_H_ */