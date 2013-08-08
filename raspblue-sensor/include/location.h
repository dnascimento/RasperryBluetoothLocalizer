#ifndef LOCATION_H_
#define LOCATION_H_

#include "listType.h"
#include "map.h"
#include "red_black_tree.h"

typedef enum data_type{ ENTRY, COUNT, RSS} DataType;
typedef enum rss_type{ WIFI, BLUETOOTH, RFID, OTHER} RSSType;
typedef enum message_type { REQUEST_INSTANT, REQUEST_FREQUENT, REGISTER_SENSOR, REGISTER_MANAGER, CONFIRM_SENSOR, CONFIRM_MANAGER, SENSOR_DATA, MANAGER_DATA } MessageType;
typedef LList SensorDataList;


typedef struct rss{
	RSSType type;
	unsigned short node_number;	//numero de dispositivos que vem nesta struct
	double * rss;		//array com os RSS de cada no
	 char * nodes;  //MD5 do num de nos
}RSSInfo;

typedef struct tri_info{
	unsigned char * node;
	Location s1,s2,s3;
	uint16_t r1,r2,r3;
	bool b2,b3;
}TriInfo;

typedef struct sensor_data{
	DataType type;
	union{
		long entrances; //POS OR NEG - COUNTING NUMBER UPDATED FOR EACH CICLE
		unsigned short people;
		RSSInfo RSS;
	};
}SensorData;

typedef struct spotter{
	uint16_t id;
	Location location;
	RSSInfo * current_info;
	uint64_t last_received;
}Spotter;

typedef struct manager{
	uint16_t id;
	uint8_t map_id;
	rb_red_blk_tree * current_info;
	unsigned short num_people;
	uint64_t last_received;
}Manager;


typedef struct location_packet{
	MessageType type;
	union{
		struct {
			Location sensor_location;
			unsigned short min_update_frequency;
		}RegSensor;
		unsigned short manager_area_id;
		SensorDataList data;
		struct {
			rb_red_blk_tree * people;
			unsigned short num_people;
		}Manager_data;
	};
	unsigned short required_frequency; //FOR Confirmation of spont. registry or for request frequent service
}LocationPacket;

#endif /* LOCATION_H_ */
