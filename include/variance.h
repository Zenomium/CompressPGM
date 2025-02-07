#ifndef __VARIANCE_H__
#define __VARIANCE_H__
#include "heap.h"
/**
 * @author Christophe TARATIBU
 */

/**
 * Calcule la valeur sigma
 * @param heap Quadtree
 * @return Valeur sigma
 */
double sigma_variance(Heap* heap);
/**
 * Calcule la variance de chaque noeud
 * @param heap Quadtree
 * @param index Index dans le quadtree
 */
void variance_heap(Heap* heap, int index);
#endif