#include "unity.h"
#include "reloj.h"

void test_alInicioHoraNoValida(void)
{
    unsigned ticksPorSegundo = 1;
    Reloj_init(ticksPorSegundo);
    TEST_ASSERT_FALSE_MESSAGE(Reloj_getTiempoEsValido(),"TiempoValido debe ser false");
}
void test_alInicioHoraCero(void)
{
    TiempoBcd tiempo = {1,1,1,1,1,1};
    unsigned ticksPorSegundo = 1;
    Reloj_init(ticksPorSegundo);
    Reloj_getTiempo(&tiempo);
    TEST_ASSERT_EQUAL_UINT8_ARRAY((TiempoBcd){},tiempo,TiempoBcd_NUM_DIGITOS);
}
void test_laHoraInvalidaDebeAvanzar(void)
{
    TiempoBcd tiempo;
    unsigned ticksPorSegundo = 1;
    Reloj_init(ticksPorSegundo);
    Reloj_tick();
    Reloj_getTiempo(&tiempo);
    TEST_ASSERT_EQUAL_UINT8_ARRAY((TiempoBcd){[UNIDAD_SEGUNDO]=1},tiempo,TiempoBcd_NUM_DIGITOS);
}
void test_puedePonerseEnHora(void)
{
    static const TiempoBcd tiempoSet = {1,3,0,0,3,4};
    TiempoBcd tiempo;
    unsigned ticksPorSegundo = 1;
    Reloj_init(ticksPorSegundo);
    TEST_ASSERT_TRUE(Reloj_setTiempo(&tiempoSet));
    TEST_ASSERT_TRUE_MESSAGE(Reloj_getTiempoEsValido(),"Luego de poner en hora el tiempo debe ser valido");
    Reloj_getTiempo(&tiempo);
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(tiempoSet,tiempo,TiempoBcd_NUM_DIGITOS,"El tiempo establecido debe reflejarse en el reloj");
}


void test_avanzaUnSegundo(void)
{
    static const TiempoBcd tiempoInicial = {1,3,0,0,3,4};
    static const TiempoBcd tiempoFinal = {1,3,0,0,3,5};
    TiempoBcd tiempo;
    unsigned ticksPorSegundo = 1;
    Reloj_init(ticksPorSegundo);
    Reloj_setTiempo(&tiempoInicial);
    Reloj_tick();
    TEST_ASSERT_TRUE_MESSAGE(Reloj_getTiempoEsValido(),"Al avanzar el tiempo sigue siendo valido");
    Reloj_getTiempo(&tiempo);
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(tiempoFinal,tiempo,TiempoBcd_NUM_DIGITOS,"Debe avanzar");
}

void test_avanzaFronteras(void)
{
    static const struct Caso {
        TiempoBcd inicial; 
        TiempoBcd final;
        const char *mensaje;
    } tablaCasos[]={
        {.inicial = {1,3,0,0,3,9}, .final = {1,3,0,0,4,0}, .mensaje = "A 13:00:39 le sigue 13:00:40"},
        {.inicial = {1,3,0,0,5,9}, .final = {1,3,0,1,0,0}, .mensaje = "A 13:00:59 le sigue 13:01:00"},
    };
    const size_t numCasos = sizeof(tablaCasos)/sizeof(*tablaCasos);
    enum{TICKS_POR_SEGUNDO = 1};
    TiempoBcd tiempo;

    Reloj_init(TICKS_POR_SEGUNDO);

    for(size_t i=0;i<numCasos;++i){
        const struct Caso *const caso = tablaCasos + i;
        Reloj_setTiempo(&caso->inicial);
        Reloj_tick();
        Reloj_getTiempo(&tiempo);
        TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(caso->final,tiempo,TiempoBcd_NUM_DIGITOS,caso->mensaje);
    }
}