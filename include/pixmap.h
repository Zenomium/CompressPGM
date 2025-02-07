#ifndef __PIXMAP_H__
#define  __PIXMAP_H__
#include "image_read.h"
#include "heap.h"
/**
 * @author Christophe TARATIBU
 */

/**
 * Crée une pixmap qui va contenir les pixels de l'image
 * @param length_line Longueur du côté de l'image
 * @return pixmap vide
 */
unsigned char** createpixmaptab(int length_line);
/**
 * Remplit la pixmap
 * @param image Contient un tableau de pixels
 * @param pixmap pixmap à remplir
 */
void fillPGM(PGMImage* image, unsigned char** pixmap);
/**
 * Remplit le Quadtree avec les donnés de l'image
 * @param heap Quadtree reconstruit
 * @param pixmap Pixmap vide à remplir
 * @param index Index dans le quadtree
 * @param x Position x dans la pixmap
 * @param y Position y dans la pixmap
 * @param size  Taille initialisée à 0 qui à 1 contiendra un pixel
 * 
 */

void fill_pixmap_from_heap(Heap* heap, unsigned char** pixmap, int index, int x, int y, int size);
/**
 * Reconstruit le fichier pgm à partir de la pixmap remplie
 * @param pixmap Pixmap contenant les pixels
 * @param filename Nom du fichier (Pas oublier l'extension pgm)
 * @param side_size Longeur d'un côté du carré d'une image
 */
void create_pgm_from_pixmap(unsigned char ** pixmap,const char* filename, int side_size);
#endif