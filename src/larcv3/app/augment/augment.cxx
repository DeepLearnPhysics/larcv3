#include "augment.h"

#include "GaussianBlur.h"
#include "Translate.h"
#include "Mirror.h"

void init_augment(pybind11::module m){

  init_gaussian_blur(m);
  init_translate(m);
  init_mirror(m);
}
