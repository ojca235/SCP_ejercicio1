#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define N1 1000 //numero de filas de la matiz1
#define M1 1000 //numero de columnas de la matriz1
#define N2 1000 //numero de filas de la matiz2
#define M2 1000 //numero de columnas de la matriz2
int *matriz1 = NULL;
int *matriz2 = NULL;

FILE* fichero1;
FILE* fichero2;

void inicializar();
void escribirSobreFichero();
int main(int argc, char *argv[])
{
    srand(time(NULL)); 
    inicializar();
    fichero1 = fopen("matrizA.txt", "w");
    fichero2 = fopen("matrizB.txt", "w");
    if (fichero1==NULL || fichero2==NULL)
    {
        printf("error al intentar abrir el fichero\n");
        return -1;
    }
    escribirSobreFichero();
    //cierre de los ficheros creados
    fclose(fichero1);
    fclose(fichero2);
    printf("Proceso completado \n");
    free(matriz1);
    free(matriz2);
    return 0;
}
//Funcion que se encarga de reservar memoria para las matrices y ademas les da valores
void inicializar()
{   
    //Reserva de memoria
    matriz1=(int *)malloc(N1*M1*sizeof(int));
    matriz2=(int *)malloc(N2*M2*sizeof(int));
    int i,j;
    //Inicializacion de la matriz A
    for(i=0;i<N1;i++)
        for(j=0;j<M1;j++)
        {
            matriz1[i*M1+j] = -100 + rand() % 201;
           
        }
    //Inicializacion de la matriz B
    for(i=0;i<N2;i++)
        for(j=0;j<M2;j++)
        {
            matriz2[i*M2+j] = -100 + rand() % 201;
           
        }
}
//Función que se encarga de guardar las matrices en un fichero cada uno
void escribirSobreFichero()
{
    int i,j;
    fprintf(fichero1,"%d",N1);
    fprintf(fichero1,"\n");
    fprintf(fichero1,"%d",M1);
    fprintf(fichero1,"\n");
    fprintf(fichero2,"%d",N2);
    fprintf(fichero2,"\n");
    fprintf(fichero2,"%d",M2);
    fprintf(fichero2,"\n");
    //Guardar en el primer fichero la primera matriz
    for(i=0;i<N1;i++)
    {
        for(j=0;j<M1;j++)
        {
            fprintf(fichero1,"%d",matriz1[i*M1+j]);
            fprintf(fichero1,"\n");
        }
    }
    //Guardar en el segundo fichero la primera segunda
    for(i=0;i<N2;i++)
    {
        for(j=0;j<M2;j++)
        {
            fprintf(fichero2,"%d",matriz2[i*M2+j]);
            fprintf(fichero2,"\n");
        }
    }
}