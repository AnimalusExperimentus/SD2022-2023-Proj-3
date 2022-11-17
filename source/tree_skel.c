/*
*   Grupo 12
*   Duarte Lopes Pinheiro nº54475
*   Filipe Henriques nº55228
*   Márcio Moreira nº41972
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "../include/sdmessage.pb-c.h"
#include "../include/tree.h"
#include "../include/tree_skel.h"
#include "../include/data.h"
#include "../include/entry.h"

struct op_proc {
    // maior identificador de ops de escrita ja concluidas
    int max_proc;
    // ops de escrita a serem atendidas pelos threads neste momento
    int *in_progress;
};
struct op_proc *proc_op;

struct request_t {
    int op_n;            //o número da operação
    int op;              //a operação a executar. op=0 se for um delete, op=1 se for um put
    char* key;           //a chave a remover ou adicionar
    struct data_t *data; // os dados a adicionar em caso de put, ou NULL em caso de delete
    struct request_t *next;
};
struct request_t *queue_head;

struct tree_t *tree;
// sempre que eh recebido pedido de escrita o main thread responde com o valor atual de
// last_assigned e de seguida incrementa-o
int last_assigned = 1;

pthread_t *threads;
int *thread_params;
int thread_num;
pthread_mutex_t queue_lock = PTHREAD_MUTEX_INITIALIZER; 
pthread_cond_t queue_not_empty = PTHREAD_COND_INITIALIZER;


/**/
void queue_add_request(struct request_t *request) {

    pthread_mutex_lock(&queue_lock);
    request->next = NULL;
    // adds to head of FIFO
    if(queue_head == NULL) {
        queue_head = request;
    // adds to end of FIFO
    } else { 
        struct request_t *tptr = queue_head;
        while(tptr->next != NULL) {tptr = tptr->next;}
        tptr->next = request;
    }
    pthread_cond_signal(&queue_not_empty);
    pthread_mutex_unlock(&queue_lock);
}


/**/
struct request_t *queue_get_request() {

    pthread_mutex_lock(&queue_lock);
    while(queue_head == NULL) {
        // wait for something
        pthread_cond_wait(&queue_not_empty, &queue_lock);
    }
    struct request_t *request = queue_head;
    queue_head = request->next;
    pthread_mutex_unlock(&queue_lock);
    
    return request;
}


/* Funcao da thread secundaria que vai processar pedidos de escrita
*/
void *process_request (void *params) {
    // TODO
    return NULL;
}


/* Verifica se a operacao identificada por op_n foi executada.
*/
int verify(int op_n) {

    if (op_n <= proc_op->max_proc) { return 0; } 
    for (int i = 0; i < thread_num; i++)
    {
        if (proc_op->in_progress[i] == op_n) { return 1; }
    }
    return -2;
}


/* Inicia o skeleton da árvore.
 * O main() do servidor deve chamar esta função antes de poder usar a
 * função invoke().
 * A funcao deve lancar N threads secundarias responsaveis por atender
 * pedidos de escrita na arvore;
 * Retorna 0 (OK) ou -1 (erro, por exemplo OUT OF MEMORY)
 */
int tree_skel_init(int N) {
    thread_num = N;
    
    tree = tree_create();
    if (tree == NULL) { return -1; }

    queue_head = NULL;
    
    // create and initialize proc_op
    proc_op = malloc(sizeof(struct op_proc));
    proc_op->in_progress = malloc((sizeof(int))*N);
    if (proc_op == NULL) {
        free(proc_op->in_progress);
        free(proc_op);
        return(-1);
    }
    proc_op->max_proc = 0;
    for(int i = 0; i < N; i++) { proc_op->in_progress[i] = 0; }

    // init threads
    threads = malloc(sizeof(pthread_t)*N);
    thread_params = malloc(sizeof(int)*N);

    // create threads
    for (int i=  0; i < N; i++){
		thread_params[i] = (i+1);
		if (pthread_create(&threads[i], NULL, &process_request, (void *) &thread_params[i]) != 0) {
			printf("Erro na criacao da thread %d.\n", i);
			return(-1);
		}
	}

    return 0;
}

/* Liberta toda a memória e recursos alocados pela função tree_skel_init.
 */
void tree_skel_destroy() {
    struct request_t *req;
    
    if(tree != NULL) {
        tree_destroy(tree);
    }

    if(queue_head != NULL) {
        while(queue_head->next != NULL) {
            free(queue_head->key);
            data_destroy(queue_head->data);
            free(queue_head);
            req=queue_head->next;
            free(queue_head);
            queue_head=req;
        }
    }
}

/* Executa uma operação na árvore (indicada pelo opcode contido em msg)
 * e utiliza a mesma estrutura message_t para devolver o resultado.
 * Retorna 0 (OK) ou -1 (erro, por exemplo, árvore nao incializada)
*/
int invoke(MessageT *msg) {
    
    if(msg==NULL || msg->opcode < 0 || msg->c_type < 0 || msg->opcode > MESSAGE_T__OPCODE__OP_ERROR || msg->c_type > MESSAGE_T__C_TYPE__CT_NONE) {
        return -1;
    }
    switch(msg->opcode) {
        case MESSAGE_T__OPCODE__OP_SIZE:
        {
            msg->opcode=MESSAGE_T__OPCODE__OP_SIZE+1;
            msg->c_type=MESSAGE_T__C_TYPE__CT_RESULT;
            msg->size=tree_size(tree);
            return 0;
        }
        case MESSAGE_T__OPCODE__OP_HEIGHT:
        {
            msg->opcode=MESSAGE_T__OPCODE__OP_HEIGHT+1;
            msg->c_type=MESSAGE_T__C_TYPE__CT_RESULT;
            msg->size=tree_height(tree);
            return 0;
        }
        case MESSAGE_T__OPCODE__OP_DEL:
        {
            int opnumber = last_assigned;
            last_assigned++;
            
            char* key_d = malloc(msg->size);
            memset(key_d, '\0', msg->size);
            memcpy(key_d, msg->key, msg->size);

            struct request_t *req;
            req = malloc(sizeof(struct request_t));
            req->op_n = opnumber;
            req->op = 0;
            req->key = key_d;

            queue_add_request(req);
            msg->opcode=MESSAGE_T__OPCODE__OP_DEL+1;
            msg->c_type=MESSAGE_T__C_TYPE__CT_RESULT;
            msg->op_n=opnumber;
            return 0;
        }
        case MESSAGE_T__OPCODE__OP_GET:
        {
            char *key = malloc(msg->size);
            memset(key, '\0', msg->size);
            memcpy(key, msg->key, msg->size);

            struct data_t *t = tree_get(tree, key);
            free(key);

            if(t == NULL) {
                msg->opcode=MESSAGE_T__OPCODE__OP_GET+1;
                msg->c_type=MESSAGE_T__C_TYPE__CT_VALUE;
                msg->data.data = NULL;
                msg->data.len = 0;
                msg->size = 0;
            } else {
                msg->opcode=MESSAGE_T__OPCODE__OP_GET+1;
                msg->c_type=MESSAGE_T__C_TYPE__CT_VALUE;
                
                msg->data.data = malloc(t->datasize);
                msg->data.len = t->datasize;
                msg->size = t->datasize;
                memcpy(msg->data.data, t->data, msg->size);
            }
            data_destroy(t);
            return 0;
        }
        case MESSAGE_T__OPCODE__OP_PUT:
        {
            int opnumber = last_assigned;
            last_assigned++;

            struct data_t *new_data = data_create((int)msg->data.len);
            memcpy(new_data->data, msg->data.data, msg->data.len);
            char* temp_key = malloc(msg->size);
            memcpy(temp_key, msg->key, msg->size);
            
            struct request_t *req;
            req = malloc(sizeof(struct request_t));
            req->op_n = opnumber;
            req->op = 1;
            req->key = temp_key;
            req->data = new_data;

            queue_add_request(req);
            msg->opcode=MESSAGE_T__OPCODE__OP_PUT+1;
            msg->c_type=MESSAGE_T__C_TYPE__CT_RESULT;
            msg->op_n=opnumber;
            return 0;
        }
        case MESSAGE_T__OPCODE__OP_GETKEYS:
        {
            char** kk = tree_get_keys(tree);

            if(kk != NULL){
                msg->opcode=MESSAGE_T__OPCODE__OP_GETKEYS+1;
                msg->c_type=MESSAGE_T__C_TYPE__CT_KEYS;

                int size = 0;
                for (int i = 0; kk[i] != NULL; i++) {
                    size++;
                }
                
                msg->n_keys = size;
                msg->keys = kk;

            }else{ 
                msg->opcode=MESSAGE_T__OPCODE__OP_ERROR;
                msg->c_type=MESSAGE_T__C_TYPE__CT_NONE;
            }
            return 0;
        }
        case MESSAGE_T__OPCODE__OP_GETVALUES:
        {
            void **val = tree_get_values(tree);

            if (val != NULL) {
                msg->opcode=MESSAGE_T__OPCODE__OP_GETVALUES+1;
                msg->c_type=MESSAGE_T__C_TYPE__CT_VALUES;

                
                int size = 0;
                for (int i = 0; val[i] != NULL; i++) {
                    size++;
                }

                msg->n_vals = size;
                msg->vals = malloc(sizeof(MessageT__Value *)*size);
                
                for (int i = 0; i < size; i++)
                {
                    struct data_t *d = val[i];

                    MessageT__Value *v;
                    v = malloc(sizeof(MessageT__Value));
                    message_t__value__init(v);
                    v->data.len = d->datasize;
                    v->data.data = malloc(d->datasize);
                    v->data.data = memcpy(v->data.data, d->data, d->datasize);
                    msg->vals[i] = v;
                }

            } else {
                msg->opcode=MESSAGE_T__OPCODE__OP_ERROR;
                msg->c_type=MESSAGE_T__C_TYPE__CT_NONE;
            }
            return 0;
        }
        case MESSAGE_T__OPCODE__OP_VERIFY:
        {
            int r = verify(msg->op_n);
            if (r == 0 || r == 1) {
                msg->opcode=MESSAGE_T__OPCODE__OP_PUT+1;
                msg->c_type=MESSAGE_T__C_TYPE__CT_RESULT;
                msg->op_n=r;
            } else if (r == -2) {
                msg->opcode=MESSAGE_T__OPCODE__OP_ERROR;
                msg->c_type=MESSAGE_T__C_TYPE__CT_NONE;
                msg->op_n=r;
            }
            return 0;
        }
        // so compiler doesn't scream at us
        case MESSAGE_T__OPCODE__OP_BAD:
        {
            return 0;
        }
        case MESSAGE_T__OPCODE__OP_ERROR:
        {
            return 0;
        }
        case _MESSAGE_T__OPCODE_IS_INT_SIZE:
        {
            return 0;
        }

    }
    return -1;
}
