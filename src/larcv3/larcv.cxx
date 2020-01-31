#include "larcv3/core/base/base.h"
#include "larcv3/core/dataformat/dataformat.h"
#include "larcv3/core/processor/processor.h"

PYBIND11_MODULE(pylarcv, m) {
  init_base(m);
  init_dataformat(m);
  init_processor(m);
}
