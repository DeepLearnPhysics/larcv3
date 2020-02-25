#include "dataformat.h"

#include <pybind11/stl_bind.h>


template <class T>
void wrap_vector(pybind11::module m){
    std::string name = "VectorOf"+larcv3::as_string<T>();
    using Class = std::vector<T>;
    pybind11::class_<Class> vect(m, name.c_str());
    vect.def(pybind11::init<>());
    vect.def("clear", &Class::clear);
    vect.def("pop_back", &Class::pop_back);
    vect.def("__len__", [](const Class &v) { return v.size(); });
    vect.def("__iter__", [](Class &v) {
       return pybind11::make_iterator(v.begin(), v.end());
    }, pybind11::keep_alive<0, 1>(), pybind11::return_value_policy::reference); /* Keep vector alive while iterator is used */


    // std::string vecname = "VectorOf" + larcv3::as_string<T>();
    // pybind11::bind_vector<std::vector<T> >(m, vecname.c_str());


}

void init_dataformat(pybind11::module m){

    // Wrap vectors of various products:
    wrap_vector<larcv3::SparseTensor<2>>(m);
    wrap_vector<larcv3::SparseTensor<3>>(m);

    wrap_vector<larcv3::Tensor<1>>(m);
    wrap_vector<larcv3::Tensor<2>>(m);
    wrap_vector<larcv3::Tensor<3>>(m);
    wrap_vector<larcv3::Tensor<4>>(m);

    wrap_vector<larcv3::ImageMeta<1>>(m);
    wrap_vector<larcv3::ImageMeta<2>>(m);
    wrap_vector<larcv3::ImageMeta<3>>(m);
    wrap_vector<larcv3::ImageMeta<4>>(m);


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
    init_eventsparsecluster(m);
    init_eventsparsetensor(m);
    init_eventtensor(m);
    init_iomanager(m);
}

