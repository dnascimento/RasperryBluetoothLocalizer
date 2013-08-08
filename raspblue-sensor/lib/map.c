/*
 * map.c
 *
 *  Created on: Jul 28, 2011
 *      Author: root
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <yajl/yajl_parse.h>
#include <yajl/yajl_tree.h>
#include "listType.h"
#include "location.h"
#include "location_json.h"
#include <string.h>
#include <arpa/inet.h>
#include <fred/handler.h>
#include "map.h"
#include <dirent.h>

void LoadMultiMaps(char * folder, LList * maps_loaded){
	
	DIR *dir;
   	struct dirent *de;
   	FILE *fp;
   	long lSize;
   	char *buffer;

   	CreateList(maps_loaded);

    dir = opendir(folder); 
    while(dir)
    {
        de = readdir(dir);
        if (!de) break;
	if(!strcmp(de->d_name, ".")) continue;
	if(!strcmp(de->d_name, "..")) continue;
        fp = fopen(de->d_name, "rb");
        if (!fp) continue;

        fseek(fp,0L,SEEK_END);
        lSize = ftell(fp);
        rewind(fp);

        buffer = calloc(1, lSize+1);
        if (!buffer) fclose(fp),fputs("ERROR: memory alloc fails", stderr),exit(1);

        if (1!=fread(buffer, lSize, 1, fp))
        	fclose(fp),free(buffer),fputs("ERROR: error reading map file", stderr),exit(1);

        fclose(fp);

        Map * new_map = LoadMap(buffer);
        AddToList(new_map,maps_loaded);

        free(buffer);



    }
    closedir(dir);

}

Map * LoadMap(char * map_info){

	yajl_val node;
	char errbuf[1024];
	short x,y;
	short i;

	Map * map = (Map *) malloc(sizeof(Map));


	//logger("Parsing packet from JSON message");

	node = yajl_tree_parse(( char *) map_info, errbuf, sizeof(errbuf));

	 if (node == NULL) {
	        fprintf(stderr,"parse_error: ");
	        if (strlen(errbuf)){
	        	fprintf(stderr," %s", errbuf);
	        }else{
	        	fprintf(stderr,"unknown error");
			}
	        fprintf(stderr,"\n");
	        return false;
	    }

	 const char * path[] = { "map_id", ( char *) 0 };

	 map->area_id = YAJL_GET_INTEGER(yajl_tree_get(node, path, yajl_t_number));

	 path[0] = "cell_size";
	 map->cell_size = YAJL_GET_INTEGER(yajl_tree_get(node, path, yajl_t_number));

	 path[0] = "x_cells";
	 map->x_cells = YAJL_GET_INTEGER(yajl_tree_get(node, path, yajl_t_number));

	 path[0] = "y_cells";
	 map->y_cells = YAJL_GET_INTEGER(yajl_tree_get(node, path, yajl_t_number));

	 map->cells = (Cell **) malloc(map->x_cells * sizeof(Cell *));
	 for (x = 0; x < map->x_cells; x++){
		 map->cells[x] = (Cell *) malloc(map->y_cells * sizeof(Cell));
	 }

	 path[0] = "cells";

	 for (x = 0; x < map->x_cells; x++)
		 for (y = 0; y < map->y_cells; y++){

			 map->cells[x][y].prob_location = 1;
			 map->cells[x][y].transition_cell = true;

			 yajl_val cell = YAJL_GET_ARRAY(YAJL_GET_ARRAY((yajl_tree_get(node,path, yajl_t_array)))->values[x])->values[y];

			 if (YAJL_IS_NUMBER(cell)){
				 map->cells[x][y].prob_location = YAJL_GET_INTEGER(YAJL_GET_ARRAY(YAJL_GET_ARRAY((yajl_tree_get(node,path, yajl_t_array)))->values[x])->values[y]);
			 	 map->cells[x][y].transition_cell = false;
			 }else if (YAJL_IS_NULL(cell)){
				 map->cells[x][y].transp.area_id = 0;
				 map->cells[x][y].transp.x_cell = 0;
				 map->cells[x][y].transp.y_cell = 0;
			 }else{
				 for (i = 0; i < YAJL_GET_OBJECT(cell)->len; i++){
					 if (!strcmp(YAJL_GET_OBJECT(cell)->keys[i], "area_id"))
						 map->cells[x][y].transp.area_id = YAJL_GET_INTEGER(YAJL_GET_OBJECT(cell)->values[i]);
					 else if (!strcmp(YAJL_GET_OBJECT(cell)->keys[i], "x_cell"))
						 map->cells[x][y].transp.x_cell = YAJL_GET_INTEGER(YAJL_GET_OBJECT(cell)->values[i]);
					 else if (!strcmp(YAJL_GET_OBJECT(cell)->keys[i], "y_cell"))
						 map->cells[x][y].transp.y_cell = YAJL_GET_INTEGER(YAJL_GET_OBJECT(cell)->values[i]);
				 }
			 }
		 }

	 return map;

}

static bool AdjacentTransition(Map * map, Location * location){

	if (map->cells[location->x][location->y].transition_cell || (location->x + 1 <= map->x_cells && map->cells[location->x + 1][location->y].transition_cell) || (location->x - 1 >= 0 && map->cells[location->x - 1][location->y].transition_cell) || (location->y + 1 <= map->y_cells && map->cells[location->x][location->y + 1].transition_cell) || (location->y - 1 >= 0 && map->cells[location->x][location->y - 1].transition_cell))
		return true;
	
	if (location->y % 2 == 0){
		return (location->x - 1 >= 0 && location->y+1 <= map->y_cells && map->cells[location->x - 1][location->y + 1].transition_cell) || (location->x - 1 >= 0 && location->y - 1 >= 0 && map->cells[location->x - 1][location->y - 1].transition_cell);
	}else{
		return (location->x + 1 <= map->x_cells && location->y - 1 >= 0 && map->cells[location->x + 1][location->y - 1].transition_cell) || (location->x +1 <= map->x_cells && location->y + 1 <= map->y_cells && map->cells[location->x + 1][location->y + 1].transition_cell);
	}
}

bool CheckTransition(LList * maps, Location * previous, Location  * comitted_pos, Location * conflicted_pos, bool prev_conflicted){
	//TODO: Check for transition cells around previous and decide if it should assume the conflicted position

	uint8_t old_area = previous->area_id;
	uint8_t new_area = comitted_pos->area_id;
	uint8_t conf_area = conflicted_pos->area_id;
	Map * map, *old_map, *new_map, *conf_map;
	LElement * elem;
	LList maps_for = *maps;

	if (new_area == conf_area)
		return false;

	if (prev_conflicted)
		return true;

	FOR_EACH(elem, maps_for){
		map = (Map *) elem->data;
		if (map->area_id == old_area)
			old_map = map;
		if (map->area_id == new_area)
			new_map = map;
		if (map->area_id == conf_area)
			conf_map = map;
	}

	if (old_area != conf_area)
		return (AdjacentTransition(old_map, previous) || AdjacentTransition(new_map, comitted_pos)) && AdjacentTransition(conf_map, conflicted_pos);
	else
		return !(AdjacentTransition(old_map, previous) || AdjacentTransition(conf_map, conflicted_pos)) && AdjacentTransition(new_map, comitted_pos);	
	

	return false;
}

void DestroyMap(Map * map){
	short x;
	for (x=0; x < map->x_cells ; x++)
			free(map->cells[x]);
	free(map->cells);
	free(map);
}

void PrintMap(Map * map){
	int i,j;
	printf("map id: %d \n floor: %d cell size: %d\n x_cells: %d\n y_cells: %d\n", map->area_id, map->floor, map->cell_size, map->x_cells, map->y_cells);
	for (i=0; i< map->x_cells; i++){
		for(j=0; j < map->y_cells; j++){
			if (map->cells[i][j].transition_cell)
				printf("2 ");
			else
				printf("%d ",map->cells[i][j].prob_location);
		}
		printf("\n");
	}
}

Location * InfoToCell(Map * map, vec3d * rs1, vec3d * rs2){


	Location * location = (Location *) malloc(sizeof(Location));
	location->area_id = map->area_id;

	uint16_t i,j;

	double x1 = rs1->x/map->cell_size;
	double y1 = rs1->y/map->cell_size;
	double x2 = rs2->x/map->cell_size;
	double y2 = rs2->y/map->cell_size;

	int cell_x1 = (int) x1;
	int cell_y1 = (int) y1;
	int cell_x2 = (int) x2;
	int cell_y2 = (int) y2;
		
	double x1c = x1 - cell_x1;
	double y1c = y1 - cell_y1;
	double x2c = x2 - cell_x2;
	double y2c = y2 - cell_y2;

	if (cell_x1 >= 0 && cell_x1 <= map->x_cells && cell_y1 >= 0 && cell_y1 <= map->y_cells){

		if (map->cells[cell_x1][cell_y1].prob_location == 1){
			location->x = cell_x1;
			location->y = cell_y1;
			return location;
		}else{
			
			if (cell_y1 % 2 == 0){
				//EVEN
				//(i+1, j), (i-1, j), (i, j-1), (i, j+1), (i-1, j-1), (i-1, j+1)

				

				if (x1c <= 0.5 && y1c <= 0.5){
					if (cell_x1 - 1 >= 0 && cell_y1 -1 >= 0 && map->cells[cell_x1 - 1][cell_y1 - 1].prob_location == 1){
						location->x = cell_x1 - 1;
						location->y = cell_y1 - 1;
						return location;
					}
					if (cell_x1 -1 >= 0 && map->cells[cell_x1 - 1][cell_y1].prob_location == 1){
						location->x = cell_x1 - 1;
						location->y = cell_y1;
						return location;
					}
					if (cell_y1 -1 >= 0 && map->cells[cell_x1][cell_y1 - 1].prob_location == 1){
						location->x = cell_x1;
						location->y = cell_y1 - 1;
						return location;
					}
				}
				if (x1c <= 0.5 && y1c >= 0.5){
					if (cell_x1 -1 >= 0 && cell_y1 + 1 <= map->y_cells && map->cells[cell_x1][cell_y1 + 1].prob_location == 1){
						location->x = cell_x1 - 1;
						location->y = cell_y1 + 1;
						return location;
					}
					if (cell_y1 + 1 <= map->y_cells && map->cells[cell_x1][cell_y1 + 1].prob_location == 1){
						location->x = cell_x1;
						location->y = cell_y1 + 1;
						return location;
					}
					if (cell_x1 - 1 >= 0 && map->cells[cell_x1 - 1][cell_y1].prob_location == 1){
						location->x = cell_x1 - 1;
						location->y = cell_y1;
						return location;
					}
				}
				if (x1c >= 0.5 && y1c <= 0.5){
					if (cell_x1 + 1 <= map->x_cells && map->cells[cell_x1 + 1][cell_y1].prob_location == 1){
						location->x = cell_x1 + 1;
						location->y = cell_y1;
						return location;
					}
					if (cell_y1 - 1 >= 0 && map->cells[cell_x1][cell_y1 - 1].prob_location == 1){
						location->x = cell_x1;
						location->y = cell_y1 - 1;
						return location;
					}
				}

				if (x1c >= 0.5 && y1c >= 0.5){
					if (cell_x1 + 1 <= map->x_cells && map->cells[cell_x1 + 1][cell_y1].prob_location == 1){
						location->x = cell_x1 + 1;
						location->y = cell_y1;
						return location;
					}
					if (cell_y1 + 1 <= map->y_cells && map->cells[cell_x1][cell_y1 + 1].prob_location == 1){
						location->x = cell_x1;
						location->y = cell_y1 + 1;
						return location;
					}
				}
			}else{
				//ODD
				//(i+1, j), (i-1, j), (i, j-1), (i, j+1), (i+1, j-1), (i+1, j+1)

				if (x1c <= 0.5 && y1c <= 0.5){
					if (cell_x1 -1 >= 0 && map->cells[cell_x1 - 1][cell_y1].prob_location == 1){
						location->x = cell_x1 - 1;
						location->y = cell_y1;
						return location;
					}
					if (cell_y1 -1 >= 0 && map->cells[cell_x1][cell_y1 - 1].prob_location == 1){
						location->x = cell_x1;
						location->y = cell_y1 - 1;
						return location;
					}
				}
				if (x1c <= 0.5 && y1c >= 0.5){
					if (cell_y1 + 1 <= map->y_cells && map->cells[cell_x1][cell_y1 + 1].prob_location == 1){
						location->x = cell_x1;
						location->y = cell_y1 + 1;
						return location;
					}
					if (cell_x1 - 1 >= 0 && map->cells[cell_x1 - 1][cell_y1].prob_location == 1){
						location->x = cell_x1 - 1;
						location->y = cell_y1;
						return location;
					}
				}
				if (x1c >= 0.5 && y1c <= 0.5){
					if (cell_x1 + 1 <= map->x_cells &&  cell_y1 -1 >= 0 && map->cells[cell_x1 + 1][cell_y1 - 1].prob_location == 1){
						location->x = cell_x1 + 1;
						location->y = cell_y1 - 1;
						return location;
					}
					if (cell_x1 + 1 <= map->x_cells && map->cells[cell_x1 + 1][cell_y1].prob_location == 1){
						location->x = cell_x1 + 1;
						location->y = cell_y1;
						return location;
					}
					if (cell_y1 - 1 >= 0 && map->cells[cell_x1][cell_y1 - 1].prob_location == 1){
						location->x = cell_x1;
						location->y = cell_y1 - 1;
						return location;
					}
				}

				if (x1c >= 0.5 && y1c >= 0.5){
					if (cell_x1 + 1 <= map->x_cells && cell_y1 + 1 <= map->y_cells && map->cells[cell_x1 + 1][cell_y1 + 1].prob_location == 1){
						location->x = cell_x1 + 1;
						location->y = cell_y1 + 1;
						return location;
					}
					if (cell_x1 + 1 <= map->x_cells && map->cells[cell_x1 + 1][cell_y1].prob_location == 1){
						location->x = cell_x1 + 1;
						location->y = cell_y1;
						return location;
					}
					if (cell_y1 + 1 <= map->y_cells && map->cells[cell_x1][cell_y1 + 1].prob_location == 1){
						location->x = cell_x1;
						location->y = cell_y1 + 1;
						return location;
					}
				}


			}
				
		}

		
	}else if (cell_x2 >= 0 && cell_x2 <= map->x_cells && cell_y2 >= 0 && cell_y2 <= map->y_cells){
		if (map->cells[cell_x2][cell_y2].prob_location == 1){
			location->x = cell_x2;
			location->y = cell_y2;
			return location;
		}else{
			if (cell_y2 % 2 == 0){
				//EVEN
				//(i+1, j), (i-1, j), (i, j-1), (i, j+1), (i-1, j-1), (i-1, j+1)

				

				if (x2c <= 0.5 && y2c <= 0.5){
					if (cell_x2 - 1 >= 0 && cell_y2 -1 >= 0 && map->cells[cell_x2 - 1][cell_y2 - 1].prob_location == 1){
						location->x = cell_x2 - 1;
						location->y = cell_y2 - 1;
						return location;
					}
					if (cell_x2 -1 >= 0 && map->cells[cell_x2 - 1][cell_y2].prob_location == 1){
						location->x = cell_x2 - 1;
						location->y = cell_y2;
						return location;
					}
					if (cell_y2 -1 >= 0 && map->cells[cell_x2][cell_y2 - 1].prob_location == 1){
						location->x = cell_x2;
						location->y = cell_y2 - 1;
						return location;
					}
				}
				if (x2c <= 0.5 && y2c >= 0.5){
					if (cell_x2 -1 >= 0 && cell_y2 + 1 <= map->y_cells && map->cells[cell_x2][cell_y2 + 1].prob_location == 1){
						location->x = cell_x2 - 1;
						location->y = cell_y2 + 1;
						return location;
					}
					if (cell_y2 + 1 <= map->y_cells && map->cells[cell_x2][cell_y2 + 1].prob_location == 1){
						location->x = cell_x2;
						location->y = cell_y2 + 1;
						return location;
					}
					if (cell_x2 - 1 >= 0 && map->cells[cell_x2 - 1][cell_y2].prob_location == 1){
						location->x = cell_x2 - 1;
						location->y = cell_y2;
						return location;
					}
				}
				if (x2c >= 0.5 && y2c <= 0.5){
					if (cell_x2 + 1 <= map->x_cells && map->cells[cell_x2 + 1][cell_y2].prob_location == 1){
						location->x = cell_x2 + 1;
						location->y = cell_y2;
						return location;
					}
					if (cell_y2 - 1 >= 0 && map->cells[cell_x2][cell_y2 - 1].prob_location == 1){
						location->x = cell_x2;
						location->y = cell_y2 - 1;
						return location;
					}
				}

				if (x2c >= 0.5 && y2c >= 0.5){
					if (cell_x2 + 1 <= map->x_cells && map->cells[cell_x2 + 1][cell_y2].prob_location == 1){
						location->x = cell_x2 + 1;
						location->y = cell_y2;
						return location;
					}
					if (cell_y2 + 1 <= map->y_cells && map->cells[cell_x2][cell_y2 + 1].prob_location == 1){
						location->x = cell_x2;
						location->y = cell_y2 + 1;
						return location;
					}
				}
			}else{
				//ODD
				//(i+1, j), (i-1, j), (i, j-1), (i, j+1), (i+1, j-1), (i+1, j+1)

				if (x2c <= 0.5 && y2c <= 0.5){
					if (cell_x2 -1 >= 0 && map->cells[cell_x2 - 1][cell_y2].prob_location == 1){
						location->x = cell_x2 - 1;
						location->y = cell_y2;
						return location;
					}
					if (cell_y2 -1 >= 0 && map->cells[cell_x2][cell_y2 - 1].prob_location == 1){
						location->x = cell_x2;
						location->y = cell_y2 - 1;
						return location;
					}
				}
				if (x2c <= 0.5 && y2c >= 0.5){
					if (cell_y2 + 1 <= map->y_cells && map->cells[cell_x2][cell_y2 + 1].prob_location == 1){
						location->x = cell_x2;
						location->y = cell_y2 + 1;
						return location;
					}
					if (cell_x2 - 1 >= 0 && map->cells[cell_x2 - 1][cell_y2].prob_location == 1){
						location->x = cell_x2 - 1;
						location->y = cell_y2;
						return location;
					}
				}
				if (x2c >= 0.5 && y2c <= 0.5){
					if (cell_x2 + 1 <= map->x_cells &&  cell_y2 -1 >= 0 && map->cells[cell_x2 + 1][cell_y2 - 1].prob_location == 1){
						location->x = cell_x2 + 1;
						location->y = cell_y2 - 1;
						return location;
					}
					if (cell_x2 + 1 <= map->x_cells && map->cells[cell_x2 + 1][cell_y2].prob_location == 1){
						location->x = cell_x2 + 1;
						location->y = cell_y2;
						return location;
					}
					if (cell_y2 - 1 >= 0 && map->cells[cell_x2][cell_y2 - 1].prob_location == 1){
						location->x = cell_x2;
						location->y = cell_y2 - 1;
						return location;
					}
				}

				if (x2c >= 0.5 && y2c >= 0.5){
					if (cell_x2 + 1 <= map->x_cells && cell_y2 + 1 <= map->y_cells && map->cells[cell_x2 + 1][cell_y2 + 1].prob_location == 1){
						location->x = cell_x2 + 1;
						location->y = cell_y2 + 1;
						return location;
					}
					if (cell_x2 + 1 <= map->x_cells && map->cells[cell_x2 + 1][cell_y2].prob_location == 1){
						location->x = cell_x2 + 1;
						location->y = cell_y2;
						return location;
					}
					if (cell_y2 + 1 <= map->y_cells && map->cells[cell_x2][cell_y2 + 1].prob_location == 1){
						location->x = cell_x2;
						location->y = cell_y2 + 1;
						return location;
					}
				}


			}
		}

	}



	for (i=0;i < map->x_cells; i++)
		for(j=0; j<map->y_cells; j++)
			if (map->cells[i][j].transition_cell){
				location->x = i;
				location->y = j;
				return location;
			}


	location->x = 0;
	location->y = 0;

	return location;
}
