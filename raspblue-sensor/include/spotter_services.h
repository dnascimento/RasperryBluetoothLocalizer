#ifndef SPOTTER_SERVICES_H_
#define SPOTTER_SERVICES_H_

void RequestInstant(uint16_t address);
void RequestFrequent(uint16_t manager_address, unsigned short required_frequence);
void ConfirmSpontaneousRegister(uint16_t manager_address, unsigned short final_frequence);

#endif /* SPOTTER_SERVICES_H_ */