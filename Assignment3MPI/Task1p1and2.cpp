#include <iostream>
#include <string>
#include <vector>
#include <mpi.h>

using namespace std;

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    string word = "Parallelization";
    int n = word.length();

    if (size != n) {
        if (rank == 0) {
            cout << "Please run with exactly " << n << " processes." << endl;
        }
        MPI_Finalize();
        return 0;
    }

    char my_char = word[rank];

    vector<char> send_buf(size, ' ');
    vector<char> recv_buf(size, ' ');

    int target_rank = n - 1 - rank;
    send_buf[target_rank] = my_char;

    MPI_Alltoall(send_buf.data(), 1, MPI_CHAR, recv_buf.data(), 1, MPI_CHAR, MPI_COMM_WORLD);

    char new_char = recv_buf[rank];

    for (int i = 0; i < size; ++i) {
        if (rank == i) {
            cout << "Process " << rank << " | Element \"" << new_char << "\"" << endl;
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}