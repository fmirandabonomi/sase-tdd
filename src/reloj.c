#include "reloj.h"

static struct Reloj{
    TiempoBcd tiempo;
    bool esValido;
}self[1];

void Reloj_init(unsigned ticksPorSegundo)
{
    (void) ticksPorSegundo;
}

bool Reloj_getTiempoEsValido(void)
{
    return self->esValido;
}

void Reloj_getTiempo(TiempoBcd *destino)
{
    for (int i=0;i<TiempoBcd_NUM_DIGITOS;++i) (*destino)[i]=self->tiempo[i];
}

void Reloj_tick(void)
{
    self->tiempo[UNIDAD_SEGUNDO]=1;   
}

bool Reloj_setTiempo(const TiempoBcd *horaActual)
{
    for(int i=0;i<TiempoBcd_NUM_DIGITOS;++i)self->tiempo[i]=(*horaActual)[i];
    self->esValido = true;
    return true;
}

