#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mpi.h>

#define MASTER 0

void create_matrix_comm(MPI_Comm old_comm, MPI_Comm *matrix_comm);

void alloc_submatrix_buffer(int **submatrix);

void compute_matrixes_variables(const char *matrix_filename, MPI_Comm matrix_comm);
void get_size_of_matrix(FILE *matrix_file, int *N);

void distribute_matrix(const char *matrix_filename,int *matrix_a, MPI_Comm matrix_comm);
void get_nth_submatrix(int nth, FILE *matrix_file, int *submatrix);
void compute_matrix_mul(int *matrix_a, int *matrix_b, int *matrix_c, int n);

void write_result(const char *matrix_c_filename, int *submatrix, MPI_Comm matrix_comm);

/* info about the topology that I keep here cause is a pain to ask
	directly to the topology through the C API */
MPI_Comm matrix_comm;
int pp_dims;

int N;
int sub_n;
int pp_dims;


int rank, size;

int main(int argc, char *argv[])
{
	int *matrix_a;
	int *matrix_b;
	int *matrix_c;

	const char *matrix_a_filename = argv[1];
	const char *matrix_b_filename = argv[2];
	const char *matrix_c_filename = argv[3];

	MPI_Comm matrix_comm;

	MPI_Init(&argc, &argv);

	create_matrix_comm(MPI_COMM_WORLD, &matrix_comm);
	MPI_Comm_size(matrix_comm, &size);
	MPI_Comm_rank(matrix_comm, &rank);

	compute_matrixes_variables(matrix_a_filename, matrix_comm);

	alloc_submatrix_buffer(&matrix_a);
	alloc_submatrix_buffer(&matrix_b);
	alloc_submatrix_buffer(&matrix_c);

	distribute_matrix(matrix_a_filename, matrix_a, matrix_comm);
	distribute_matrix(matrix_b_filename, matrix_b, matrix_comm);

	/* The actual cannon algorithms */
	int row_source, row_dst;
	int col_source, col_dst;
	MPI_Cart_shift(matrix_comm, 0, -1, &row_source, &row_dst);
	MPI_Cart_shift(matrix_comm, 1, -1, &col_source, &col_dst);
	int i;
	for (i = 0; i < pp_dims; i++) {
		compute_matrix_mul(matrix_a, matrix_b, matrix_c, N);
		MPI_Sendrecv_replace(matrix_a, sub_n * sub_n, MPI_INT,
				     row_source, MPI_ANY_TAG, row_dst, MPI_ANY_TAG,
				     matrix_comm, MPI_STATUS_IGNORE);

		MPI_Sendrecv_replace(matrix_b, sub_n * sub_n, MPI_INT,
				     col_source, MPI_ANY_TAG, col_dst, MPI_ANY_TAG,
				     matrix_comm, MPI_STATUS_IGNORE);
	}


	write_result(matrix_c_filename, matrix_c, matrix_comm);

	free(matrix_a);
	free(matrix_b);
	free(matrix_c);

	MPI_Comm_free(&matrix_comm);

	MPI_Finalize();
	return 0;
}

void
compute_matrix_mul(int *matrix_a, int *matrix_b, int *matrix_c, int n)
{
	int i, j, k;
	for (i = 0; i < n; ++i)
		for (j = 0; j < n; ++j)
			for (k = 0; k < n; ++k)
				matrix_c[i*n + j] += matrix_a[i*n + k] * matrix_b[k*n + j];
}

void
create_matrix_comm(MPI_Comm old_comm, MPI_Comm *matrix_comm)
{
	int size;
	MPI_Comm_size(old_comm, &size);

	pp_dims = sqrt(size);
	int dims[] = { pp_dims, pp_dims };
	int periods[] = { 1, 1 };

	MPI_Cart_create(old_comm, 2, dims, periods, 1, matrix_comm);
}

void
get_size_of_matrix(FILE *matrix_file, int *N)
{
	long int pos = ftell(matrix_file);
	rewind(matrix_file);
	fread(N, sizeof(*N), 1, matrix_file);
	fseek(matrix_file, pos, SEEK_SET);
}

/*
* nth: index 0-based
* matrix_file is a file with a squared matrix
* N elements per dimention in the matrix
* submatrix: the buffer that will hold the submatrix (preallocated)
* sub_n: # of elements per dimention of the submatrix
*/
void
get_nth_submatrix(int nth, FILE *matrix_file, int *submatrix)
{
	int submatrix_per_row = N / sub_n;
	int row_index = nth / submatrix_per_row;
	int col_index = nth % submatrix_per_row;

	fseek(matrix_file, sizeof(long int), SEEK_SET);
	fseek(matrix_file, sizeof(*submatrix) * N * sub_n * row_index, SEEK_CUR);

	size_t buff_size = sizeof(*submatrix) * N * sub_n;
	int *buffer = malloc(buff_size);
	fread(buffer, buff_size, 1, matrix_file);
	int row;
	for (row = 0; row < sub_n; row++)
		memcpy(submatrix + (sub_n * row) + (sub_n * col_index),
		       buffer + (N * row),
		       sizeof(*submatrix) * sub_n);

	free(buffer);
}

void
compute_matrixes_variables(const char *matrix_filename, MPI_Comm matrix_comm)
{
	pp_dims = sqrt(size);

	if (rank == MASTER) {
		FILE *matrix_file = fopen(matrix_filename, "rb");
		get_size_of_matrix(matrix_file, &N);
		fclose(matrix_file);
	}

	MPI_Bcast(&N, 1, MPI_INT, MASTER, matrix_comm);
	sub_n = N / pp_dims;

	if (N % pp_dims != 0)
		MPI_Abort(matrix_comm, 1);
}

void
distribute_matrix(const char *matrix_filename,int *matrix_a, MPI_Comm matrix_comm)
{
	if (rank == MASTER) {
		FILE *matrix_file = fopen(matrix_filename, "rb");
		int i;
		/* starts from 1 since we keep the first submatrix for master */
		for (i = 1; i < size; i++) {
			get_nth_submatrix(i, matrix_file, matrix_a);
			MPI_Send(matrix_a, sub_n * sub_n, MPI_INT, i, MPI_ANY_TAG, matrix_comm);
		}

		get_nth_submatrix(0, matrix_file, matrix_a);
		fclose(matrix_file);
	}else {
		MPI_Recv(matrix_a, sub_n * sub_n, MPI_INT, MASTER, MPI_ANY_TAG, matrix_comm, MPI_STATUS_IGNORE);
	}
}

void
alloc_submatrix_buffer(int **submatrix)
{
	size_t submatrix_size = sizeof(**submatrix) * sub_n * sub_n;
	*submatrix = malloc(submatrix_size);
	memset(*submatrix, 0, submatrix_size);
}

void
write_result(const char *matrix_c_filename, int *submatrix, MPI_Comm matrix_comm)
{
	if (rank == MASTER) {
		size_t buff_size = sizeof(*submatrix) * N * sub_n;
		int *buffer = malloc(buff_size);

		FILE *matrix_c_file = fopen(matrix_c_filename, "wb");
		/* copy mine int the buffer */
		memcpy(buffer, submatrix, sizeof(*submatrix) * sub_n * sub_n);
		int i;
		for (i = 1; i < size; i++) {
			MPI_Recv(submatrix, sub_n * sub_n, MPI_INT, i, MPI_ANY_TAG, matrix_comm, MPI_STATUS_IGNORE);
			memcpy(buffer + (sub_n * sub_n), submatrix, sizeof(*submatrix) * sub_n * sub_n);

			if (i % pp_dims == pp_dims -1) {
				fwrite(buffer, N * sub_n, sizeof(*buffer), matrix_c_file);
				memset(buffer, 0, buff_size);
			}
		}
		free(buffer);
		fclose(matrix_c_file);
	} else {
		MPI_Send(submatrix, sub_n * sub_n, MPI_INT, MASTER, MPI_ANY_TAG, matrix_comm);
	}
}
