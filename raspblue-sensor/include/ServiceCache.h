#ifndef __SERVICECACHE_H__
#define __SERVICECACHE_H__

#include "listType.h"
#include "discovery.h"


typedef LList CacheList;

typedef struct service_cache{
	uint16_t source_address;
	unsigned short lifetime;
	unsigned short broadcast_id;
	bool local;
	ServiceList services; 
	GroupList vicinity_groups;
}ServiceCache;

void GetLocal_ServiceInfo(ServiceList *);

//LList GetLocal_ServiceGroupInfo(void);

void GetVicinity_GroupInfo(GroupList *);

void print_cache();

void FilterCache();

void CleanPreviousCache(uint16_t address);

#endif
