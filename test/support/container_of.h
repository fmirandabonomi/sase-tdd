#ifndef CONTAINER_OF_H
#define CONTAINER_OF_H
#include <stddef.h>
#define container_of(puntero,tipo,miembro) ((tipo*)((char*)(1?(puntero):&((tipo*)0)->miembro)-offsetof(tipo,miembro)))

#endif
