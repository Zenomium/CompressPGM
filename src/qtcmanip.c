/*
 * Module de gestion des fichiers compressés qtc
 * Ce module fournit les fonctions nécessaires pour créer et lire un fichier qtc
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include "qtcmanip.h"


/* Crée un fichier compressé au format QTC en incluant des données */
void create_QTC(const char* filename,Heap* heap,int encoded_bits, uchar* encoded_tab){
    uchar length = logBase2((int)sqrt((double)heap->nb_leaves)); // Calcul de la profondeur maximale du tas compressé
    if (!filename) {
		fprintf(stderr, "Erreur nom de fichier nul");
	}
    
	FILE * file = fopen(filename,"wb");
    int real_size = encoded_bits / 8; // Taille réelle en octets après compression
	fprintf(file,"Q1\n");
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    fprintf(file, "# %s", asctime(&tm)); // Ajout d'un horodatage lisible par l'utilisateur
	fprintf(file,"# compression rate %.2f%%\n",(100.0 * encoded_bits / (heap->nb_leaves *CHAR_BIT))); // Taux de compression
	fwrite(&length,sizeof(uchar),1,file); // Écriture de la profondeur du tas
	fwrite(encoded_tab,sizeof(uchar),real_size,file); // Écriture des données compressées
	fclose(file);
}

/*
 * Vérifie le format et la validité d'un fichier QTC
 * retourne un pointeur vers le fichier si tout est valide, sinon NULL.
 */
FILE* check_QTCfile_header(const char* filename, uchar* nb_level) {
    FILE* file = fopen(filename, "rb"); // Ouverture en mode lecture binaire
    if (!file) {
        fprintf(stderr, "Erreur : fichier QTC inexistant\n");
        return NULL;
    }
    char format[4]; 
    fgets(format,4,file); // Lecture du format du fichier
    if (strcmp(format, "Q1\n") != 0) {
        fprintf(stderr, "Format invalide (seul Q1 est supporté) !\n");
        fclose(file);
        return NULL;
    }
    char ligne[2048] ="";
    char c;
    while ((c = fgetc(file)) == '#'){ // Lecture et saut des lignes de commentaires
        fgets(ligne,2048,file);
    }
    ungetc(c,file); // Repositionnement pour continuer la lecture
    fread(nb_level, sizeof(uchar), 1, file); // Extraction du nombre de niveaux
    return file;
}



