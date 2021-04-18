#include <stdio.h>
#include <stdlib.h>
#include <time.h>



int main(int argc, char *argv[])
{
	FILE *fileA;
	FILE *fileB;
	FILE *resfile;
	char *fileAname= "matriz1.txt",*fileBname= "matriz2.txt",*resfilename= "MPI_matriz_res.txt";
	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	int N1,N2,M1,M2;
	int *matriz1 = NULL;
	int *matriz2 = NULL;
	int *matriz_res = NULL;
	double tej;
	struct timespec t0, t1;
    int i,j;
    fileA = fopen(fileAname,"r");
    fileB = fopen(fileBname,"r");
    resfile = fopen(resfilename,"w");
    if (fileA==NULL || fileB==NULL || resfile==NULL)
    {
        printf("error al intentar abrir el fichero\n");
        return -1;
    }
    fscanf(fileA, "%d %d",&N1,&M1);
    matriz1=(int *)malloc(N1*M1*sizeof(int));
    int contCol1=0,contFil1=0;
    printf("Primera matriz de dimension %d x %d\n",N1,M1);
    while(!feof(fileA))
        for(i=0; i<N1; i++)
		for(j=0; j<M1; j++)
			fscanf(fileA, "%d", &matriz1[(i*M1)+j]);
    printf("\n");
    fscanf(fileB, "%d %d",&N2,&M2);
    matriz2=(int *)malloc(N2*M2*sizeof(int));
    int contCol2=0,contFil2=0;
    printf("Segunda matriz de dimension %d x %d\n",N2,M2);
    while (!feof(fileB))
	for(i=0; i<N2; i++)
		for(j=0; j<M2; j++)
			fscanf(fileB, "%d", &matriz2[j*(N2)+i]);
    printf("\n");
    fclose(fileA);
    fclose(fileB);
    if (line)free(line);
    if(M1!=N2)
    {
        printf("La multiplicación de las matrices no es posible por que %d y %d no coinciden.",M1,N2);
	fclose(resfile);
	free(matriz1);
	free(matriz2);
        return(1);
    }
    
    printf("La multiplicación de las matrices es:\n");
    matriz_res=(int *)malloc(N1*M2*sizeof(int));
    int sum;
    gettimeofday (&t0, 0);
    for(i=0;i<N1;i++)
    {
        for(j=0;j<M2;j++)
        {
	    sum=0;
            for(int k=0;k<M1;k++)
            {
                sum+=matriz1[i*M1+k]*matriz2[j*M1+k];
            }
	    matriz_res[i*M2+j]=sum;
        }
    }
    gettimeofday (&t1, 0);
    tej = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / (double)1e9;
    printf ("\n Tej = %1.3f ms\n\n", tej*1000);
    fprintf(resfile,"%d %d \n",N1,N2);
     for(i=0;i<N1;i++)
    {
        for(j=0;j<M2;j++)
        {
            fprintf(resfile,"%d ",matriz_res[i*M2+j]);
            
        }
	fprintf(resfile,"\n");
    }
    fclose(resfile);
    free(matriz1);
    free(matriz2);
    free(matriz_res);
    return(0);
}
