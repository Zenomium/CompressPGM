/*
 * Implémentation des fonctions de gestion de l'arbre quadtree
 * L'arbre est stocké sous forme de tableau pour un accès rapide aux noeuds
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "heap.h"

/* Utilisé pour déterminer le nombre de niveaux dans l'arbre */
int logBase2(int number){
   return (int)log2(number);
}

int power(int number, int iter){
    if (iter == 0){
        return 1;
    } else {
        return number * power(number, iter - 1);
    }
}
/* Utilisé pour déterminer la taille nécessaire du tableau */
int sumPower4(int end){
    if(end == 0){
        return power(4,0);
    }
    int result = 0;
    for(int i = 0; i <= end; i++){
       result += power(4,i);
    }
    return result;
}


/* Initialise un nouvel arbre quadtree avec une capacité size
 * retourne un pointeur vers le nouvel arbre ou NULL en cas d'erreur 
 */
Heap* initHeap(int size){
	Heap* newHeap = malloc(sizeof(Heap));
	newHeap->tab = (Node*)malloc(sizeof(Node)*size);
	if(!newHeap->tab){
		fprintf(stderr, "Erreur d'allocation de mémoire");
		return newHeap;
	} else if(size > MAX_SIZE){
		fprintf(stderr, "Erreur : taille trop grande");
		free(newHeap->tab);
		return newHeap;
	}
	newHeap->size = 0;
    newHeap->capacity = size;
	return newHeap;

}

/*
 * Calcule le nombre de feuilles nécessaire pour une image
 * retourne le Log2 de la largeur de l'image
 */
int nb_leaves(PGMImage* image){
    return logBase2(image->width);
}

/*
 * Vérifie si un noeud a au moins un enfant valide
 * retourne true si le noeud est un père, false sinon
 */
bool is_father(Heap *src, int index) {
    if (index < 0 || index >= src->capacity) {
        return false;
    }

    for (int i = 0; i < 4; i++) {
        int childIndex = child(index, i);
        if (childIndex < src->capacity) {
            return true;
        }
    }
    return false;
}

/*
 * Vérifie si un noeud n'a pas d'enfants
 * retourne true si le noeud est une feuille, false sinon
 */
bool is_leaf(Heap* heap, int index) {
    return (child(index, 0) >= heap->size);
}

/*
 * Vérifie si les quatre enfants d'un noeud ont la même valeur
 * et sont tous uniformes pour détecter les zones pouvant être fusionnées
 * retourne true si les quadruplets sont identiques et uniformes, false sinon
 */
bool quadruplets(Heap* heap, int fatherIndex) { 
    int same = 1; // Compte les pixels identiques
    int sameU = 1; // Compte les noeuds uniformes
    int firstChildPixel = heap->tab[child(fatherIndex, 0)].pixel; 
    for (int i = 1; i < 4; i++) { 
        int indexOfChild = child(fatherIndex, i); 
        if (firstChildPixel == heap->tab[indexOfChild].pixel){ 
            same++; 
        } 
        if(heap->tab[indexOfChild].uniform) sameU++;
    } 
    return (same == 4 && sameU == 4)? true: false; }

/*
 * Construit l'arbre quadtree à partir de la matrice de pixels 
 * en subdivisant l'image de manière récursive
 */
void fill_heap(Heap* heap, unsigned char** pixmap, int pos_x, int pos_y, int size, int index, int img_width) {
    // Cas de base : bloc 1x1
    if (size == 1) {
        heap->tab[index].pixel = pixmap[pos_x][pos_y];
        heap->tab[index].epsilon = 0;
        heap->tab[index].uniform = 1;
        heap->tab[index].variance = 0;
        heap->size++;
        return;
    }
    // Subdivision récursive en quatre sous-blocs
    int middle = size / 2;
    fill_heap(heap, pixmap, pos_x, pos_y, middle, child(index, 0), img_width);                    // Nord-Ouest
    fill_heap(heap, pixmap, pos_x, pos_y + middle, middle, child(index, 1), img_width);           // Nord-Est
    fill_heap(heap, pixmap, pos_x + middle, pos_y + middle, middle, child(index, 2), img_width);  // Sud-Est
    fill_heap(heap, pixmap, pos_x + middle, pos_y, middle, child(index, 3), img_width);           // Sud-Ouest
    // Calcul de la moyenne et de l'erreur (epsilon) pour le nœud courant
    int sum_pixel = 0;
    for (int i = 0; i < 4; i++) {
        sum_pixel += heap->tab[child(index, i)].pixel;
    }
    heap->tab[index].pixel = sum_pixel / 4;     // Moyenne des enfants
    heap->tab[index].epsilon = sum_pixel % 4;    // Reste de la division (erreur)
    heap->tab[index].variance = 0;
    // Vérifie si le bloc peut être unifié
    if (quadruplets(heap,index)){
        heap->tab[index].uniform = 1;
    } else {
       heap->tab[index].uniform = 0;
    }
    heap->size++;

    
}

/*
 * Propage une valeur uniforme dans tout un sous-arbre
 */
void subtree_route_uniform(Heap* heap, int index, unsigned char pixel){
    if(index >= heap->capacity){
        return;
    }
    heap->tab[index].pixel = pixel;
    heap->tab[index].epsilon = 0;
    heap->tab[index].uniform = 1;
    heap->tab[index].variance = 0;
    for(int i = 0; i < 4; i++){
        subtree_route_uniform(heap,child(index,i),pixel);
    }
}  


