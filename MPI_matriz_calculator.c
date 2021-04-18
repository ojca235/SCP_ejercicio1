#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>


int main(int argc, char *argv[])
{
	FILE *fileA;
	FILE *fileB;
	FILE *resfile;
	char *fileAname= "matriz1.txt",*fileBname= "matriz2.txt",*resfilename= "MPI_matriz_res.txt";
	int rank, size;
	int colA,rowA,colB,rowB;
	int partrow,fracrow;
	int *matrizbufA,*matrizbufC;
	int *matrizA,*matrizB,*matrizres;
	int sum = 0;
	int i,j,k;
	int aux;
	double t0,t1;
	

	MPI_Init(&argc, &argv); //to initial MPI with argument | ex. argument = 1, argc = 1, argv(vector) = "1"
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); //ID of processor
	MPI_Comm_size(MPI_COMM_WORLD, &size); //amount of processor

	if(rank==0)
	{
		//open file
		fileA = fopen(fileAname,"r");
		fileB = fopen(fileBname,"r");
		if(!fileA || !fileB){
			printf("No se ha encontrado el fichero matriz1.txt o matriz2.txt, asegurese de haber ejecutado el programa que genera matrices.\n");
			return 0;
		}
		else
		{
			printf("Abriendo las dos ficheros.\n");

			fscanf(fileA, "%d %d",&rowA,&colA);
			fscanf(fileB, "%d %d",&rowB,&colB);
			if (colA!=rowB)
			{
				printf("Las matrices que se estan intentado multiplicar no es posible multiplicarlas, la razón es que columna de matriz1 es %d y las filas de la matriz2 es.\n",colA,rowB);
				return 0;
			}
			else
			{
				//repartición de columnas
				partrow=rowA/size;
				fracrow=rowA%size;

				//allocate matrix
				matrizA = (int*)malloc(rowA * colA * sizeof(int));
				matrizB = (int*)malloc(rowB * colB * sizeof(int));
				matrizres = (int*)malloc(rowA * colB * sizeof(int));


				//read matrixA
				while (!feof(fileA))
					for(i=0; i<rowA; i++)
						for(j=0; j<colA; j++)
							fscanf(fileA, "%d", &matrizA[(i*colA)+j]);

				//read matrixB with transposing
				while (!feof(fileB))
					for(i=0; i<rowB; i++)
						for(j=0; j<colB; j++)
							fscanf(fileB, "%d", &matrizB[j*(rowB)+i]);

				//swap row-col
				aux=rowB;
				rowB=colB;
				colB=aux;

				printf("read A B successful\n");

				//close file
				fclose(fileA);
				fclose(fileB);
			}
		}
	}

	t0 = MPI_Wtime();

	MPI_Bcast(&partrow,1,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&rowA,1,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&colA,1,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&rowB,1,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&colB,1,MPI_INT,0,MPI_COMM_WORLD);
	printf("bcast finish! from rank %d\n",rank);

	if (rank!=0)
	{
		matrizB = (int*)malloc(rowB * colB * sizeof(int));
		matrizbufA = (int*)malloc(partrow * colA * sizeof(int));
	}

	MPI_Bcast(&matrizB[0],rowB*colB,MPI_INT,0,MPI_COMM_WORLD);
	printf("Gotcha! matrix B from rank %d\n",rank);

	matrizbufC = (int*)malloc(partrow * rowB * sizeof(int));

	if(rank==0){
		//send matrix A
		for (i=1; i<size; i++) {
			MPI_Send(&matrizA[partrow*colA*i],partrow*colA, MPI_INT, i, 0, MPI_COMM_WORLD);
		}
		printf("send A successful\n");
	}
	else{ //other rank

		MPI_Recv(&matrizbufA[0],partrow*colA, MPI_INT, 0, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		printf("received matrix A from rank %d\n",rank);

		//multiplication in other rank
		printf("Start multiplication!\n");
		for(i=0;i<partrow;i++){ //shift down row in A
			for(j=0; j < rowB; j++){ //shift down row in B
				sum = 0;
				for(k = 0 ; k < colA; k++){
					sum+=matrizbufA[(i*colA)+k]*matrizB[(j*colA)+k];
				}
				matrizbufC[(i*rowB)+j]=sum;
			}
		}

		printf("finish multiplication in rank %d\n",rank);

		MPI_Send(&matrizbufC[0],partrow*rowB, MPI_INT, 0, 123, MPI_COMM_WORLD);
		printf("Send bufC! from rank %d\n",rank);

		free(matrizbufA);
		free(matrizbufC);
	}

	if(rank==0) //rank 0
	{
		//multiplication in rank 0
		printf("Start multiplication! rank 0\n");
		for(i=0;i<partrow;i++){
			for(j=0; j < rowB; j++){
				sum = 0;
				for(k = 0 ; k < colA; k++){
					sum+=matrizA[(i*colA)+k]*matrizB[(j*colA)+k];
				}
				matrizres[(i*rowB)+j]=sum;
			}
		}
		printf("finish multiplication in rank %d\n",rank);

		if (fracrow!=0)
		{
		printf("hey");
			//fraction multiplication in rank 0
			for(i=partrow*size;i<(partrow*size)+fracrow;i++){
				for(j=0; j < rowB; j++){
					sum = 0;
					for(k = 0 ; k < colA; k++){
						sum+=matrizA[(i*colA)+k]*matrizB[(j*colA)+k];
					}
					matrizres[(i*rowB)+j]=sum;
				}
			}
			printf("End frac mul\n");
		}

		free(matrizA);
		free(matrizB);

		for (i=1; i<size; i++) { //other PART from other rank
			MPI_Recv(&matrizres[partrow*rowB*i],partrow*rowB, MPI_INT, i, 123, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		}

		t1 = MPI_Wtime();

		//write file
		resfile = fopen(resfilename,"w+");
		fprintf(resfile,"%d %d",rowA,rowB);
		fprintf(resfile, "\n");
		for(i=0;i<rowA;i++){
			for(j=0;j<rowB;j++)
				fprintf(resfile, "%d ",matrizres[(i*rowB)+j]);
			fprintf(resfile, "\n");
		}
		free(matrizres);

		fclose(resfile);
		printf("Se ha guardado el resultado en el fichor %s.\n", resfilename);

		printf("Tiempo que que ha durado la comunicación más la multiplicación de la matriz : %1.3f msec\n", (t1 - t0)*1000);
	}

	MPI_Finalize();
	return 0;
}