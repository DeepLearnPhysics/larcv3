#include "dataformat.h"



void init_dataformat(pybind11::module m){
    init_dataformattypes(m);
    init_point(m);
    init_vertex(m);
    init_particle(m);
    init_imagemeta(m);
    init_bbox(m);
    init_tensor(m);
    init_voxel(m);
    init_eventid(m);
    init_eventbase(m);
    init_eventparticle(m);
    // init_eventsparsecluster(m);
    // init_eventsparsetensor(m);
    // init_eventtensor(m);
    init_iomanager(m);
}

