#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "backprop.h"

#define LAMBDA 1.0
#define ETA 0.1

#define SQR(x) ( ( x ) * ( x ) )


const double sigmoid(const double sij, const double lambda) {
	return 1.0 / (1.0 + exp(-1 * LAMBDA * sij));
}

const double derivate_sigmoid(const double y) {
	return y * (1 - y);
}

void randomize(double * p, int n) {
	for (int i = 0; i < n; i++) {
		p[i] = (double) rand() / (RAND_MAX);
	}
}

NN * createNN(int n, int h, int o) {
	srand(time(NULL));
	NN * nn = new NN;

	nn->n = new int[3];
	nn->n[0] = n;
	nn->n[1] = h;
	nn->n[2] = o;
	nn->l = 3;

	nn->w = new double ** [nn->l - 1];


	for (int k = 0; k < nn->l - 1; k++) {
		nn->w[k] = new double * [nn->n[k + 1]];
		for (int j = 0; j < nn->n[k + 1]; j++) {
			nn->w[k][j] = new double[nn->n[k]];
			randomize(nn->w[k][j], nn->n[k]);
			// BIAS
			//nn->w[k][j] = new double[nn->n[k] + 1];			
			//randomize( nn->w[k][j], nn->n[k] + 1 );
		}
	}

	nn->y = new double * [nn->l];
	for (int k = 0; k < nn->l; k++) {
		nn->y[k] = new double[nn->n[k]];
		memset(nn->y[k], 0, sizeof(double) * nn->n[k]);
	}

	nn->in = nn->y[0];
	nn->out = nn->y[nn->l - 1];

	nn->d = new double * [nn->l];
	for (int k = 0; k < nn->l; k++) {
		nn->d[k] = new double[nn->n[k]];
		memset(nn->d[k], 0, sizeof(double) * nn->n[k]);
	}

	return nn;
}

void releaseNN(NN *& nn) {
	for (int k = 0; k < nn->l - 1; k++) {
		for (int j = 0; j < nn->n[k + 1]; j++) {
			delete[] nn->w[k][j];
		}
		delete[] nn->w[k];
	}
	delete[] nn->w;

	for (int k = 0; k < nn->l; k++) {
		delete[] nn->y[k];
	}
	delete[] nn->y;

	for (int k = 0; k < nn->l; k++) {
		delete[] nn->d[k];

	}
	delete[] nn->d;

	delete[] nn->n;

	delete nn;
	nn = NULL;
}

void feedforward(NN * nn) {
	double ski = 0;
	for (int level = 1; level < nn->l; level++) {
		for (int node = 0; node < nn->n[level]; node++) {
			ski = 0;
			for (int neighbourNode = 0; neighbourNode < nn->n[level - 1]; ++neighbourNode) {
				ski += nn->y[level - 1][neighbourNode] * nn->w[level - 1][node][neighbourNode];
			}
			nn->y[level][node] = sigmoid(ski, LAMBDA);
		}
	}
}

double backpropagation(NN * nn, double * t) {
	double error = 0.0;
	const int outputLayerIndex = nn->l - 1;
	double y_kj = 0;

	for (int i = 0; i < nn->n[outputLayerIndex]; i++) {
		y_kj = derivate_sigmoid(nn->y[outputLayerIndex][i]);
		nn->d[outputLayerIndex][i] = (t[i] - nn->y[outputLayerIndex][i]) * LAMBDA * y_kj;
	}

	for (int k = nn->l - 2; k > 0; k--) {
		for (int i = 0; i < nn->n[k]; i++) {
			nn->d[k][i] = 0.0;
			for (int j = 0; j < nn->n[k + 1]; j++) {
				nn->d[k][i] += nn->d[k + 1][j] * nn->w[k][j][i];
			}
			nn->d[k][i] *= LAMBDA * derivate_sigmoid(nn->y[k][i]);
		}
	}

	for (int k = 0; k < nn->l - 1; k++) {
		for (int i = 0; i < nn->n[k + 1]; i++) {
			for (int j = 0; j < nn->n[k]; j++) {
				nn->w[k][i][j] += ETA * nn->d[k + 1][i] * nn->y[k][j];
			}
		}
	}

	for (int i = 0; i < nn->n[nn->l-1]; i++) {
		error += pow(t[i] - nn->y[2][i], 2);
	}
	error /= 2.0;

	return error;
}

void setInput(NN * nn, double * in, bool verbose) {
	memcpy(nn->in, in, sizeof(double) * nn->n[0]);

	if (verbose) {
		printf("input=(");
		for (int i = 0; i < nn->n[0]; i++) {
			printf("%0.3f", nn->in[i]);
			if (i < nn->n[0] - 1) {
				printf(", ");
			}
		}
		printf(")\n");
	}
}

int getOutput(NN * nn, bool verbose) {
	double max = 0.0;
	int max_i = 0;
	if (verbose) printf(" output=");
	for (int i = 0; i < nn->n[nn->l - 1]; i++) {
		if (verbose) printf("%0.3f ", nn->out[i]);
		if (nn->out[i] > max) {
			max = nn->out[i];
			max_i = i;
		}
	}
	if (verbose) printf(" -> %d\n", max_i);
	//if (nn->out[0] > nn->out[1] && nn->out[0] - nn->out[1] < 0.1) return 2;
	return max_i;
}
