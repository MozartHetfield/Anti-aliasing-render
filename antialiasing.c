#include "homework.h"

int num_threads, resize_factor;
int width, height, maxval;
int new_width, new_height;
int work_per_thread;

int gaussian_kernel[9] = {1, 2, 1, 2, 4, 2, 1, 2, 1};
unsigned char R = 'r', G = 'g', B = 'b', P = 'p'; //p from pixel, stands for grey image

image* global_out = NULL;
image* global_in = NULL;

unsigned char odd_resizer(int from_i, int from_j, int to_i, int to_j, unsigned char pixel, image *img) {
	int index = 0, helper = 0;

	if (pixel == R) {
		for (int i = from_i; i <= to_i; i++) {
			for (int j = from_j; j <= to_j; j++) {
				helper += gaussian_kernel[index] * img->colored[i][j].r;
				index++;
			}
		}
	} else if (pixel == G) {
		for (int i = from_i; i <= to_i; i++) {
			for (int j = from_j; j <= to_j; j++) {
				helper += gaussian_kernel[index] * img->colored[i][j].g;
				index++;
			}
		}
	} else if (pixel == B) {
		for (int i = from_i; i <= to_i; i++) {
			for (int j = from_j; j <= to_j; j++) {
				helper += gaussian_kernel[index] * img->colored[i][j].b;
				index++;
			}
		}
	} else {
		for (int i = from_i; i <= to_i; i++) {
			for (int j = from_j; j <= to_j; j++) {
				helper += gaussian_kernel[index] * img->colorless[i][j].pixel;
				index++;
			}
		}
	}
	helper /= 16;
	return (unsigned char) helper;
}

unsigned char even_resizer(int from_i, int from_j, int to_i, int to_j, unsigned char pixel, image *img) {
	int helper = 0;

	if (pixel == R) {
		for (int i = from_i; i <= to_i; i++) {
			for (int j = from_j; j <= to_j; j++) {
				helper += img->colored[i][j].r;
			}
		}	
	} else if (pixel == G) {
		for (int i = from_i; i <= to_i; i++) {
			for (int j = from_j; j <= to_j; j++) {
				helper += img->colored[i][j].g;
			}
		}	
	} else if (pixel == B) {
		for (int i = from_i; i <= to_i; i++) {
			for (int j = from_j; j <= to_j; j++) {
				helper += img->colored[i][j].b;
			}
		}
	} else {
		for (int i = from_i; i <= to_i; i++) {
			for (int j = from_j; j <= to_j; j++) {
				helper += img->colorless[i][j].pixel;
			}
		}
	}
	helper /= (resize_factor * resize_factor);
	return (unsigned char) helper;
}

void readInput(const char * fileName, image *img) {
	FILE* file = fopen(fileName, "r");
	unsigned char color, useless;
	fscanf(file, "%c %c %d %d %d%c", &color, &color, &width, &height, &maxval, &useless);

	if (color == '6') {
		unsigned char buffer[width * height * 3];
		img->colorless = NULL;
		img->colored = malloc(height * sizeof(colored_image*));
		for (int i = 0; i < height; i++) {
			img->colored[i] = malloc(width * sizeof(colored_image));
		}

		img->color = 1;
		fread(&buffer, sizeof(unsigned char), width * height * 3, file);
		int z = 0, k;

		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				for (k = 0; k < 3; k++) {
					if (z % 3 == 0) img->colored[i][j].r = buffer[z];
					else if (z % 3 == 1) img->colored[i][j].g = buffer[z];
					else img->colored[i][j].b = buffer[z];
					z++;
				}
			}
		}
	} else {
		unsigned char buffer[width * height];
		img->colored = NULL;
		img->colorless = malloc(height * sizeof(colorless_image*));
		for (int i = 0; i < height; i++) {
			img->colorless[i] = malloc(width * sizeof(colorless_image));
		}

		img->color = 0;
		fread(&buffer, sizeof(unsigned char), width * height, file);
		int z = 0;
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				img->colorless[i][j].pixel = buffer[z];
				z++;
			}
		}
	}
	fclose(file);
}

void writeData(const char * fileName, image *img) {
	FILE* file;
	file = fopen(fileName, "w");

	if (img->color == 0) {
		fprintf(file, "P5\n");
	} else {
		fprintf(file, "P6\n");
	}

	int z = 0;
	fprintf(file, "%d %d\n%d\n", new_width, new_height, maxval);
	if (img->color == 0) {
		unsigned char buffer[new_height * new_width];
		for(int i = 0; i < new_height; i++) {
			for (int j = 0; j < new_width; j++) {
				buffer[z] = img->colorless[i][j].pixel;
				z++;
			}
		}
		fwrite(buffer, sizeof(char), new_height * new_width, file); //buffer shortens the time
	} else {
		unsigned char buffer[new_height * new_width * 3];
		for(int i = 0; i < new_height; i++) {
			for (int j = 0; j < new_width; j++) {
				buffer[z] = img->colored[i][j].r;
				z++;
				buffer[z] = img->colored[i][j].g;
				z++;
				buffer[z] = img->colored[i][j].b;
				z++;
			}
		}
		fwrite(buffer, sizeof(char), new_height * new_width * 3, file);
	}
	
	fclose(file);
}

void* thread_function_colored(void* var) {
	int thread_id = *(int*)var;
	int from_i = thread_id * work_per_thread * resize_factor; //need to be multiple of resize_factor
	int to_i;
	if (thread_id == num_threads - 1) {
		to_i = height;
	} else {
		to_i = from_i + work_per_thread * resize_factor;
	}
	
	int pos_i = from_i / resize_factor;
	int pos_j = 0;

	if (resize_factor % 2 == 0) {
		for (int i = from_i; i < to_i; i += resize_factor) {
			for (int j = 0; j < width; j += resize_factor) {
				int i_delim = i + resize_factor - 1, j_delim = j + resize_factor - 1;
				if (i_delim >= height || j_delim >= width) {
					pos_j = 0;
					break;
				}
				global_out->colored[pos_i][pos_j].r = even_resizer(i, j, i_delim, j_delim, R, global_in);
				global_out->colored[pos_i][pos_j].g = even_resizer(i, j, i_delim, j_delim, G, global_in);
				global_out->colored[pos_i][pos_j].b = even_resizer(i, j, i_delim, j_delim, B, global_in);
				pos_j++;
			}
			pos_i++;
			pos_j = 0;
		}
	} else {
		for (int i = from_i; i < to_i; i += resize_factor) {
			for (int j = 0; j < width; j += resize_factor) {
				int i_delim = i + resize_factor - 1, j_delim = j + resize_factor - 1;
				if (i_delim >= height || j_delim >= width) {
					pos_j = 0;
					break;
				}
				global_out->colored[pos_i][pos_j].r = odd_resizer(i, j, i_delim, j_delim, R, global_in);
				global_out->colored[pos_i][pos_j].g = odd_resizer(i, j, i_delim, j_delim, G, global_in);
				global_out->colored[pos_i][pos_j].b = odd_resizer(i, j, i_delim, j_delim, B, global_in);
				pos_j++;
			}
			pos_i++;
			pos_j = 0;
		}
	}
	return 0;
}

void* thread_function_colorless(void* var) {
	int thread_id = *(int*)var;
	int from_i = thread_id * work_per_thread * resize_factor;
	int to_i;
	if (thread_id == num_threads - 1) {
		to_i = height;
	} else {
		to_i = from_i + work_per_thread * resize_factor;
	}
	
	int pos_i = from_i / resize_factor;
	int pos_j = 0;

	if (resize_factor % 2 == 0) {
		for (int i = from_i; i < to_i; i += resize_factor) {
			for (int j = 0; j < width; j += resize_factor) {
				int i_delim = i + resize_factor - 1, j_delim = j + resize_factor - 1;
				if (i_delim >= height || j_delim >= width) {
					pos_j = 0;
					break;
				}
				global_out->colorless[pos_i][pos_j].pixel = even_resizer(i, j, i_delim, j_delim, P, global_in);
				pos_j++;
			}
			pos_i++;
			pos_j = 0;
		}
	} else {
		for (int i = from_i; i < to_i; i += resize_factor) {
			for (int j = 0; j < width; j += resize_factor) {
				int i_delim = i + resize_factor - 1, j_delim = j + resize_factor - 1;
				if (i_delim >= height || j_delim >= width) {
					pos_j = 0;
					break;
				}
				global_out->colorless[pos_i][pos_j].pixel = odd_resizer(i, j, i_delim, j_delim, P, global_in);
				pos_j++;
			}
			pos_i++;
			pos_j = 0;
		}
	}
	return 0;
}

void resize(image *in, image * out) {
	global_in = in;
	new_height = height / resize_factor;
	new_width = width / resize_factor;
	work_per_thread = new_height / num_threads;

	pthread_t tid[num_threads];
	int thread_id[num_threads];

	for(int i = 0; i < num_threads; i++) {
		thread_id[i] = i;
	}

	if (in->color == 1) {
		global_out = malloc(sizeof(image*));
		global_out->colorless = NULL;
		global_out->colored = malloc(new_height * sizeof(colored_image*));
		for (int i = 0; i < new_height; i++) {
			global_out->colored[i] = malloc(new_width * sizeof(colored_image));
		}
		global_out->color = 1;

		for(int i = 0; i < num_threads; i++) {
			pthread_create(&(tid[i]), NULL, thread_function_colored, &(thread_id[i]));
		}
	} else {
		global_out = malloc(sizeof(image*));
		global_out->colored = NULL;
		global_out->colorless = malloc(new_height * sizeof(colorless_image*));
		for (int i = 0; i < new_height; i++) {
			global_out->colorless[i] = malloc(new_width * sizeof(colorless_image));
		}
		global_out->color = 0;

		for(int i = 0; i < num_threads; i++) {
			pthread_create(&(tid[i]), NULL, thread_function_colorless, &(thread_id[i]));
		}
	}

	for(int i = 0; i < num_threads; i++) {
		pthread_join(tid[i], NULL);
	}
	
	*out = *global_out;
}