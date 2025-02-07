/*
 * Module de compression/décompression
 * Implémente les fonctions de manipulation de bits et de compression des données
 */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#ifndef _UCHAR_
	#define _UCHAR_
	#include <limits.h>
	#include <string.h>
	#include <ctype.h>
#endif
#include "compress.h"

/*
 * Écrit nbit bits de src dans le flux de bits curr
 * Gère les cas où les bits sont à cheval sur plusieurs octets
 * puis retourne le nombre de bits écrits
 */

static size_t pushbits(BitStream* curr, uchar src, size_t nbit) {
    size_t written = 0;
    // Masque pour ne garder que les nbit bits de poids faible
    src &= (1 << nbit) - 1;
    while (nbit > 0) {
        if (nbit <= curr->capa) {
            // Cas simple : tous les bits rentrent dans l'octet courant
            *curr->ptr |= src << (curr->capa - nbit);
            curr->capa -= nbit;
            written += nbit;
            nbit = 0;
        } else {
            // Cas complexe : les bits sont répartis sur plusieurs octets
            size_t bits_to_write = curr->capa;
            // Écrit les bits de poids fort dans l'octet courant
            *curr->ptr |= src >> (nbit - bits_to_write);
            written += bits_to_write;
            nbit -= bits_to_write;
            // Passe à l'octet suivant
            curr->ptr++;
            curr->capa = CHAR_BIT;
            // Écrit les bits restants dans le nouvel octet
            *curr->ptr = (src & ((1 << nbit) - 1)) << (CHAR_BIT - nbit);
            curr->capa -= nbit;
            written += nbit;
            nbit = 0;
        }
    }
    return written;
}

/*
 * Lit nbit bits du flux de bits curr et les stocke dans dest
 * Fonction miroir de pushbits pour la décompression
 * retourne le nombre de bits lus
 */
static size_t pullbits(BitStream* curr, uchar* dest, size_t nbit) {
    size_t read = 0;
    *dest = 0;
    while (nbit > 0) {
        if (nbit <= curr->capa) {
            // Cas simple : tous les bits sont dans l'octet courant
            *dest |= (*curr->ptr >> (curr->capa - nbit)) & ((1 << nbit) - 1);
            curr->capa -= nbit;
            read += nbit;
            nbit = 0;
        } else {
            // Cas complexe : les bits sont répartis sur plusieurs octets
            size_t bits_to_read = curr->capa;
            // Lit les bits disponibles dans l'octet courant
            *dest |= (*curr->ptr & ((1 << bits_to_read) - 1)) << (nbit - bits_to_read);
            read += bits_to_read;
            nbit -= bits_to_read;
            // Passe à l'octet suivante
            curr->ptr++;
            curr->capa = CHAR_BIT;
            // Lit les bits restants dans le nouvel octet
            *dest |= (*curr->ptr >> (CHAR_BIT - nbit)) & ((1 << nbit) - 1);
            curr->capa -= nbit;
            read += nbit;
            nbit = 0;
        }
    }
    return read;
}
/*
 * Encode l'arbre quadtree dans un format binaire
 * Pour chaque nœud non-uniforme :
 * - Encode la valeur du pixel (8 bits)
 * - Encode epsilon (2 bits) pour les nœuds non-feuilles
 * - Si epsilon est 0, encode le flag uniform (1 bit)
 * retourne le nombre total de bits encodés
 */
void encode(Heap* src, uchar* dest, const char* output){
	BitStream bitstream = {dest, CHAR_BIT};
	int encoded_bits = 0;
	for(int i = 0; i < src->size; i++){
        // Ne traite pas les nœuds des sous-arbres uniformes
		if((src->tab[father(i)].uniform != 1)){
            // Encode le pixel pour les premiers enfants ou la racine
			if(i%4 != 0 || i == 0){
				encoded_bits +=pushbits(&bitstream,src->tab[i].pixel,8); // encode m
			}
            // Pour les nœuds non-feuilles, encode epsilon et uniform si nécessaire
			if(is_father(src,i)){
				encoded_bits +=pushbits(&bitstream,src->tab[i].epsilon,2); //encode epsilon
				if(src->tab[i].epsilon == 0){
					   encoded_bits +=pushbits(&bitstream,src->tab[i].uniform,1); //encode u
				}
			}
		} 
    }
    // Complète le dernier octet si nécessaire
	encoded_bits = encoded_bits + encoded_bits % 8;
    // Crée le fichier QTC final
	create_QTC(output,src,encoded_bits,dest);
	free(dest);

}

/*
 * Calcule la valeur d'un pixel par interpolation à partir de son père et de ses frères
 * Utilisé lors du décodage pour reconstruire les pixels non stockés
 * retourne la valeur du pixel interpolé
 */
static uchar interpolation(Heap* dest, int index){
	uchar pixel_father = dest->tab[father(index)].pixel;
	int error_father =  dest->tab[father(index)].epsilon;
	uchar sum_pixel = 0;
    // Somme les valeurs des frères déjà décodés
	for(int i = 0; i < 3; i ++){
		sum_pixel += dest->tab[child(father(index),i)].pixel;
	}
     // Formule d'interpolation : p4 = 4*p0 + e - (p1 + p2 + p3)
	return (4 * pixel_father + error_father) - sum_pixel;

}

/*
 * Décode un fichier QTC et reconstruit l'arbre quaternaire
 * retourne l'arbre quadtree reconstruit ou NULL en cas d'erreur
 */
Heap* decode(const char* file_encoded){
	uchar n;
    // Vérifie l'en-tête du fichier et récupère le niveau de décomposition
	FILE * ptr_file = check_QTCfile_header(file_encoded, &n);
    // Initialisation de l'arbre
	Heap* dest = malloc(sizeof(Heap));
	dest->capacity = sumPower4(n);
	dest->tab = (Node*)calloc(dest->capacity,sizeof(Node));
	dest->size = 0;
	dest->nb_leaves = power(power(2,n),2);
	if(!ptr_file){
        free(dest);
		return NULL;
    }
    // Lecture des données encodées
    long current = ftell(ptr_file);
	fseek(ptr_file, 0, SEEK_END);
	long data_size = ftell(ptr_file) - current;
    fseek(ptr_file,current,SEEK_SET);
	uchar* encoded = (uchar*)malloc(sizeof(uchar)*data_size);
	if (fread(encoded, sizeof(uchar), data_size, ptr_file) != data_size) {
        fprintf(stderr, "Erreur de lecture des données encodées\n");
        free(dest);
        free(encoded);
        fclose(ptr_file);
        return NULL;
    }
    fclose(ptr_file);
    // Décodage bit par bit
	BitStream bitstream = {encoded,CHAR_BIT};
    for(int i = 0; i < dest->capacity; i++){
        if(is_father(dest,i)){
            // Traitement de la racine
            if(i == 0){
                pullbits(&bitstream,&dest->tab[i].pixel,8);
				pullbits(&bitstream,(uchar*)&dest->tab[i].epsilon,2);
                dest->tab[i].uniform = 0;
                dest->tab[i].variance = 0;
				if(dest->tab[i].epsilon == 0){
				   pullbits(&bitstream,(uchar*)&dest->tab[i].uniform,1);
                    if(dest->tab[i].uniform == 1){
                        subtree_route_uniform(dest,i,dest->tab[i].pixel);
                        return dest;
                    }
				}
            // Traitement des autres nœuds non-uniformes
            } else if(i % 4 != 0){
               if(dest->tab[i].uniform != 1){
                    pullbits(&bitstream,&dest->tab[i].pixel,8);
				    pullbits(&bitstream,(uchar*)&dest->tab[i].epsilon,2);
                    dest->tab[i].uniform = 0;
                    dest->tab[i].variance = 0;
                    if(dest->tab[i].epsilon == 0){
                        pullbits(&bitstream,(uchar*)&dest->tab[i].uniform,1);
                        if(dest->tab[i].uniform == 1){
                            subtree_route_uniform(dest,i,dest->tab[i].pixel);
                        }
                    }
               }
            // Traitement des nœuds à interpoler
            } else {
                if(dest->tab[i].uniform != 1){
                    dest->tab[i].pixel = interpolation(dest,i);
                    pullbits(&bitstream,(uchar*)&dest->tab[i].epsilon,2);
                    dest->tab[i].variance = 0;
                    if(dest->tab[i].epsilon == 0){
                        pullbits(&bitstream,(uchar*)&dest->tab[i].uniform,1);
                        if(dest->tab[i].uniform == 1){
                            subtree_route_uniform(dest,i,dest->tab[i].pixel);
                        }
                    }
                }
            }
        // Traitement des feuilles
        } else {
            if(dest->tab[father(i)].uniform == 0){
				if(i % 4 != 0){
				  pullbits(&bitstream,&dest->tab[i].pixel,8);
				  dest->tab[i].epsilon = 0;
				  dest->tab[i].uniform = 1;
                  dest->tab[i].variance = 0;
				} else {
					dest->tab[i].pixel = interpolation(dest,i);
					dest->tab[i].epsilon = 0;
			    	dest->tab[i].uniform = 1;
                    dest->tab[i].variance = 0;
				}
			}
        }
        dest->size++;
    }
	return dest;
}
/*
 * Applique un filtrage sur l'arbre pour optimiser la compression
 * Unifie les sous-arbres dont la variance est inférieure au seuil alpha
 * retourne 1 si le sous-arbre est unifié, 0 sinon
 */
int filtrage(Heap* heap, int index, double sigma, double alpha) {
    // Cas de base : nœud déjà uniforme ou feuille
    if (heap->tab[index].uniform == 1 || is_leaf(heap, index)) {
        return 1;
    }
    // Filtrage récursif des sous-arbres
    int s = 0;
    s += filtrage(heap, child(index,0), alpha * sigma, alpha);
    s += filtrage(heap, child(index,1), alpha * sigma, alpha);
    s += filtrage(heap, child(index,2), alpha * sigma, alpha);
    s += filtrage(heap, child(index,3), alpha * sigma, alpha);
    // Unifie le sous-arbre si tous les enfants sont unifiés et la variance est faible
    if (s < 4 || heap->tab[index].variance > sigma) {
        return 0;
    }
    heap->tab[index].epsilon = 0;
    heap->tab[index].uniform = 1;
    return 1;
}
