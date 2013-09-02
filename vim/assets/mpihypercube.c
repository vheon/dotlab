#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mpi.h>

#define displacement(type, member) \
	  (((unsigned long) &((type *)0)->member) - (unsigned long)((type *)0))

double log2(double x);
int create_hypercube_topology(MPI_Comm old_comm, int n, MPI_Comm *new_comm);
void keep_low(double *mbuffer, double *obuffer);
void keep_high(double *mbuffer, double *obuffer);

double *tmp;

int main(int argc, char *argv[])
{
	int cw_size;
	int rank;

	int ntri;
	size_t buff_size = ntri * 9 * sizeof(double);
	double *buffer = malloc(buff_size);

	MPI_Comm hc_comm;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &cw_size);

	int hyper_dims = log2(cw_size);

	create_hypercube_topology(MPI_COMM_WORLD, hyper_dims, &hc_comm);
	MPI_Comm_rank(hc_comm, &rank);

	int i;
	int source, dest;
	for (i = 0; i < hyper_dims; i++) {
		MPI_Cart_shift(hc_comm, i, 1, &source, &dest);
		printf("Step %2d: I am process %3d and I will excange my buffer with %3d (source = %3d)\n", i, rank, dest, source);
		MPI_Barrier(hc_comm);
	}

	MPI_Finalize();

	return 0;
}

double log2(double x)
{
	return log(x) / log(2);
}

int create_hypercube_topology(MPI_Comm old_comm, int n, MPI_Comm *new_comm)
{
	int i;
	int dims[n];
	int period[n];

	for (i = 0; i < n; ++i) {
		dims[i] = 2;
		period[i] = 1;
	}

	/*
	 * reorder may be 1 if the master remain the same since is the one with
	 * access to the file
	 */
	int err = MPI_Cart_create(old_comm, n, dims, period, 0, new_comm);

	return err;
}

void
keep_low(double *mbuffer, double *obuffer)
{
}

void
keep_high(double *mbuffer, double *obuffer)
{
	
}
