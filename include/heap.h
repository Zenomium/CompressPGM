#ifndef __HEAP_H__
#define __HEAP_H__
#include "image_read.h"
#include "stdbool.h"
#define MAX_SIZE 5592405 // Taille maximale
/**
 * @author Christophe TARATIBU
 */

// Calcul de l'indice du père dans le  Quadtree
#define father(index) (((index) - 1) / 4)

// Calcul de l'indice de l'enfant d'un noeud dans le Quadtree
#define child(index, n) ((4 * (index)) + (n) + 1)

/**
 *  Strucuture d'un noeud du Quadtree
 */ 
typedef struct node {
	unsigned char pixel; // pixel de l'image ou moyenne intensité gris
	int epsilon; // marge d'erreur (moyenne)
	int uniform; // bit d'uniformité 
	double variance; // Variance du noeud
} Node, *Quadtree;

/**
 * Strucuture d'un tas d'ordre 4 (Quadtree)
 */
typedef struct heap4 {
	Quadtree tab; // Tableau
	int size; // Taille du tableau 
	int capacity; //
	int nb_leaves;
} Heap;

/**
 * Initiailialise le tas d'ordre
 * @param size taille du Quadtree
 * @return Tas initialisé
 */
Heap* initHeap(int size);

/**
 * Remplit le Quadtree avec les donnés de l'image
 * @param heap Quadtree (à remplir)
 * @param pixmap Grille contenant les pixels de l'image
 * @param pos_x Position x dans la pixmap
 * @param pos_y Position y dans la pixmap
 * @param size  Taille initialisée à 0 qui à 1 contiendra un pixel
 * @param img_width Taille d'un côteé de l'image (carré)
 * 
 */
void fill_heap(Heap* heap, unsigned char** pixmap, int pos_x, int pos_y,int size,int index,int img_width);
/**
 * Fait la somme des puissances de 4
 * @param end Nombre de puissance de 4 à sommer
 * @return Résultat de ce calcul
 */
int sumPower4(int end);
/**
 * Vérifie si un noeud du quadtree est 
 * uniforme en regardant ses fils
 * @param heap Quadtree
 * @param fatherIndex Indice du père
 * @return booléen pour vérifier l'uniformité
 */
bool quadruplets(Heap* heap, int fatherIndex);

/**
 * Calcule la puissance d'un nombre
 * @param number Nombre
 * @param iter exposant
 * @return Résultat de la puissance
 */
int power(int number, int iter);
/**
 * Calcule le logarithme en base 2 d'un nombre
 * @param number Nombre
 * @return Résultat du calcul
 */
int logBase2(int number);
/**
 * Cette fonction uniformise les neouds si leur antécédant et uniforme
 * @param heap Quadtree
 * @param index Index dans le quadtree
 * @param pixel Moyenne (pixel a attribuer au déscendants d'un noeud)
 */
void subtree_route_uniform(Heap* heap, int index, unsigned char pixel);

/**
 * Vérifie si un noeud est une feuille
 * @param heap Quadtree
 * @param index Index 
 * @return booléen 
 */
bool is_leaf(Heap* heap, int index);
/**
 * Vérifie si un noeud est un père
 * @param heap Quadtree
 * @param index Index 
 * @return booléen 
 */
bool is_father(Heap *src, int index);

#endif