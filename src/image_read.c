/*
 * lecture d'images PGM
 * qui permet de lire des fichiers au format PGM (P5) et de les charger en mémoire
 * pour être traité plus tard.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "image_read.h"


/*
 * Fonction de lecture d'un fichier PGM
 * Cette fonction prend en charge uniquement le format P5 (binaire)
 * retourne un pointeur vers une structure PGMImage contenant les données de l'image ou NULL en cas d'erreur
 */
PGMImage* readPGM(const char* filename) {
    // Ouverture du fichier en mode binaire
    FILE* file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Erreur à l'ouverture du fichier %s\n", filename);
        return NULL;
    }
    // Vérification du format (doit être P5)
	char format[3];
    fscanf(file, "%2s", format);
    if (strcmp(format, "P5") != 0) {
        fprintf(stderr, "Format PGM invalide (seul P5 est supporté) !\n");
        exit(EXIT_FAILURE);
    }
    // Allocation de la structure image
    PGMImage* image = (PGMImage*)malloc(sizeof(PGMImage));
    if (!image) {
        fprintf(stderr, "Erreur d'allocation de mémoire\n");
        fclose(file);
        return NULL;
    }
    // Gestion des commentaires (lignes commençant par #)
    int c;
    while ((c = fgetc(file)) != EOF) {
        // Ignore toute la ligne de commentaire
        if (c == '#') {
            while ((c = fgetc(file)) != '\n' && c != EOF);
        } else if (!isspace(c)) {
            ungetc(c, file);
            break;
        }
    }
    // Lecture des dimensions et du niveau de gris maximum
    fscanf(file, "%d %d %d", &image->width, &image->height, &image->max_gray);
    fgetc(file); // Lecture du caractère newline après max_gray
    // Vérification de la validité des dimensions
    if (image->width <= 0 || image->height <= 0 || image->max_gray <= 0) {
        fprintf(stderr, "Dimensions ou niveaux de gris invalides !\n");
        exit(EXIT_FAILURE);
    }
    // Allocation du tableau de pixels
    image->data = (unsigned char*)malloc(image->width * image->height * sizeof(unsigned char));
    if (!image->data) {
        fprintf(stderr, "Erreur d'allocation de mémoire pour les données de l'image\n");
        free(image);
        fclose(file);
        return NULL;
    }
    // Lecture des données de l'image
    size_t pixels_read = fread(image->data, sizeof(unsigned char), image->width * image->height, file);
    if (pixels_read != (size_t)(image->width * image->height)) {
                fprintf(stderr, "Fichier incomplet : données insuffisantes !\n");
            exit(EXIT_FAILURE);
    }
    fclose(file);
    return image;
}





