#include "sbnd_imagemod.h"

#include "ParentParticleSeg.h"

void init_sbnd_imagemod(pybind11::module m){
    init_parent_particle_seg(m);
}
