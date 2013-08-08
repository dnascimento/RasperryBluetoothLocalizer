#include "location.h"

/* some functions re-used between modules of inq */
void reporter_add(inquiry_info*);
void reporter_add_with_rssi(inquiry_info_with_rssi*);

SensorData * reporter_swap();
