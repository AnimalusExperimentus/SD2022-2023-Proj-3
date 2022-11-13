/*
*   Grupo 12
*   Duarte Lopes Pinheiro nº54475
*   Filipe Henriques nº55228
*   Márcio Moreira nº41972
*/

#ifndef _REQUEST_H
#define _REQUEST_H

#include "./data.h";


struct request_t {
int op_n; //o número da operação
int op; //a operação a executar. op=0 se for um delete, op=1 se for um put
char* key; //a chave a remover ou adicionar
struct data_t *data; // os dados a adicionar em caso de put, ou NULL em caso de delete
//adicionar campo(s) necessário(s) para implementar fila do tipo produtor/consumidor
};

#endif