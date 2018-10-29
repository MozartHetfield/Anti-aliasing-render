#ifndef HOMEWORK_H1
#define HOMEWORK_H1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>

typedef struct {
	unsigned char** final_pixel;
}image;

void color(unsigned char** matrix, int i_aux, int j_aux);
void initialize(image *im);
void render(image *im);
void writeData(const char * fileName, image *img);

#endif /* HOMEWORK_H1 */