/*
*   Grupo 12
*   Duarte Lopes Pinheiro nº54475
*   Filipe Henriques nº55228
*   Márcio Moreira nº41972
*/

#include "../include/network_client.h"
#include "../include/client_stub_private.h"
#include "../include/sdmessage.pb-c.h"
#include "../include/data.h"
#include <stdlib.h>
#include <arpa/inet.h>
#include <stdio.h>
#include "string.h"


/* Função para estabelecer uma associação entre o cliente e o servidor,
 * em que address_port é uma string no formato <hostname>:<port>.
 * Retorna NULL em caso de erro.
 */
struct rtree_t *rtree_connect(const char *address_port) {
    
    if(address_port == NULL){
        return NULL;
    }

    struct rtree_t *rTree;
    
    // Create and allocate rtree
    rTree = malloc(sizeof(struct rtree_t));
    if( (rTree == NULL)) {return NULL;} 
    
    // get address and port
    char copAdr [strlen(address_port)];
    strcpy(copAdr,address_port);
    char *adr = strtok(copAdr, ":");
    char* ptr;
    int port = (int) strtol( strtok(NULL,"\0"), &ptr, 10);

    // save socket data in rtree
    rTree->server.sin_family = AF_INET;
    rTree->server.sin_port = htons(port);
    rTree->server.sin_addr.s_addr = inet_addr(adr);

    // connect to server
    if ((network_connect(rTree)) == -1) {
        free(rTree);
        return NULL;
    }

    return rTree;
}


/* Termina a associação entre o cliente e o servidor, fechando a
 * ligação com o servidor e libertando toda a memória local.
 * Retorna 0 se tudo correr bem e -1 em caso de erro.
 */
int rtree_disconnect(struct rtree_t *rtree) {
    
    if (rtree == NULL) {
        return -1;
    }
    if (network_close(rtree) == -1) {
        return -1;
    }
    free(rtree);
    return 0;
}


//-------------------------------------------------------------------------------
/* Função para adicionar um elemento na árvore.
 * Se a key já existe, vai substituir essa entrada pelos novos dados.
 * Devolve 0 (ok, em adição/substituição) ou -1 (problemas).
 */
int rtree_put(struct rtree_t *rtree, struct entry_t *entry) {

    // create and fill message with entry
    MessageT msg = MESSAGE_T__INIT;
    msg.opcode = MESSAGE_T__OPCODE__OP_PUT;
    msg.c_type = MESSAGE_T__C_TYPE__CT_ENTRY;

    msg.key = malloc(strlen(entry->key)+1);
    memcpy(msg.key, entry->key, strlen(entry->key)+1);
    msg.size = strlen(entry->key)+1;
    msg.data.len = entry->value->datasize;
    msg.data.data = malloc(entry->value->datasize);
    memcpy(msg.data.data, entry->value->data, entry->value->datasize);

    // send and receive response
    MessageT *msg_rcv = network_send_receive(rtree, &msg);
    free(msg.key);
    free(msg.data.data);
    if (msg_rcv == NULL) {
        message_t__free_unpacked(msg_rcv, NULL);
        return -1;
    }
    if (msg_rcv->opcode == MESSAGE_T__OPCODE__OP_ERROR) {
        message_t__free_unpacked(msg_rcv, NULL);
        return -1;
    }
    

    int op_n = msg_rcv->op_n;
    message_t__free_unpacked(msg_rcv, NULL);
    return op_n;
}


/* Função para obter um elemento da árvore.
 * Em caso de erro, devolve NULL.
 */
struct data_t *rtree_get(struct rtree_t *rtree, char *key) {

    // create and fill message with entry
    MessageT msg = MESSAGE_T__INIT;
    msg.opcode = MESSAGE_T__OPCODE__OP_GET;
    msg.c_type = MESSAGE_T__C_TYPE__CT_KEY;

    int len = strlen(key)+1;
    msg.key = malloc(len);
    memcpy(msg.key, key, len);
    msg.size = len;

    // send msg and receive response
    MessageT *msg_rcv = network_send_receive(rtree, &msg);
    free(msg.key);
    if (msg_rcv == NULL) {
        message_t__free_unpacked(msg_rcv, NULL);
        return NULL;
    }
    
    if (msg_rcv->opcode == MESSAGE_T__OPCODE__OP_ERROR) {
        message_t__free_unpacked(msg_rcv, NULL);
        return NULL;
    }

    // create data struct to return from msg
    struct data_t *d;
    if (msg_rcv->data.data == NULL) {
        d = NULL;
    } else {
        d = data_create(msg_rcv->size);
        memcpy(d->data, msg_rcv->data.data, d->datasize);
    }

    message_t__free_unpacked(msg_rcv, NULL);
    return d;
}


/* Função para remover um elemento da árvore. Vai libertar
 * toda a memoria alocada na respetiva operação rtree_put().
 * Devolve: 0 (ok), -1 (key not found ou problemas).
 */
int rtree_del(struct rtree_t *rtree, char *key){

    // create message
    MessageT msg = MESSAGE_T__INIT;
    msg.opcode = MESSAGE_T__OPCODE__OP_DEL;
    msg.c_type = MESSAGE_T__C_TYPE__CT_KEY;

    int len = strlen(key)+1;
    msg.key = malloc(len);
    memcpy(msg.key, key, len);
    msg.size = len;

    // send msg and receive response
    MessageT *msg_rcv = network_send_receive(rtree, &msg);
    free(msg.key);

    if (msg_rcv == NULL) {
        message_t__free_unpacked(msg_rcv, NULL);
        return -1;
    }
    if (msg_rcv->opcode == MESSAGE_T__OPCODE__OP_ERROR) {
        message_t__free_unpacked(msg_rcv, NULL);
        return -1;
    }

    int op_n = msg_rcv->op_n;
    message_t__free_unpacked(msg_rcv, NULL);
    return op_n;
 }


/* Devolve o número de elementos contidos na árvore.
 */
int rtree_size(struct rtree_t *rtree) {

    MessageT msg = MESSAGE_T__INIT;
    msg.opcode = MESSAGE_T__OPCODE__OP_SIZE;
    msg.c_type = MESSAGE_T__C_TYPE__CT_NONE;

    MessageT *msg_rcv = network_send_receive(rtree, &msg);
    if (msg_rcv == NULL) {
        message_t__free_unpacked(msg_rcv, NULL);
        return -1;
    }
    if (msg_rcv->opcode == MESSAGE_T__OPCODE__OP_ERROR) {
        message_t__free_unpacked(msg_rcv, NULL);
        return -1;
    }
    
    int size = msg_rcv->size;

    message_t__free_unpacked(msg_rcv, NULL);
    return size;
}


/* Função que devolve a altura da árvore.
 */
int rtree_height(struct rtree_t *rtree) {

    MessageT msg = MESSAGE_T__INIT;
    msg.opcode = MESSAGE_T__OPCODE__OP_HEIGHT;
    msg.c_type = MESSAGE_T__C_TYPE__CT_NONE;

    MessageT *msg_rcv = network_send_receive(rtree, &msg);
    if (msg_rcv == NULL) {
        message_t__free_unpacked(msg_rcv, NULL);
        return -1;
    }
    if (msg_rcv->opcode == MESSAGE_T__OPCODE__OP_ERROR) {
        message_t__free_unpacked(msg_rcv, NULL);
        return -1;
    }
    
    int size = msg_rcv->size;

    message_t__free_unpacked(msg_rcv, NULL);
    return size;
}


/* Devolve um array de char* com a cópia de todas as keys da árvore,
 * colocando um último elemento a NULL.
 */
char **rtree_get_keys(struct rtree_t *rtree) {

    MessageT msg = MESSAGE_T__INIT;
    msg.opcode = MESSAGE_T__OPCODE__OP_GETKEYS;
    msg.c_type = MESSAGE_T__C_TYPE__CT_NONE;

    MessageT *msg_rcv = network_send_receive(rtree, &msg);
    if (msg_rcv == NULL) {
        message_t__free_unpacked(msg_rcv, NULL);
        return NULL;
    }
    if (msg_rcv->opcode == MESSAGE_T__OPCODE__OP_ERROR) {
        message_t__free_unpacked(msg_rcv, NULL);
        return NULL;
    }

    // copy keys to local array to return
    int size = msg_rcv->n_keys;
    char **key_arr = malloc(sizeof(char *)*(size+1));
    key_arr[size] = NULL;
    
    for (int i = 0; i < size; i++) {
        int len = strlen(msg_rcv->keys[i]) + 1;
        key_arr[i] = malloc(len);
        memcpy(key_arr[i], msg_rcv->keys[i], len);
    }
    
    message_t__free_unpacked(msg_rcv, NULL);
    return key_arr;
}


/* Devolve um array de void* com a cópia de todas os values da árvore,
 * colocando um último elemento a NULL.
 */
void **rtree_get_values(struct rtree_t *rtree) {

    MessageT msg = MESSAGE_T__INIT;
    msg.opcode = MESSAGE_T__OPCODE__OP_GETVALUES;
    msg.c_type = MESSAGE_T__C_TYPE__CT_NONE;

    MessageT *msg_rcv = network_send_receive(rtree, &msg);
    if (msg_rcv == NULL) {
        message_t__free_unpacked(msg_rcv, NULL);
        return NULL;
    }
    if (msg_rcv->opcode == MESSAGE_T__OPCODE__OP_ERROR) {
        message_t__free_unpacked(msg_rcv, NULL);
        return NULL;
    }

    // build result array
    int n_values = msg_rcv->n_vals;
    void  **result_arr = malloc(sizeof(struct data_t *)*(n_values+1));
    result_arr[n_values] = NULL;
    for (int i = 0; i < n_values; i++) {

        struct data_t *d = malloc(sizeof(struct data_t));
        d->datasize = msg_rcv->vals[i]->data.len;
        d->data = malloc((sizeof(uint8_t *))*d->datasize);
        memcpy(d->data, msg_rcv->vals[i]->data.data, d->datasize);
        result_arr[i] = d;
    }
    
    message_t__free_unpacked(msg_rcv, NULL);
    return result_arr;
}


/* Verifica se a operacao identificada por op_n foi executada
*/
int rtree_verify(struct rtree_t *rtree, int op_n) {

    MessageT msg = MESSAGE_T__INIT;
    msg.opcode = MESSAGE_T__OPCODE__OP_VERIFY;
    msg.c_type = MESSAGE_T__C_TYPE__CT_RESULT;
    msg.op_n = op_n;

    MessageT *msg_rcv = network_send_receive(rtree, &msg);
    if (msg_rcv == NULL) {
        message_t__free_unpacked(msg_rcv, NULL);
        return  -1;
    }

    if (msg_rcv->opcode == MESSAGE_T__OPCODE__OP_ERROR) {
        message_t__free_unpacked(msg_rcv, NULL);
        return -2;
    }

    int r = msg_rcv->op_n;
    message_t__free_unpacked(msg_rcv, NULL);
    return r;
}