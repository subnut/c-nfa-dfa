#include "common.h"
#include "macros.h"
#include "map.h"

/*
 * Currently we use a very basic and naive approach
 * comprising of arrays.
 */

#include <string.h>

/*
 * string.h	- memcmp, memcpy, NULL
 */

Map *newMap(size_t hashlen) {
	new(map, Map);
	map->hashlen = hashlen;
	return map;
}

MapElem *mapGet(Map *map, const MapHash *hash) {
	for (size_t i = 0; i < map->arrlen; i++)
		if (0 == memcmp(hash, map->arr[i].hash, map->hashlen))
			return &map->arr[i];
	return NULL;
}

void freeMap(Map *map) {
	for (size_t i = 0; i < map->arrlen; i++) {
		free(map->arr[i].hash);
		free(map->arr[i].data);
	}
	free(map->arr);
	free(map);
}

MapElem *mapAdd(Map *map, const MapHash *hash) {
	// Add new MapElem
	add(map->arrlen, 1);
	reallocarr(map->arr, map->arrlen);

	// Initialize said MapElem
	MapElem *elem = &map->arr[map->arrlen - 1];
	elem->hash = elem->data = NULL;

	// Set MapElem.hash to given hash
	reallocarr(elem->hash, map->hashlen);
	memcpy(elem->hash, hash, map->hashlen);

	// Return pointer to the MapElem
	return elem;
}
