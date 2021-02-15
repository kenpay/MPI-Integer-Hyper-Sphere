#include "mpi.h"
#include <stdio.h>
#include <string>
#include <cmath>

using namespace std;
int n, * sol, n_count = 0, starting_point, end_point, numtasks, rang;
float r;

void afisare()
{/*
	for (int index = 0; index < n; index++)
		printf("%d ", sol[index]);
	printf("\n");*/
	n_count++;
}

bool verifica(int poz)
{
	int sum = 0;
	for (int index = 0; index < n; index++)
		sum += (sol[index] * sol[index]);
	float value = r * r;
	return sum <= r * r;
}

void bkt(int poz)
{
	if (poz == n) afisare();
	else
	{
		if (poz == 0)
		{
			if (rang != numtasks - 1)
				for (int i = starting_point; i < end_point; i++)
				{
					sol[poz] = i;
					if (verifica(poz)) bkt(poz + 1);
					else {
						sol[poz] = 0; break;
					}
				}
			else
				for (int i = starting_point; i <= end_point; i++)
				{
					sol[poz] = i;
					if (verifica(poz)) bkt(poz + 1);
					else {
						sol[poz] = 0; break;
					}
				}
		}
		else
		{
			for (int i = 0; ; i++)
			{
				sol[poz] = i;
				if (verifica(poz)) bkt(poz + 1);
				else {
					sol[poz] = 0; break;
				}
			}
			for (int i = -1; ; i--)
			{
				sol[poz] = i;
				if (verifica(poz)) bkt(poz + 1);
				else {
					sol[poz] = 0; break;
				}
			}
		}
	}

}

int main(int argc, char* argv[])
{
	n = atoi(argv[1]);
	r = stof(argv[2]);

	int  len, rc;
	char hostname[MPI_MAX_PROCESSOR_NAME];

	rc = MPI_Init(&argc, &argv);

	if (rc != MPI_SUCCESS) {
		printf("Error starting MPI program. Terminating.\n");
		MPI_Abort(MPI_COMM_WORLD, rc);
	}

	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	MPI_Comm_rank(MPI_COMM_WORLD, &rang);

	int t = int(float((2 * int(r)) + 1) / numtasks);
	starting_point = -int(r) + (rang)*t, end_point = -int(r) + (rang + 1) * t;
	if (rang == numtasks - 1)
		end_point = int(r);

	//printf("%d %d\n", starting_point, end_point);

	MPI_Get_processor_name(hostname, &len);

	//printf("Number of tasks= %d My rank= %d Running on %s\n", numtasks, rang, hostname);

	sol = new int[n];
	for (int index = 0; index < n; index++)
		sol[index] = 0;

	bkt(0);

	delete[] sol;

	MPI_Barrier(MPI_COMM_WORLD);
	
	if (rang == 0)
	{
		for (int i = 1; i < numtasks; i++)
		{
			MPI_Status status;
			int valueReceived;
			MPI_Recv(&valueReceived, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
			n_count += valueReceived;
		}
	}
	else
	{
		MPI_Send(&n_count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		MPI_Finalize();
		return 0;
	}


	printf("%d ", n_count);

	MPI_Finalize();

	return 0;
}