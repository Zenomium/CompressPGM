/*
 * Programme principal - Compression/décompression d'images en niveaux de gris
 * Utilise un arbre quadtree pour la compression
 */

#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "image_read.h"
#include "pixmap.h"
#include "heap.h"
#include "compress.h"
#include "variance.h"

/* 
 * Affiche l'aide du programme avec les options disponibles
 * argv[0] : nom du programme
 */

void help(char* argv){
	fprintf(stdout,"usage : %s [OPTIONS] : \n", argv);
	fprintf(stdout,"-c :  encodeur\n");
    fprintf(stdout,"-u : décodeur\n");
    fprintf(stdout, "-i: Nom du fichier d'entrée à préciser \n");
	fprintf(stdout, "-o: Nom du fichier de sortie à préciser \n");
	fprintf(stdout, "-v: Mode bavard \n");
	fprintf(stdout, "-h: aide \n");
    fprintf(stdout,"Les autres options seront ignorés\n");
	fprintf(stdout, "\n");
}

int main(int argc, char *argv[])
{
	int opt;
	char* output_file = NULL;
	char* input_file = NULL;
    int encodes = 0;            // Flag pour mode encodage
    int decodes = 0;            // Flag pour mode décodage
    double alpha = 1;           // Paramètre de filtrage
    int verbose_mode = 0;       // Mode verbeux
    int grid = 0;               // Option pour la grille de segmentation

	// Analyse des options de la ligne de commande
	while((opt = getopt(argc,argv,"cu:i:o:hva:")) != -1){
		switch (opt){
			case 'c':
				encodes = 1;
				break;
			case 'u':
			   decodes = 1;
			   input_file = optarg;
			   break;
			case 'i':
			     // Vérifie si l'argument n'est pas une autre option
				 if (optarg[0] == '-') {
                    optind--;
                    input_file = NULL;
                } else {
                    input_file = optarg;
                }
                break;
			case 'o':
			    if (optarg[0] == '-') {
                    optind--;
                    output_file = NULL;
                } else {
                    output_file = optarg;
                }
                break;
		    case 'a':
			   alpha = atof(optarg);
			   break;
			case 'h':
			  help(argv[0]);
			  return 0;
			case 'v':
			verbose_mode = 1;
			break;
			default:
				fprintf(stderr,"Erreur : Argument inconnu. %c\n", opt);
				fprintf(stderr,"\n");
				help(argv[0]);
				break;
		}
	
	}
	// Affichage des informations en mode verbeux
	if(verbose_mode){
		if(encodes) {
			printf("encodage activé\n");
		} else {
			printf("encodage désactivé\n");
		}
				if(decodes) {
			printf("decodage activé\n");
		} else {
			printf("decodage désactive\n");
		}
		if(input_file) {
			printf("source : %s\n", input_file);
		}
		if(output_file) {
			printf("sortie : %s\n", output_file);
		}
		if(grid) {
			printf("grid on\n");
		}else {
			printf("grid off\n");
		}
	}
	// Vérification des paramètres obligatoires
	if(!encodes && !decodes){
			fprintf(stderr, "Erreur ! \n");
			return -1;
	}
    if(encodes && decodes){
		fprintf(stderr, "Erreur : encodage et décodage impossible! \n");
		return -1;
	}
	if(!input_file){
		fprintf(stderr, "Fichier d'entrée a spécifier ! \n");
		return -1;
	}
	if(!output_file){
		if(encodes){
			output_file ="out.qtc";
		} else {
			output_file = "out.pgm";
		}
	}
	// Définition des noms de fichiers par défaut si non spécifiés
	if(encodes){
		// Lecture de l'image PGM
		PGMImage* image = readPGM(input_file);
		if(!image) return 1;
	  	int val = logBase2(image->height);
		// Initialisation de l'arbre quadtree
      	Heap *heap = initHeap(sumPower4(val));
      	heap->nb_leaves = power(2,val) * power(2,val);
		// Création et remplissage de la matrice de pixels
	  	unsigned char** pixmap = createpixmaptab(image->height);
	  	fillPGM(image, pixmap);
		// Construction de l'arbre et application du filtrage
	  	fill_heap(heap,pixmap,0,0,image->width,0,image->width);
		variance_heap(heap,0);
		filtrage(heap,0,sigma_variance(heap),alpha);
		// Encodage et écriture du fichier compressé
	  	uchar* encoded = calloc(50000000,sizeof(uchar));
      	encode(heap,encoded,output_file);
		// Libération de la mémoire
	  	for (int i = 0; i < image->height; i++) {
    		free(pixmap[i]);
        }
		free(image->data);
      	free(image);
		free(pixmap);
		free(heap->tab);
		free(heap);
	  

	} 
	// Mode décodage
	if(decodes){
		// Décodage du fichier QTC
    	Heap *dest = decode(input_file);
		if(!dest){
			return -1;
		}
		// Reconstruction de l'image
	    int img_side = (int)sqrt(dest->nb_leaves);
	    unsigned char** pixmap2 = createpixmaptab(img_side);
	    fill_pixmap_from_heap(dest,pixmap2,0,0,0,img_side);
		// Sauvegarde des résultats
		create_pgm_from_pixmap(pixmap2,output_file,img_side);
		// Libération de la mémoire
	    for (int i = 0; i < img_side; i++) {
    		free(pixmap2[i]);
        }
		free(pixmap2);
		free(dest->tab);
		free(dest);


	}
		
	return 0;
}