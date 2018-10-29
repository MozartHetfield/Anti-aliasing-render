#include "micro_render.h"

int num_threads, resolution;
int thread_steps;

image* global_img = NULL;

unsigned char get_color(int j, int i) {

	float x = (j + 0.5) * 100 / resolution; //scaled to centimeters
	float y = (i + 0.5) * 100 / resolution; //scaled to centimeters

	if (abs(-x + 2 * y) / sqrt(5) <= 3) return 0; //black
	return 255; //white
}

void initialize(image *im) {

	im->final_pixel = malloc(resolution * sizeof(unsigned char*));
	for (int i = 0; i < resolution; i++) {
		im->final_pixel[i] = malloc(resolution * sizeof(unsigned char));
	}

	global_img = malloc(sizeof(image));
	global_img->final_pixel = malloc(resolution * sizeof(unsigned char*));
	for (int i = 0; i < resolution; i++) {
		global_img->final_pixel[i] = malloc(resolution * sizeof(unsigned char));
	}
}

void* thread_function(void* var) {

	int thread_id = *(int*)var;

	int from_i = resolution - 1 - thread_id * thread_steps;
	int to_i;
	if (thread_id == num_threads - 1) {
		to_i = 0;
	} else {
		to_i = resolution - (thread_id + 1) * thread_steps;
	}
	for (int i = from_i; i >= to_i; i--) {
		for (int j = 0; j < resolution; j++) {
			global_img->final_pixel[i][j] = get_color(j, resolution - 1 - i);
		}
	}
	return 0;
}

void render(image *im) {

	global_img = im;
	thread_steps = resolution / num_threads; //work per thread

	pthread_t tid[num_threads];
	int thread_id[num_threads];

	for (int i = 0; i < num_threads; i++)
		thread_id[i] = i;

	for(int i = 0; i < num_threads; i++) {
		pthread_create(&(tid[i]), NULL, thread_function, &(thread_id[i]));
	}

	for(int i = 0; i < num_threads; i++) {
		pthread_join(tid[i], NULL);
	}

	*im = *global_img;
}

void writeData(const char * fileName, image *img) {
	
	FILE* file;
	file = fopen(fileName, "wb");
	
	fprintf(file, "P5\n"); //always grey
	fprintf(file, "%d %d\n%d\n", resolution, resolution, 255); //maxval always 255

	for(int i = 0; i < resolution; i++) {
		for (int j = 0; j < resolution; j++) {
			fprintf(file, "%c", img->final_pixel[i][j]); //it works well with time
		}
	}

	fclose(file);
}
