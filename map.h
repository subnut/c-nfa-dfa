#include "common.h"
#include <stddef.h>	// for size_t

/*
 * TODO:
 * Convert the map to something more efficient and effective,
 * like red-black trees, for example. Or maybe even a doubly linked list.
 */

#ifndef MAP_H
#define MAP_H

/*
 * WHY?
 *
 * Because, according to C99, unsigned char is the only integer type that is
 * guaranteed to have no padding whatsoever; also, its object representation
 * is the exact same as its pure binary representation.
 *
 * Also, since our hash will most probably be larger than an unsigned char, we
 * store the hash as an unsigned char array. Since the whole array is guaranteed
 * to be contiguous, we can increase the length of our hash as much as we want.
 */
typedef unsigned char * MapHash;

/*
 * WHY?
 * data is a void * so that we can make it point to anything we need.
 *
 * BUT!
 * yes, void * is dangerous, I know. It's generally regarded as dangerous
 * because by using it, you loose all type-safety automatically.
 *
 * But in this particular case, we trust the caller to use the pointer properly.
 * i.e. by casting it to the same type that they assigned it from
 *
 * P.S.
 * In this struct, only hash and data are permanent.
 * There may be additional fields depending on the datastructure being used.
 */
typedef struct {
	MapHash hash;
	void *data;
} MapElem;

/*
 * In this struct, only hashlen is permanent.
 * The rest may change dependending on whatever datastructure is being used.
 */
typedef struct {
	size_t hashlen;
	size_t arrlen;
	MapElem *arr;
} Map;

/*
 * Adds a new entry to the map corresponding to the given MapHash.
 * Returns the pointer to the newly-created MapElem (if successful).
 *
 * N.B. Doesn't check if the MapElem corresponding to the given MapHash
 * already exists.
 */
MapElem *mapAdd(Map *, MapHash);

/*
 * Gets the pointer to the MapElem corresponding to the given MapHash.
 * Returns NULL if said MapElem doesn't exist.
 */
MapElem *mapGet(Map *, MapHash);


Map *newMap(size_t);
void freeMap(Map *);	// frees everything recursively.

#endif /* MAP_H */
