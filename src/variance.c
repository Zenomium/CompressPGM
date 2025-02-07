/*
 * Module de gestion de la variance
 * Ce module fournit les fonctions nécessaires pour calculer la variance d'un tas
 */

#include<stdio.h>
#include <stdlib.h>
#include <math.h>
#include "variance.h"



/* 
 * Calcule la variance pour chaque noeud d'un tas en fonction des valeurs des enfants
 * Si le noeud est une feuille ou est marqué comme uniforme, la variance est nulle.
 */
void variance_heap(Heap* heap, int index) {
    if (is_leaf(heap, index) || heap->tab[index].uniform == 1) {
        heap->tab[index].variance = 0;
        return;
    }
    // Calcul récursif pour les enfants
    variance_heap(heap, child(index, 0));
    variance_heap(heap, child(index, 1));
    variance_heap(heap, child(index, 2));
    variance_heap(heap, child(index, 3));
    // Moyennes des enfants
    double mean1 = heap->tab[child(index, 0)].pixel;
    double mean2 = heap->tab[child(index, 1)].pixel;
    double mean3 = heap->tab[child(index, 2)].pixel;
    double mean4 = heap->tab[child(index, 3)].pixel;
    // Moyenne globale
    double total_mean = (mean1 + mean2 + mean3 + mean4) / 4.0;
    // Variances des enfants
    double variance1 = heap->tab[child(index, 0)].variance;
    double variance2 = heap->tab[child(index, 1)].variance;
    double variance3 = heap->tab[child(index, 2)].variance;
    double variance4 = heap->tab[child(index, 3)].variance;

    double mu = variance1 + pow(total_mean - mean1, 2) +
                variance2 + pow(total_mean - mean2, 2) +
                variance3 + pow(total_mean - mean3, 2) +
                variance4 + pow(total_mean - mean4, 2);
    // Variance au noeud courant
    heap->tab[index].variance = sqrt(mu / 4.0);
}

/* Calcule la moyenne des variances dans un tas */
double mean_variance(Heap* heap){
    double mean = 0;
    for(int i = 0; i < heap->capacity; i++){
        mean += heap->tab[i].variance;
    }
    return mean /(double) heap->capacity;
}

/* Trouve la variance maximale dans un tas */
double max_variance(Heap* heap){
    double max = 0;
    for(int j = 0; j < heap->capacity; j++){
        if(max < heap->tab[j].variance){
            max = heap->tab[j].variance;
        }
    } 
    return max;
}

/* Calcule le rapport entre la variance moyenne et 
 * la variance maximale pour obtenir le seuil de départ 
 */
double sigma_variance(Heap* heap){
    double mean = mean_variance(heap);
    double max = max_variance(heap);
    return mean / max;
}