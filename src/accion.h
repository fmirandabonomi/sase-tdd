#ifndef ACCION_H
#define ACCION_H

/**
 * @brief Interfaz abstracta que implementa una acción
 * (callback orientado a objetos)
 * 
 */
typedef struct Accion Accion;

struct Accion{
    struct Accion_VT{
        void (*const ejecuta)(Accion *);
    }const *vptr_;
};

/**
 * @brief Ejecuta una accion
 * 
 * @param self la accion
 */
inline void Accion_ejecuta(Accion *self){
    self->vptr_->ejecuta(self);
}

#endif // ACCION_H
