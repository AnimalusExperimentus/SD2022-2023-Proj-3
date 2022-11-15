/*
*   Grupo 12
*   Duarte Lopes Pinheiro nº54475
*   Filipe Henriques nº55228
*   Márcio Moreira nº41972
*/

#ifndef _REQUEST_LINE_H
#define _REQUEST_LINE_H

#include <pthread.h>
#include "./request.h"


struct task_t{
    request_t *request;
    task_t *next;
};

void queue_init(); 

void queue_add_task(task_t *task) ;

task_t *queue_get_task() ;

#endif