#ifndef ACCION_H
#define ACCION_H

typedef struct Accion Accion;

struct Accion{
    struct Accion_VT{
        void (*const ejecuta)(Accion *);
    }const *vptr_;
};

inline void Accion_ejecuta(Accion *self){
    self->vptr_->ejecuta(self);
}

#endif // ACCION_H
