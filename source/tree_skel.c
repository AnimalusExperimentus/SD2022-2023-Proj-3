/*
*   Grupo 12
*   Duarte Lopes Pinheiro nº54475
*   Filipe Henriques nº55228
*   Márcio Moreira nº41972
*/

#include "../include/sdmessage.pb-c.h"
#include "../include/tree.h"
#include "../include/tree_skel.h"
#include "../include/data.h"
#include "../include/entry.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct tree_t *tree;


/* Inicia o skeleton da árvore.
 * O main() do servidor deve chamar esta função antes de poder usar a
 * função invoke(). 
 * Retorna 0 (OK) ou -1 (erro, por exemplo OUT OF MEMORY)
 */
int tree_skel_init() {
   
   tree = tree_create();
    if(tree == NULL) {
        return -1;
    }
    return 0;
}

/* Liberta toda a memória e recursos alocados pela função tree_skel_init.
 */
void tree_skel_destroy() {
    
    if(tree != NULL) {
        tree_destroy(tree);
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

            msg->opcode=MESSAGE_T__OPCODE__OP_SIZE+1;
            msg->c_type=MESSAGE_T__C_TYPE__CT_RESULT;
            msg->size=tree_size(tree);
            return 0;

        case MESSAGE_T__OPCODE__OP_HEIGHT:
            
            msg->opcode=MESSAGE_T__OPCODE__OP_HEIGHT+1;
            msg->c_type=MESSAGE_T__C_TYPE__CT_RESULT;
            msg->size=tree_height(tree);
            return 0;

        case MESSAGE_T__OPCODE__OP_DEL:

            char* key_d = malloc(msg->size);
            memset(key_d, '\0', msg->size);
            memcpy(key_d, msg->key, msg->size);
            
            int i = tree_del(tree, key_d);
            free(key_d);
            if(i == 0) {
                msg->opcode=MESSAGE_T__OPCODE__OP_DEL+1;
            }else {
                msg->opcode=MESSAGE_T__OPCODE__OP_ERROR;
            }
            msg->c_type=MESSAGE_T__C_TYPE__CT_NONE;
            return 0;

        case MESSAGE_T__OPCODE__OP_GET:

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

        case MESSAGE_T__OPCODE__OP_PUT:
            
            struct data_t *new_data = data_create((int)msg->data.len);
            memcpy(new_data->data, msg->data.data, msg->data.len);
            char* temp_key = malloc(msg->size);
            memcpy(temp_key, msg->key, msg->size);
            
            int r = tree_put(tree, temp_key, new_data);
            data_destroy(new_data);
            free(temp_key);

            if(r == 0){
                msg->opcode=MESSAGE_T__OPCODE__OP_PUT+1;
            }else{
                msg->opcode=MESSAGE_T__OPCODE__OP_ERROR;
            }
            msg->c_type=MESSAGE_T__C_TYPE__CT_NONE;
            return 0;

        case MESSAGE_T__OPCODE__OP_GETKEYS:

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

        case MESSAGE_T__OPCODE__OP_GETVALUES:
            
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
            
        // so compiler doesn't scream at us
        case MESSAGE_T__OPCODE__OP_BAD:
            return 0;
        case MESSAGE_T__OPCODE__OP_ERROR:
            return 0;
        case _MESSAGE_T__OPCODE_IS_INT_SIZE:
            return 0;

    }
    return -1;
}
