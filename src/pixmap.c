/*
 * Module de gestion des pixmaps
 * Ce module fournit les fonctions nécessaires pour manipuler les pixmaps
 * et effectuer les conversions entre différents formats
 */

#include<stdio.h>
#include<stdlib.h>
#include "pixmap.h"


/*
 * Crée un tableau 2D pour stocker les pixels d'une image
 * Alloue dynamiquement un tableau carré de taille taille_ligne x taille_ligne
 * retourne la pixmap allouée dynamiquement, NULL en cas d'échec
 */
unsigned char** createpixmaptab(int taille_ligne) {
    // Allocation du tableau de pointeurs
    unsigned char** tab = (unsigned char**)malloc(sizeof(unsigned char*) * taille_ligne);
    if (!tab) {
        return NULL;
    }
    // Allocation des lignes du tableau
    for (int i = 0; i < taille_ligne; i++) {
        tab[i] = (unsigned char*)malloc(sizeof(unsigned char) * taille_ligne);
    }
    return tab;
}

/*
 * Convertit une image PGM en pixmap
 * Réorganise les données linéaires de l'image en tableau 2D
 */
void fillPGM(PGMImage* image, unsigned char** pixmap) {
    for (int i = 0; i < image->height; i++) {
        for (int j = 0; j < image->width; j++) {
            // Conversion de l'indice 1D en indices 2D
            pixmap[i][j] = image->data[i * image->width + j];
        }
    }
}

/*
 * Sauvegarde les valeurs de gris d'une pixmap dans un fichier texte
 * Utile pour le débogage et la visualisation des valeurs
 */
void saveGrayValuesToFile(int img_size,unsigned char** pixmap, const char* output_filename) {
    FILE* file = fopen(output_filename, "w");
    if (!file) {
        fprintf(stderr, "Erreur à l'ouverture du fichier de sortie.\n");
        exit(EXIT_FAILURE);
    }
    // Affichage des valeurs de pixels
    for (int i = 0; i < img_size; i++) {
        for (int j = 0; j < img_size; j++) {
            fprintf(file, "%3d ", pixmap[i][j]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

/*
 * Remplit une pixmap à partir d'un tas
 * Reconstruction récursive de l'image en utilisant la structure quadtree
 */
void fill_pixmap_from_heap(Heap* heap, unsigned char** pixmap,int pos_x, int pos_y,int index,int size) {
    if (index >= heap->size) {
        return;
    }
    // Si le nœud est uniforme, remplit tout le bloc avec la même valeur
    if (heap->tab[index].uniform == 1) {
        for (int i = pos_x; i < pos_x + size; i++) {
            for (int j = pos_y; j < pos_y + size; j++) {
                pixmap[i][j] = heap->tab[index].pixel;
            }
        }
    } else {
        // Sinon, divise le bloc en quatre et traite récursivement
        int middle = size / 2;
        fill_pixmap_from_heap(heap,pixmap,pos_x,pos_y,child(index,0),middle);
        fill_pixmap_from_heap(heap,pixmap,pos_x,pos_y + middle,child(index,1),middle);
        fill_pixmap_from_heap(heap,pixmap,pos_x + middle,pos_y + middle,child(index,2),middle);
        fill_pixmap_from_heap(heap,pixmap,pos_x + middle,pos_y,child(index,3),middle);
    }
}

/*
 * Crée un fichier PGM à partir d'une pixmap
 * Convertit le tableau 2D en format PGM binaire (P5)
 */
void create_pgm_from_pixmap(unsigned char ** pixmap,const char* filename, int side_size){
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Erreur lors de l'ouverture du fichier");
        return;
    }
    // Écriture de l'en-tête PGM
    fprintf(file, "P5\n%d %d\n255\n",side_size,side_size);
    // Écriture des données binaires
    for (int i = 0; i < side_size; i++) {
        fwrite(pixmap[i], sizeof(unsigned char), side_size, file);
    }
    fclose(file);
    printf("Fichier PGM généré avec succès : %s\n",filename);
}
