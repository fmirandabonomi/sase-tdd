#include "reloj.h"
#include <string.h>
//#define HACER_LOG
#ifdef HACER_LOG
#include <stdio.h>
#define LOG(s,...) printf(s __VA_OPT__(,) __VA_ARGS__)
#define LOG_TIEMPO(t)     do{\
        uint8_t *const base = (t);\
        LOG("%02d:%02d:%02d",\
        base[DECENA_HORA]*10+base[UNIDAD_HORA],\
        base[DECENA_MINUTO]*10+base[UNIDAD_MINUTO],\
        base[DECENA_SEGUNDO]*10+base[UNIDAD_SEGUNDO]);\
}while(0)
#else
#define LOG(s,...)
#define LOG_TIEMPO(t)
#endif

static struct Reloj{
    TiempoBcd tiempo;
    struct Alarma{
        TiempoBcd tiempo;
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

static void copiaTiempBcd(TiempoBcd *destino,const TiempoBcd *horaActual)
{
    for(int i=0;i<TiempoBcd_NUM_DIGITOS;++i)(*destino)[i]=(*horaActual)[i];
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
    copiaTiempBcd(destino,&self->tiempo);
}


static void normalizaDigito(uint8_t *const digito,uint8_t *const siguiente,const uint8_t pesoSiguiente)
{
    while(*digito >= pesoSiguiente){
        *digito -= pesoSiguiente;
        *siguiente += 1;
    }
}
static void normalizaTiempo(TiempoBcd *tiempo)
{
    uint8_t *const base = *tiempo;

    normalizaDigito(base + UNIDAD_SEGUNDO,base + DECENA_SEGUNDO,10);
    normalizaDigito(base + DECENA_SEGUNDO,base + UNIDAD_MINUTO, 6);
    normalizaDigito(base + UNIDAD_MINUTO, base + DECENA_MINUTO, 10);
    normalizaDigito(base + DECENA_MINUTO, base + UNIDAD_HORA,   6);
    
    normalizaDigito(base + UNIDAD_HORA, base + DECENA_HORA, 10);

    while(   base[DECENA_HORA] > 2 
          || (base[DECENA_HORA] == 2 && base[UNIDAD_HORA] > 3)){
        base[UNIDAD_HORA] += 10 - 4;
        base[DECENA_HORA] -= 3;
        normalizaDigito(base + UNIDAD_HORA, base + DECENA_HORA, 10);
    }

}
static int comparaTiempos(TiempoBcd *a,TiempoBcd *b)
{
    return memcmp(*a,*b,TiempoBcd_NUM_DIGITOS);
}
static bool Reloj_coincideAlarma(void)
{
    return !comparaTiempos(&self->tiempo,&self->alarma.tiempo);
}
static bool Reloj_coincideAlarmaPospuesta(void)
{
    return !comparaTiempos(&self->tiempo,&self->alarmaPospuesta.tiempo);
}

void Reloj_tick(void)
{
    if(self->prescaler.cuenta < self->prescaler.limite){
        self->prescaler.cuenta++;
        return;
    }
    self->prescaler.cuenta = 0;
    
    self->tiempo[UNIDAD_SEGUNDO]++;
    normalizaTiempo(&self->tiempo);

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
    copiaTiempBcd(&self->tiempo,horaActual);
    self->esValido = true;
    return true;
}

bool Reloj_getAlarmaActivada(void)
{
    return self->alarma.activada;
}

bool Reloj_setTiempoAlarma(const TiempoBcd *tiempoAlarma)
{
    copiaTiempBcd(&self->alarma.tiempo,tiempoAlarma);
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
    copiaTiempBcd(&self->alarmaPospuesta.tiempo,&self->tiempo);

    minutos %= 24*60;
    self->alarmaPospuesta.tiempo[UNIDAD_MINUTO] += minutos%10;
    minutos/=10;
    self->alarmaPospuesta.tiempo[DECENA_MINUTO] += minutos%6;
    minutos/=6;
    self->alarmaPospuesta.tiempo[UNIDAD_HORA] += minutos;
    
    normalizaTiempo(&self->alarmaPospuesta.tiempo);
    
    self->alarmaPospuesta.activada = true;

    LOG("Alarma pospuesta para ");
    LOG_TIEMPO(self->alarmaPospuesta.tiempo);
    LOG("\n");

}