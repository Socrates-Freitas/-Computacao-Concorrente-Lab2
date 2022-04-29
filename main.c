#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

#define VALOR_INICIAL_MATRIZ_ENTRADA 1;
#define VALOR_INICIAL_MATRIZ_RESULTADO 0;



typedef struct {
    int id, dimensao;
    int numThreads;

} Args;


float *matriz1;
float *matriz2;
float *matrizResultado;


void exibirMatriz(float *matriz, int dimensao){
    for(int i =0; i < dimensao; i++){
        for(int j =0; j < dimensao; j++){
            printf("%.1f ",matriz[i*dimensao + j]);
        }
        puts("");
    }
}



void inicializarMatrizes(int dimensao){
    for(int i =0; i < dimensao; i++){// para cada linha
        for(int j = 0; j < dimensao; j++){ // para cada coluna
            matriz1[dimensao * i + j] = VALOR_INICIAL_MATRIZ_ENTRADA;
            matriz2[dimensao * i + j] = VALOR_INICIAL_MATRIZ_ENTRADA;
            matrizResultado[dimensao * i + j] = VALOR_INICIAL_MATRIZ_RESULTADO;
        }
    }
}

void multiplicarMatrizesSequencial(int dimensao){
    for(int i = 0; i < dimensao; i++){
        for(int j = 0; j < dimensao; j++){
            for(int k =0; k < dimensao; k++){
                matrizResultado[i*dimensao + j] += matriz1[i*dimensao + j] * matriz2[i * dimensao + j];
            }
        }
    }
}

void *multiplicarMatrizesConcorrente(void * arg){
    Args *argumento = (Args*) arg;

    for(int i = argumento->id; i < argumento->dimensao; i +=  argumento->numThreads){
        
        for(int j = 0; j < argumento->dimensao; j++){
            
            for(int k = 0; k < argumento->dimensao; k++){
                matrizResultado[i* argumento->dimensao + j] += matriz1[i* argumento->dimensao + k] * matriz2[ k * argumento->dimensao + j];
            }
        }
    }

    pthread_exit(NULL);
}

void verificarResultadoFinal(int dimensao){
    for(int i = 0; i < dimensao; i++){
        for(int j = 0; j < dimensao; j++){
            if(matrizResultado[i] != dimensao){
                printf("Resultado Inesperado na iteração %d. Esperado: %d. Recebido: %.1f\n",i,dimensao, matrizResultado[i]);
                return;
            }
        }
    }
    puts("A matriz de resultado está correta!");
}

int main(int argc,char * argv[]){

    int dimensao, numThreads;
    pthread_t *tid; // identificadores
    Args *argumentos;
    
    float inicio, fim, delta; 

     // Leitura e avaliação dos dados de entrada

    if(argc < 3){
        puts("Digite <Numero de Threads> <Dimensão da Matriz>");
        return 1;
    }

    numThreads = atoi(argv[1]);
    dimensao = atoi(argv[2]); 

     // Alocação dinamica de memoria
    matriz1 = (float *) malloc(sizeof(float) * dimensao * dimensao);
    if(matriz1 == NULL){
         printf("Erro Malloc!\n");
         return 2;
    }
    
    matriz2 = (float *) malloc(sizeof(float) * dimensao * dimensao);
    if(matriz2 == NULL){
         printf("Erro Malloc!\n");
         return 2;
    }

    matrizResultado = (float *) malloc(sizeof(float) * dimensao * dimensao);
    if(matrizResultado == NULL){ 
        printf("Erro Malloc!\n");
        return 2;
    }

    tid = (pthread_t *) malloc(sizeof(pthread_t) * numThreads);
    if(tid == NULL){
         printf("Erro Malloc!\n");
         return 2;
    }
    argumentos = (Args *) malloc(sizeof(Args) * numThreads);
    if(argumentos == NULL){
         printf("Erro Malloc!\n");
         return 2;
    }

     // Iniclialização das Matrizes

    inicializarMatrizes(dimensao);


/*
    puts("Vetor 1");
    exibirMatriz(matriz1, dimensao);
    puts("");
    puts("Vetor 2");
    exibirMatriz(matriz2, dimensao);
    puts("");
    puts("Vetor Resultado");
    exibirMatriz(matrizResultado, dimensao);

*/

    // Multiplicação de matrizes...

    GET_TIME(inicio); // Começando a registrar tempo de execução sequencial
    multiplicarMatrizesSequencial(dimensao);
    GET_TIME(fim);
    delta = fim - inicio;
    printf("Tempo da execução sequencial (dimensão: %d): %lfS\n", dimensao, delta);

    inicializarMatrizes(dimensao); // resetando matrizes

    GET_TIME(inicio); // Começando a registrar tempo de execução concorrente

    for(int i = 0; i < numThreads; i++){
        (argumentos + i)->dimensao = dimensao;
        (argumentos + i)->id = i;
        (argumentos + i)->numThreads = numThreads;
    
        if(pthread_create((tid + i),NULL, multiplicarMatrizesConcorrente,(void*) (argumentos+i) )){
            printf("Erro Pthread_Create!\n");
            break;
        }
    }

    for(int i = 0; i < numThreads; i++){
        if(pthread_join(tid[i],NULL)){
             printf("ERRO Pthread_join()! \n");
             return 2;
        }
    }

    GET_TIME(fim);
    delta = fim - inicio;
    printf("Tempo da execução concorrente (dimensão: %d | threads: %d ): %lfS\n", dimensao, numThreads, delta);


    // puts("Vetor Resultado");

    verificarResultadoFinal(dimensao);
    
   //  exibirMatriz(matrizResultado, dimensao);


     // liberação de memória

    free(matriz1);
    free(matriz2);
    free(matrizResultado);

    return 0;
}

