#ifndef __LARCV3THREADIO_BATCHFILLERPARTICLE_CXX__
#define __LARCV3THREADIO_BATCHFILLERPARTICLE_CXX__

#include "BatchFillerParticle.h"
#include <random>

namespace larcv3 {

  static BatchFillerParticleProcessFactory __global_BatchFillerParticleProcessFactory__;

  BatchFillerParticle::BatchFillerParticle(const std::string name)
    : BatchFillerTemplate<larcv3::Particle>(name)
  {}

  void BatchFillerParticle::configure(const json& cfg){
    config = this -> default_config();
    config = augment_default_config(config, cfg);
  }

  void BatchFillerParticle::initialize(){}

  bool BatchFillerParticle::process(IOManager& mgr){

    std::string producer = config["ParticleProducer"].get<std::string>();
    // Fetch the particles:
    auto const& event_part = mgr.get_data<larcv3::EventParticle>(producer);

    // Refresh the dimension:
    std::vector<int> dim(2);
    dim[0] = batch_size();
    dim[1] = event_part.size();
    set_dim(dim);


    // labels
    auto const& part_v = event_part.as_vector();
    if (part_v.size() != 1) {
      LARCV_CRITICAL() << "Only support single particle label now: EventParticle size != 1" << std::endl;
      throw larbys();
    }
    // class
    size_t label = kINVALID_SIZE;
    int pdg = 0;

    _entry_data.resize(1);
    _entry_data.at(0) = part_v.front();


    set_entry_data(_entry_data);

    return true;


  }

  void BatchFillerParticle::_batch_begin_(){
    if (!batch_data().dim().empty() && (int)(batch_size()) != batch_data().dim().front()) {
      auto dim = batch_data().dim();
      dim[0] = batch_size();
      this->set_dim(dim);
    }

  }

  void BatchFillerParticle::_batch_end_(){}

  void BatchFillerParticle::finalize(){}

}

#endif