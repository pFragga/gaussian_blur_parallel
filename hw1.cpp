#include "hw1.h"

#include <chrono>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include "stb_image.h"
#include "stb_image_write.h"

/* implemented in main.cpp */
unsigned char blur(int x, int y, int channel, unsigned char* input, int width, int height);

#define JOBS 2

struct img_proc_area {
	int tid;
	int w, w0, w1, h0, h1, c;
	unsigned char *in, *out;
	const char *name;
};

void *foo(void *arg)
{
	if (!arg)
		return NULL;

	struct img_proc_area *img = (struct img_proc_area *)arg;
	printf("TRD#%d: Processing '%s'\n[width] %d - %d\n[height] %d - %d\n[channels] %d\n",
			img->tid, img->name, img->w0, img->w1, img->h0,
			img->h1, img->c);
	for (int y = img->h0; y < img->h1; ++y) {
		for (int x = img->w0; x < img->w1; ++x) {
			int pxl = y * img->w + x;
			for (int c = 0; c < 4; ++c)
				img->out[4 * pxl + c] = blur(x, y, c, img->in, img->w, img->h1);
		}
	}

	free(arg);
	return NULL;
}

void gaussian_blur_parallel(const char *filename)
{
	if (!filename)
		return;

	int width = 0, height = 0, img_orig_channels = 4;
	unsigned char* img_in = stbi_load(filename, &width, &height, &img_orig_channels /*image file channels*/, 4 /*requested channels*/);
	if (!img_in) {
		fprintf(stderr, "Could not load '%s'\n", filename);
		return;
	}
	unsigned char *img_out = new unsigned char[width * height * 4];
	pthread_t *threads = new pthread_t[JOBS];
	int _w = 0;

	auto start = std::chrono::high_resolution_clock::now();
	int i;
	for (i = 0; i < JOBS; ++i) {
		/* make sure to free this in the thread's subroutine */
		struct img_proc_area *ipa = (struct img_proc_area *)malloc(sizeof(*ipa));
		ipa->tid = i;
		ipa->w = width;
		ipa->w0 = _w;
		ipa->w1 = _w + width / JOBS;
		ipa->h0 = 0;
		ipa->h1 = height;
		ipa->c = img_orig_channels;
		ipa->in = img_in;
		ipa->out = img_out;
		ipa->name = filename;
		_w += width / JOBS;		/* IMPORTANT */
		if (pthread_create(&threads[i], NULL, &foo, (void *)ipa) != 0)
			perror("Could not create thread.");
	}
	for (i = 0; i < JOBS; ++i) {
		if (pthread_join(threads[i], NULL) != 0)
			perror("Could not join thread.");
	}
	auto end = std::chrono::high_resolution_clock::now();
	int time = (int)std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	printf("Gaussian Blur - Parallel: Time %dms\n", time);

	stbi_write_jpg("blurred_image_parallel.jpg", width, height, 4, img_out, 90 /*quality*/);
	delete[] img_out;
	delete[] threads;
}

void gaussian_blur_separate_parallel(const char *filename)
{
	if (!filename)
		return;
}
