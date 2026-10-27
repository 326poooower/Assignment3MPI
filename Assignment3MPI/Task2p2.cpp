#include <iostream>
#include <vector>
#include <mpi.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n = 0;
    std::vector<double> A, B, C;

    if (rank == 0) {
        std::cin >> n;
        A.resize(n * n);
        B.resize(n * n);
        C.resize(n * n);
        for (int i = 0; i < n * n; ++i) std::cin >> A[i];
        for (int i = 0; i < n * n; ++i) std::cin >> B[i];
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int rows = n / size;
    std::vector<double> local_A(rows * n);
    std::vector<double> local_C(rows * n);
    if (rank != 0) {
        B.resize(n * n);
    }

    MPI_Bcast(B.data(), n * n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        for (int i = 1; i < size; ++i) {
            MPI_Send(A.data() + i * rows * n, rows * n, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
        }
        for (int i = 0; i < rows * n; ++i) {
            local_A[i] = A[i];
        }
    }
    else {
        MPI_Recv(local_A.data(), rows * n, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < n; ++j) {
            double sum = 0.0;
            for (int k = 0; k < n; ++k) {
                sum += local_A[i * n + k] * B[k * n + j];
            }
            local_C[i * n + j] = sum;
        }
    }

    if (rank == 0) {
        for (int i = 0; i < rows * n; ++i) {
            C[i] = local_C[i];
        }
        for (int i = 1; i < size; ++i) {
            MPI_Recv(C.data() + i * rows * n, rows * n, MPI_DOUBLE, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }
    else {
        MPI_Send(local_C.data(), rows * n, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
    }

    if (rank == 0) {
        for (int i = 0; i < n * n; ++i) {
            std::cout << C[i] << (i == n * n - 1 ? "" : " ");
        }
        std::cout << std::endl;
    }

    MPI_Finalize();
    return 0;
}