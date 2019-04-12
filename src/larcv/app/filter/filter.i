%module filter
%{
/* Put header files here or function declarations like below */
#include "app/filter/filterTypes.h"
#include "app/filter/EmptyImageFilter.h"
#include "app/filter/EmptyTensorFilter.h"
#include "app/filter/EventIDFilter.h"
#include "app/filter/MCSingleParticleFilter.h"
#include "app/filter/ParticleCountFilter.h"
%include "app/filter/QSumFilter.h"
#include "app/filter/ProcessFactory.h"

using namespace larcv;
%}

%include "app/filter/filterTypes.h"
%include "app/filter/EmptyImageFilter.h"
%include "app/filter/EmptyTensorFilter.h"
%include "app/filter/EventIDFilter.h"
%include "app/filter/MCSingleParticleFilter.h"
#include "app/filter/ParticleCountFilter.h"
%include "app/filter/QSumFilter.h"
%include "app/filter/ProcessFactory.h"

