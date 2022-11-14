#ifndef _TREE_SKEL_PRIVATE_H
#define _TREE_SKEL_PRIVATE_H


/* Estrutura para dizer quantos valores e que podem ser feitos de cada vez /guarda os pedidos?
????
*/

struct op_proc{
    int max_proc;
    int *in_progress;
};

#endif