#include "reloj.h"

static struct Reloj{
    TiempoBcd tiempo;
}self[1];

void Reloj_init(unsigned ticksPorSegundo)
{
    *self = (struct Reloj){};
    (void) ticksPorSegundo;
}

bool Reloj_getTiempoEsValido(void)
{
    return false;
}

void Reloj_getTiempo(TiempoBcd *destino)
{
    for (int i=0;i<TiempoBcd_NUM_DIGITOS;++i) (*destino)[i]=self->tiempo[i];
}

void Reloj_tick(void)
{
    self->tiempo[UNIDAD_SEGUNDO]=1;   
}