#include <mpi.h>
#include <cstdio>
#include <vector>
//WINDOWS DOES NOT SUPPORT MPI_COMM_SPAWN, SO THIS CODE WILL NOT WORK ON WINDOWS
int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Comm parent;
    MPI_Comm_get_parent(&parent);

    if (parent == MPI_COMM_NULL) {
        int n = 0;
        if (rank == 0) {
            scanf("%d", &n);
        }
        MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

        printf("I am %d process from %d processes! My parent is none.\n", rank, size);
        fflush(stdout);

        MPI_Barrier(MPI_COMM_WORLD);

        MPI_Comm children;
        std::vector<int> errors(n);
        MPI_Comm_spawn(argv[0], MPI_ARGV_NULL, n, MPI_INFO_NULL, 0,
            MPI_COMM_WORLD, &children, errors.data());

        MPI_Comm_disconnect(&children);
    }
    else {
        int parentRank = 0;
        printf("I am %d process from %d processes! My parent is %d.\n", rank, size, parentRank);
        fflush(stdout);

        MPI_Comm_disconnect(&parent);
    }

    MPI_Finalize();
    return 0;
}