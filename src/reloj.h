#ifndef RELOJ_H
#define RELOJ_H
#include <stdbool.h>
#include <stdint.h>

void Reloj_init(unsigned ticksPorSegundo);
bool Reloj_getTiempoValido();

#endif // RELOJ_H
