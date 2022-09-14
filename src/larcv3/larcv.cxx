#include "larcv3/core/base/base.h"
#include "larcv3/core/dataformat/dataformat.h"
#include "larcv3/core/processor/processor.h"
#include "larcv3/app/queueio/queueio.h"
#include "larcv3/app/imagemod/imagemod.h"
#include "larcv3/app/sbnd_imagemod/sbnd_imagemod.h"


PYBIND11_MAKE_OPAQUE(std::vector<larcv3::SparseTensor<2> >);
PYBIND11_MAKE_OPAQUE(std::vector<larcv3::SparseTensor<3> >);
PYBIND11_MAKE_OPAQUE(std::vector<larcv3::Tensor      <1> >);
PYBIND11_MAKE_OPAQUE(std::vector<larcv3::Tensor      <2> >);
PYBIND11_MAKE_OPAQUE(std::vector<larcv3::Tensor      <3> >);
PYBIND11_MAKE_OPAQUE(std::vector<larcv3::Tensor      <4> >);

PYBIND11_MAKE_OPAQUE(std::vector<larcv3::ImageMeta   <1> >);
PYBIND11_MAKE_OPAQUE(std::vector<larcv3::ImageMeta   <2> >);
PYBIND11_MAKE_OPAQUE(std::vector<larcv3::ImageMeta   <3> >);
PYBIND11_MAKE_OPAQUE(std::vector<larcv3::ImageMeta   <4> >);


PYBIND11_MODULE(pylarcv, m) {
  m.doc() = R"pbdoc(
      larcv
      -----------------------
      .. currentmodule:: larcv
      .. autosummary::
         :toctree: _generate
         base
  )pbdoc";
  
  init_base(m);
  init_dataformat(m);
  init_processor(m);
  init_queueio(m);
  init_imagemod(m);
  init_sbnd_imagemod(m);
}
