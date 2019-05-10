#pragma once

struct NN {
	int * n; // pocty neuronu
	int l; // pocet vrstev
	double *** w; // vahy

	double * in; // vstupni vektor
	double * out; // vystupni vektor
	double ** y; // vystupni vektory vrstev

	double ** d; // chyby neuronu
};

NN * createNN( int n, int h, int o );
void releaseNN( NN *& nn );
void feedforward( NN * nn );
double backpropagation( NN * nn, double * t );
void setInput( NN * nn, double * in, bool verbose = false  );
int getOutput( NN * nn, bool verbose = false );
