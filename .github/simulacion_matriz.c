#include <stdio.h>
#include <stdlib.h>
#include <time.h>

FILE *fichero1;
FILE *fichero2;
char fich1[]="matriz1.txt";
char fich2[]="matriz2.txt";
char * line = NULL;
size_t len = 0;
ssize_t read;
int N1,N2,M1,M2;

double tej;
struct timespec t0, t1;

int main(int argc, char *argv[])
{
    int i,j;
    fichero1 = fopen(fich1,"r");
    fichero2 = fopen(fich2,"r");
    if (fichero1==NULL || fichero2==NULL)
    {
        printf("error al intentar abrir el fichero\n");
        return -1;
    }
    read = getline(&line, &len, fichero1);
    N1=atoi(line);
    read = getline(&line, &len, fichero1);
    M1=atoi(line);
    int matriz1[N1][M1];
    int contCol1=0,contFil1=0;
    printf("Primera matriz de dimension %d x %d\n",N1,M1);
    while((read = getline(&line, &len, fichero1)) != -1)
    {
        if (contCol1==M1)
        {
            contCol1=0;
            contFil1++;
            //printf("\n");
        }
        matriz1[contFil1][contCol1]=atoi(line);
        //printf("%d ",matriz1[contFil1][contCol1]);
        contCol1++;
    }
    printf("\n");
    read = getline(&line, &len, fichero2);
    N2=atoi(line);
    read = getline(&line, &len, fichero2);
    M2=atoi(line);
    int matriz2[N2][M2];
    int contCol2=0,contFil2=0;
    printf("Segunda matriz de dimension %d x %d\n",N2,M2);
    while((read = getline(&line, &len, fichero2)) != -1)
    {
        if (contCol2==M2)
        {
            contCol2=0;
            contFil2++;
            //printf("\n");
        }
        matriz2[contFil2][contCol2]=atoi(line);
        //printf("%d ",matriz2[contFil2][contCol2]);
        contCol2++;
    }
    printf("\n");
    gettimeofday (&t0, 0);

    if(M1!=N2)
    {
        printf("La multiplicación de las matrices no es posible por que %d y %d no coinciden.",M1,N2);
        fclose(fichero1);
        fclose(fichero2);
        if (line)free(line);
        return(0);
    }
    
    printf("La multiplicación de las matrices es:\n");
    int matriz_res[N1][M2];
    for(i=0;i<N1;i++)
    {
        for(j=0;j<M2;j++)
        {
            matriz_res[i][j]=0;
            for(int k=0;k<M1;k++)
            {
                matriz_res[i][j]+=matriz1[i][k]*matriz2[k][j];
            }
        }
    }
    gettimeofday (&t1, 0);
    tej = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / (double)1e9;
    printf ("\n Tej = %1.3f ms\n\n", tej*1000);

     for(i=0;i<N1;i++)
    {
        for(j=0;j<M2;j++)
        {
            //printf("%d ",matriz_res[i][j]);
        }
        //printf("\n");
    }
    fclose(fichero1);
    fclose(fichero2);
    if (line)free(line);
    return(0);
}
