#ifndef MAP_H_
#define MAP_H_

#include "vec3d.h"
#include "red_black_tree.h"

typedef struct transition{
	unsigned short area_id;
	unsigned short x_cell;
	unsigned short y_cell;
}Transition;

typedef struct cell{
	unsigned short prob_location;
	//unsigned short prob_move[6];
	bool transition_cell;
	Transition transp;
}Cell;

typedef struct map{
	unsigned short area_id;
	Cell **cells;
	unsigned short x_cells;
	unsigned short y_cells;
	unsigned short floor;
	unsigned short cell_size;
	unsigned short num_people;
	rb_red_blk_tree * people;
}Map;

typedef struct location{
	unsigned short x;
	unsigned short y;
	unsigned short area_id;
}Location;

typedef struct global_location{
	Location current;
	Location previous;
	bool conflicted;
	bool has_previous;
	uint64_t last_update;
}GlobalLocation;

void LoadMultiMaps(char * folder, LList * maps_loaded);
Map * LoadMap(char * map_info);
void DestroyMap(Map * map);
Location * InfoToCell(Map * map, vec3d * rs1, vec3d * rs2);
bool CheckTransition(LList * maps, Location * previous, Location  * comitted_pos, Location * conflicted_pos, bool prev_conflicted);
void PrintMap(Map *);

#endif /* MAP_H_ */
