/*
 * spotter_services.c
 *
 *  Created on: Jun 22, 2011
 *      Author: root
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <arpa/inet.h>
#include <fred/handler.h>
#include "location.h"
#include "location_json.h"
#include "spotter.h"
#include "spotter_services.h"

extern Location self;
extern __tp(handler)* handler;


void RequestInstant(uint16_t address){
}

void RequestFrequent(uint16_t manager_address, unsigned short required_frequence){
}

void ConfirmSpontaneousRegister(uint16_t manager_address, unsigned short final_frequence){
}
