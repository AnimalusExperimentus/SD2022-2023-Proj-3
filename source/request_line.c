/*
*   Grupo 12
*   Duarte Lopes Pinheiro nº54475
*   Filipe Henriques nº55228
*   Márcio Moreira nº41972
*/

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include "../include/request.h"
#include "../include/request_line.h"


pthread_mutex_t queue_lock; 
pthread_cond_t queue_not_empty;
task_t *queue_head = NULL;

void queue_init() { 
    queue_lock = PTHREAD_MUTEX_INITIALIZER;
    queue_not_empty = PTHREAD_COND_INITIALIZER;
} 


void queue_add_task(task_t *task) {
    pthread_mutex_lock(&queue_lock);
    if(queue_head==NULL) { /* Adiciona na cabeça da fila */
        queue_head = task; task->next=NULL;
    } else { /* Adiciona no fim da fila */
        task_t *tptr = queue_head;
        while(tptr->next != NULL) tptr=tptr->next;
        tptr->next=task; task->next=NULL;
    }
    pthread_cond_signal(&queue_not_empty); /* Avisa um bloqueado nessa condição */
    pthread_mutex_unlock(&queue_lock);
}





task_t *queue_get_task() {
    pthread_mutex_lock(&queue_lock);
    while(queue_head==NULL)
        pthread_cond_wait(&queue_not_empty, &queue_lock); /* Espera haver algo */
    task_t *task = queue_head; queue_head = task->next;
    pthread_mutex_unlock(&queue_lock);
    return task;
}