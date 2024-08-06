#ifndef ACCION_ALARMA_H
#define ACCION_ALARMA_H
#include "accion.h"

typedef struct AccionAlarma AccionAlarma;

AccionAlarma *AccionAlarma_new(void);
Accion *AccionAlarma_getAccion(AccionAlarma *self);
void AccionAlarma_delete(AccionAlarma *self);
unsigned AccionAlarma_vecesDisparada(AccionAlarma *self);

#endif
