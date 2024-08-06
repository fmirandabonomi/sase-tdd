#include "accion_alarma.h"
#include "container_of.h"
#include <stdbool.h>
#include <assert.h>

#ifndef ACCION_ALARMA_MAX_INSTANCIAS
#define ACCION_ALARMA_MAX_INSTANCIAS 1
#endif

struct AccionAlarma{
    Accion accion;
    unsigned vecesActivada;
    bool usada;
};

AccionAlarma instancias[ACCION_ALARMA_MAX_INSTANCIAS]={};

static void AccionAlarma_accion(Accion *accion)
{
    AccionAlarma *const self = container_of(accion,AccionAlarma,accion);
    self->vecesActivada++;
}

AccionAlarma* AccionAlarma_new(void)
{
    static const struct Accion_VT tabla = {
        .ejecuta = AccionAlarma_accion
    };
    AccionAlarma *self = NULL;
    for (int i=0;i<ACCION_ALARMA_MAX_INSTANCIAS;++i){
        if (!instancias[i].usada){
            self = &instancias[i];
            break;
        }
    }

    self->accion.vptr_=&tabla;
    self->vecesActivada = 0;
    self->usada=true;
    return self;
}

void AccionAlarma_delete(AccionAlarma *self){
    if (self >= instancias && self < instancias + ACCION_ALARMA_MAX_INSTANCIAS ){
        self->usada=false;
    }
}

unsigned AccionAlarma_vecesDisparada(AccionAlarma *self)
{
    assert(self->usada);
    return self->vecesActivada;
}

Accion *AccionAlarma_getAccion(AccionAlarma *self)
{
    return &self->accion;
}