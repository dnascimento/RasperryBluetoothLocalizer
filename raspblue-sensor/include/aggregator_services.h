/*
 * aggregator_services.h
 *
 *  Created on: Jul 11, 2011
 *      Author: root
 */

#ifndef AGGREGATOR_SERVICES_H_
#define AGGREGATOR_SERVICES_H_

void RequestInstant(uint16_t address);
void RequestFrequent(uint16_t server_address, unsigned short frequency);
void ConfirmManager(uint16_t manager_address, uint8_t map_id, unsigned short final_frequency);
void SpontaneousManager(uint16_t spotter_address, uint8_t map_id, unsigned short max_frequency);
void DeliverManagerData(uint16_t spotter_address, LocationPacket * packet, uint64_t time_sent);

#endif /* AGGREGATOR_SERVICES_H_ */
