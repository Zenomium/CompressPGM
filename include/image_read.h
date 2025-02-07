#ifndef __IMAGE_READ_H__
#define __IMAGE_READ_H__

/**
 * @author Christophe TARATIBU
 */

// Structure pour définir une Image PGM
typedef struct {
    int width;
    int height;
    int max_gray;
    unsigned char* data;
} PGMImage;

/**
 *  Lit une image PGM
 * @param filename Nom de l'image
 * @return élément de type PGMImage
 */
PGMImage* readPGM(const char* filename);

#endif