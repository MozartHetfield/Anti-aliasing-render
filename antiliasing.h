#ifndef HOMEWORK_H
#define HOMEWORK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

typedef struct {
	unsigned char r, g, b;
} colored_image;

typedef struct {
	unsigned char pixel;
} colorless_image;

typedef struct {
	int color;
	colored_image** colored;
	colorless_image** colorless;
}image;

unsigned char odd_resizer(int from_i, int from_j, int to_i, int to_j, unsigned char pixel, image *img);

unsigned char even_resizer(int from_i, int from_j, int to_i, int to_j, unsigned char pixel, image *img);

void readInput(const char * fileName, image *img);

void writeData(const char * fileName, image *img);

void resize(image *in, image * out);

#endif /* HOMEWORK_H */