#ifndef __QTCMANIP_H__
#define __QTCMANIP_H__
#include "heap.h"
/**
 * @author Christophe TARATIBU
 */

//Alias redéfinissan unsigned char
typedef unsigned char uchar;

/**
 * Crée le fichier qtc
 * @param filename Nom du fichier qtc
 * @param heap quadtree
 * @param encoded_bits Bits encodés
 * @param encoded_tab Buffer contenant les donnés encodées
 */
void create_QTC(const char* filename,Heap* heap,int encoded_bits, uchar* encoded_tab);
/**
 * Vérifie l'en tête du fichier
 * @param filename Nom du fichier qtc
 * @param nb_level Nombre de niveaux
 * @return pointeur FILE
 */
FILE* check_QTCfile_header(const char* filename, uchar* nb_level);


#endif
