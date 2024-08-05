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

void test_avanzaFronteraDecenaDeSegundo(void)
{
    static const TiempoBcd tiempoInicial = {1,3,0,0,3,9};
    static const TiempoBcd tiempoFinal = {1,3,0,0,4,0};
    TiempoBcd tiempo;
    unsigned ticksPorSegundo = 1;
    Reloj_init(ticksPorSegundo);
    Reloj_setTiempo(&tiempoInicial);
    Reloj_tick();
    Reloj_getTiempo(&tiempo);
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(tiempoFinal,tiempo,TiempoBcd_NUM_DIGITOS,"A las 9 unidades le sigue una decena");
}