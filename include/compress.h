#ifndef __COMPRESS_H__
#define __COMPRESS_H__
#include "heap.h"
#include "qtcmanip.h"
/**
 * @author Christophe TARATIBU
 */

/*Alias pour le type unsigned char*/
typedef unsigned char uchar;

/**
 * Structure bitStream utilisé pour récupérer ou ou écrire
 * des données binaires dans un buffer d'encodage.
 */
typedef struct { 
  uchar* ptr; 
  size_t capa; 
} BitStream;

/** 
 * Encode les données du quadtree dans un buffer d'encodage
 * @param src Tas d'ordre 4 représentant le Quadtree
 * @param dest Buffer d'encodage contenant les données compressées du tas
 * @param output Fichier de sortie ou seront stockées données du Quadtree (out.qtc par défaut)
*/
void encode(Heap* src, uchar* dest, const char* output);
/**
 * Décode un fichier .qtc contenant les données encodées
 * et reconstruit le Quadtree d'origine
 * @param file_encoded Fichier qtc contenant les données encodées
 * @return Tas d'ordre 4 reconstruit
 */
Heap *decode(const char* file_encoded);
/**
 * Initialise un filtrage de l'image
 * @param heap Quadtree contenant les données de l'image
 * @param index Index dans le Quadtree (représenté par un tableau)
 * @param sigma Seuil du filtrage
 * @param alpha Paramètre d'evolution du seuil du filtrage
 * @return 0 ou 1 si le noeud est uniformisable
 */
int filtrage(Heap* heap,int index,double sigma, double alpha);

#endif