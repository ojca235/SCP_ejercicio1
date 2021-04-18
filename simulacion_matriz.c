#include <stdio.h>
#include <stdlib.h>
#include <time.h>



int main(int argc, char *argv[])
{
	FILE *fileA;
	FILE *fileB;
	FILE *fileC;
	char *fileAname= "matrizA.txt",*fileBname= "matrizB.txt",*resfilename= "Matriz_res.txt";
	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	int N1,N2,M1,M2;
	int *matrizA = NULL;
	int *matrizB = NULL;
	int *matrizres = NULL;
	double tej;
	struct timespec t0, t1;
    int i,j;
    fileA = fopen(fileAname,"r");
    fileB = fopen(fileBname,"r");
    fileC = fopen(resfilename,"w+");
    //Comprobacion de que todos los ficheros se han abierto bien
    if (!fileA || !fileB || !fileC)
    {
        printf("error al intentar abrir el fichero\n");
        return -1;
    }
    fscanf(fileA, "%d %d",&N1,&M1);//Obtención de las dimensiones de la matriz A
    matrizA=(int *)malloc(N1*M1*sizeof(int));//Se reserva memoria de manera dinamica para la matriz A
    int contCol1=0,contFil1=0;
    printf("Primera matriz de dimension %d x %d\n",N1,M1);
    //Lectura de la primera matriz
    while(!feof(fileA))
        for(i=0; i<N1; i++)
		for(j=0; j<M1; j++)
			fscanf(fileA, "%d", &matrizA[(i*M1)+j]);
    printf("\n");
    fscanf(fileB, "%d %d",&N2,&M2);//Obtención de las dimensiones de la matriz B
    matrizB=(int *)malloc(N2*M2*sizeof(int));//Se reserva memoria de manera dinamica para la matriz B
    int contCol2=0,contFil2=0;
    //Lectuara de la segunda matriz
    printf("Segunda matriz de dimension %d x %d\n",N2,M2);
    while (!feof(fileB))
	for(i=0; i<N2; i++)
		for(j=0; j<M2; j++)
			fscanf(fileB, "%d", &matrizB[j*(N2)+i]);//La matriz se transpone
    printf("\n");
    fclose(fileA);
    fclose(fileB);
    if (line)free(line);
    if(M1!=N2)//Comprobación de que se puede realizar la multilicación
    {
        printf("La multiplicación de las matrices no es posible por que %d y %d no coinciden.",M1,N2);
        fclose(fileC);
        free(matrizA);
        free(matrizB);
        return(1);
    }
    
    printf("La multiplicación de las matrices es:\n");
    matrizres=(int *)malloc(N1*M2*sizeof(int));//Se reserva memoria de manera dinamica para la matriz resultado
    int sum;
    gettimeofday (&t0, 0);//Comienzo del cronometro
    //Multipicacion de las matrices teniendo en cuenta que la matriz B esta transpuesta
    for(i=0;i<N1;i++)
    {
        for(j=0;j<M2;j++)
        {
	    sum=0;
            for(int k=0;k<M1;k++)
            {
                sum+=matrizA[i*M1+k]*matrizB[j*M1+k];
            }
	    matrizres[i*M2+j]=sum;
        }
    }
    gettimeofday (&t1, 0);//final cronomentro
    tej = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / (double)1e9;
    printf ("\n Tej = %1.3f ms\n\n", tej*1000);
   //Se guarda el resultado en el fichero C
    fprintf(fileC,"%d %d \n",N1,N2);
    for(i=0;i<N1;i++)
    {
        for(j=0;j<M2;j++)
        {
            fprintf(fileC,"%d ",matrizres[i*M2+j]);
            
        }
	fprintf(fileC,"\n");
    }
    fclose(fileC);
    free(matrizA);
    free(matrizB);
    free(matrizres);
    return(0);
}
