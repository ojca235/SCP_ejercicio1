#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>


int main(int argc, char *argv[])
{
	FILE *fileA;
	FILE *fileB;
	FILE *fileC;
	char *fileAname= "matrizA.txt",*fileBname= "matrizB.txt",*resfilename= "MPI_matriz_res.txt";
	int myrank, size;
	int colA,rowA,colB,rowB;
	int nRows,restodematriz;
	int *matrizbufA,*matrizbufC;
	int *matrizA,*matrizB,*matrizres;
	int sum,aux,i,j,k;
	double t0,t1;
	

	MPI_Init(&argc, &argv); //llamada a la funcion de MPI que hace que se inicialice
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank); //Se le da cada procesador su rank que es el equivalente a su id
	MPI_Comm_size(MPI_COMM_WORLD, &size); //Numero total de procesadores que hay

	if(myrank==0) 
	{
		//En caso de ser el procesador root qque equivale al que tiene rank cero lo que hará es leer los dos ficheros para inicializar las dos matrices que se van a multiplicar
		fileA = fopen(fileAname,"r");
		fileB = fopen(fileBname,"r");
		if(fileA==NULL || fileB==NULL) // Se comprueba que no haya habido ningun error al abrir los ficheros
		{
			printf("No se ha encontrado el fichero matrizA.txt o matrizB.txt, asegurese de haber ejecutado el programa que genera matrices.\n");
			return 0;
		}
		else
		{//En caso de que se haya abierto correctamente lols ficheros entonces lo que se hara es leerlos para crear las matrices
			printf("Abriendo las dos ficheros.\n");// Imprimimos por pantalla que estamos en el paso de abrir los ficheros para inicializar las matrices

			fscanf(fileA, "%d %d",&rowA,&colA);//Leemos del primer fichero sus dimensiones
			fscanf(fileB, "%d %d",&rowB,&colB);//Leemos del segundo fichero sus dimendsiones
			if (colA!=rowB)//Comprobacion de que la columna de la primera matriz A sea igual a la de la matriz B para poder multiplicar
			{
				printf("Las matrices que se estan intentado multiplicar no es posible multiplicarlas, la razón es que columna de matriz1 es %d y las filas de la matriz2 es %d.\n",colA,rowB);
				return 0;
			}
			else
			{
				//Una vez comprobado de que se puede multiplicar lo que se hará es repartir las filas
				nRows=rowA/size;//Se reparte las filas entre el numero de procesadores, es decir, cunatas filas le corresponde a cada procesador
				restodematriz=rowA%size;//Se comprueba que despues de haber

				//mediante malloc reservamos dinamicamente el espacio necesario para las matrices en el procesador 0 
				matrizA = (int*)malloc(rowA * colA * sizeof(int));
				matrizB = (int*)malloc(rowB * colB * sizeof(int));
				matrizres = (int*)malloc(rowA * colB * sizeof(int));

				//Lectura de la primera matriz
				while (!feof(fileA))
					for(i=0; i<rowA; i++)
						for(j=0; j<colA; j++)
							fscanf(fileA, "%d", &matrizA[(i*colA)+j]);

				//Lectura de la segunda matriz pero en este caso la transponemos
				while (!feof(fileB))
					for(i=0; i<rowB; i++)
						for(j=0; j<colB; j++)
							fscanf(fileB, "%d", &matrizB[j*(rowB)+i]);

				//Al transponer la matriz B entonces debemos transponer los valores de la fila y la columna
				aux=rowB;
				rowB=colB;
				colB=aux;

				//Cierre de los dos archivos que se han abierto
				fclose(fileA);
				fclose(fileB);
			}
		}
	}
	//Empezamos a cornometrar antes de enviar los datos al resto de procesadores
	t0 = MPI_Wtime();

	//Se les envia al resto de procesadores las variables necesarias para poder realizar su parte
	MPI_Bcast(&nRows,1,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&rowA,1,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&colA,1,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&rowB,1,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&colB,1,MPI_INT,0,MPI_COMM_WORLD);

	if (myrank!=0)
	{
		//Todos los procesadores que no sean el procesador root reservaran espacio con malloc para almacenar las matriz B entera más las filas que le corresponde de la matriz A
		matrizB = (int*)malloc(rowB * colB * sizeof(int));
		matrizbufA = (int*)malloc(nRows * colA * sizeof(int));
	}
	// El procesador root hara broadcast de la matriz B 
	MPI_Bcast(&matrizB[0],rowB*colB,MPI_INT,0,MPI_COMM_WORLD);
	//mediante malloc se reservara dinamicamente espacio en todos los procesadores el espacio justo y necesario para guardar la parte de multiplicación que le corresponde
	matrizbufC = (int*)malloc(nRows * rowB * sizeof(int));

	if(myrank==0)
	{
		//El proceso root se encargará de repartir la matriz A entre todos los procesadores
		for (i=1; i<size; i++) 
		{
			MPI_Send(&matrizA[nRows*colA*i],nRows*colA, MPI_INT, i, 0, MPI_COMM_WORLD);
		}
	}
	else
	{ 
		//El resto de procesadores que no sean root esperan a recibir la parte que les corresponde de la matriz A
		MPI_Recv(&matrizbufA[0],nRows*colA, MPI_INT, 0, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		//Simnplemnete se multiplica teniendo en cuenta la cantidad de filas le corresponde
		for(i=0;i<nRows;i++)
		{
			for(j=0; j < rowB; j++)
			{ 
				sum = 0;
				for(k = 0 ; k < colA; k++)
					sum+=matrizbufA[(i*colA)+k]*matrizB[(j*colA)+k];
				matrizbufC[(i*rowB)+j]=sum;
			}
		}
		//Una vez terminado la multiplicacion lo que hara es enviar al proceso root las filas que ha obtenido tras la multiplicacion
		MPI_Send(&matrizbufC[0],nRows*rowB, MPI_INT, 0, 666, MPI_COMM_WORLD);
		//Liberacion de los dos buffers temporales que se crean para los procesadores que no son root
		free(matrizbufA);
		free(matrizbufC);
	}

	if(myrank==0) //rank 0
	{
		//Simnplemnete se multiplica teniendo en cuenta la cantidad de filas le corresponde
		for(i=0;i<nRows;i++)
		{
			for(j=0; j < rowB; j++)
			{
				sum = 0;
				for(k = 0 ; k < colA; k++)
					sum+=matrizA[(i*colA)+k]*matrizB[(j*colA)+k];
				matrizres[(i*rowB)+j]=sum;
			}
		}
		if (restodematriz!=0)//Se comprueba haber si despues de repartir la matriz queda alguna fila que no se haya realizado la operación
		{
			//Multiplicacion sobre todas las filas en las que no se ha hecho ninguna operación
			//Nota: Tiene un problema,  si las filas es menor al numero de procesadores que van a ejecutar la matriz todo vendrá aqui y es el procesador root el que hará la multiplicación
			for(i=nRows*size;i<(nRows*size)+restodematriz;i++)
			{
				for(j=0; j < rowB; j++)
				{
					sum = 0;
					for(k = 0 ; k < colA; k++)

						sum+=matrizA[(i*colA)+k]*matrizB[(j*colA)+k];
					matrizres[(i*rowB)+j]=sum;
				}
			}
		}
		//Se libera el espacio de las matrices
		free(matrizA);
		free(matrizB);

		//Se recibe todos los resultados obtenidos en el resto de procesadores
		for (i=1; i<size; i++) 
			MPI_Recv(&matrizres[nRows*rowB*i],nRows*rowB, MPI_INT, i, 666, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		//Tiempo final en el que ya se ha hecho toda la multiplicacion
		t1 = MPI_Wtime();

		//Escritura del resultado en el fichero
		fileC = fopen(resfilename,"w");
		fprintf(fileC,"%d %d",rowA,rowB);
		fprintf(fileC, "\n");
		for(i=0;i<rowA;i++){
			for(j=0;j<rowB;j++)
				fprintf(fileC, "%d ",matrizres[(i*rowB)+j]);
			fprintf(fileC, "\n");
		}
		free(matrizres);
		fclose(fileC);
		printf("Se ha guardado el resultado en el fichor %s.\n", resfilename);
		printf("Tej + Comunicaciones : %1.3f msec\n", (t1 - t0)*1000);
	}
	MPI_Finalize();
	return 0;
}