#include "reloj.h"
#include <string.h>
//#define HACER_LOG
#ifdef HACER_LOG
#include <stdio.h>
#define LOG(s,...) printf(s __VA_OPT__(,) __VA_ARGS__)
#else
#define LOG(s,...)
#endif

static struct Reloj{
    unsigned tiempo;
    struct Alarma{
        unsigned tiempo;
        bool activada;
    } alarma;
    struct Alarma alarmaPospuesta;
    bool esValido;
    struct Prescaler{
        unsigned limite;
        unsigned cuenta;
    }prescaler;
    Accion *accionAlarma;
}self[1];

static void tiempoASegundos(unsigned *destino,const TiempoBcd *horaActual)
{
    *destino =  ((*horaActual)[UNIDAD_SEGUNDO] + (*horaActual)[DECENA_SEGUNDO] * 10)
              + ((*horaActual)[UNIDAD_MINUTO] + (*horaActual)[DECENA_MINUTO] * 10) * 60
              + ((*horaActual)[UNIDAD_HORA] + (*horaActual)[DECENA_HORA] * 10) * 3600;
}
static void segundosATiempo(TiempoBcd *destino,unsigned origen)
{
    (*destino)[UNIDAD_SEGUNDO] = origen % 10; origen /= 10;
    (*destino)[DECENA_SEGUNDO] = origen % 6;  origen /= 6;
    (*destino)[UNIDAD_MINUTO] = origen % 10;  origen /= 10;
    (*destino)[DECENA_MINUTO] = origen % 6;   origen /= 6;
    (*destino)[UNIDAD_HORA] = origen % 10;    origen /= 10;
    (*destino)[DECENA_HORA] = origen % 10;
}

void Reloj_init(unsigned ticksPorSegundo,Accion *accionAlarma)
{
    self->prescaler.limite = ticksPorSegundo - 1;
    self->prescaler.cuenta = 0;
    self->accionAlarma = accionAlarma;
}

bool Reloj_getTiempoEsValido(void)
{
    return self->esValido;
}

void Reloj_getTiempo(TiempoBcd *destino)
{
    segundosATiempo(destino,self->tiempo);
}


static void normalizaSegundos(unsigned *tiempo)
{
    *tiempo %= 24*3600;
}
static int comparaSegundos(const unsigned *a,const unsigned *b)
{
    return (*a == *b)?  0:
           (*a >  *b)?  1:
                       -1;
}
static bool Reloj_coincideAlarma(void)
{
    return !comparaSegundos(&self->tiempo,&self->alarma.tiempo);
}
static bool Reloj_coincideAlarmaPospuesta(void)
{
    return !comparaSegundos(&self->tiempo,&self->alarmaPospuesta.tiempo);
}

void Reloj_tick(void)
{
    if(self->prescaler.cuenta < self->prescaler.limite){
        self->prescaler.cuenta++;
        return;
    }
    self->prescaler.cuenta = 0;
    
    self->tiempo++;
    normalizaSegundos(&self->tiempo);

    if(!self->accionAlarma) return;

    if (self->alarma.activada && Reloj_coincideAlarma()){
        Accion_ejecuta(self->accionAlarma);
    }else if(self->alarmaPospuesta.activada && Reloj_coincideAlarmaPospuesta()){
        self->alarmaPospuesta.activada = false;
        Accion_ejecuta(self->accionAlarma);
    }
}

bool Reloj_setTiempo(const TiempoBcd *horaActual)
{
    tiempoASegundos(&self->tiempo,horaActual);
    self->esValido = true;
    return true;
}

bool Reloj_getAlarmaActivada(void)
{
    return self->alarma.activada;
}

bool Reloj_setTiempoAlarma(const TiempoBcd *tiempoAlarma)
{
    tiempoASegundos(&self->alarma.tiempo,tiempoAlarma);
    self->alarma.activada = true;
    return true;
}

void Reloj_desactivaAlarma(void)
{
    self->alarma.activada = false;
}

void Reloj_activaAlarma(void)
{
    self->alarma.activada = true;
}


void Reloj_posponAlarma(unsigned minutos)
{
    self->alarmaPospuesta.tiempo = self->tiempo + minutos*60;
    normalizaSegundos(&self->alarmaPospuesta.tiempo);
    self->alarmaPospuesta.activada = true;
}