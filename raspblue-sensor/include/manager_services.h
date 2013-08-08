/*
 * spotter_services.h
 *
 *  Created on: Jun 24, 2011
 *      Author: root
 */
 
#ifndef MANAGER_SERVICES_H_
#define MANAGER_SERVICES_H_

void RequestInstant(uint16_t address);
void RequestFrequent(uint16_t aggregator_address, unsigned short frequency);
void ConfirmSpotter(uint16_t spotter_address, Location location, unsigned short final_frequency);
void SpontaneousSpotter(uint16_t spotter_address, Location location, unsigned short max_frequency);
void DeliverSpotterData(uint16_t spotter_address, LocationPacket * packet, uint64_t time_sent);
void ConfirmSpontaneousRegister(uint16_t aggregator_address, unsigned short final_frequence);

#endif /* MANAGER_SERVICES_H_ */