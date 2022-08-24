/**
 * @defgroup   GROUPPARTICLE Group Particle
 *
 * @brief      This file implements Group Particle, a collection of EventParticles
 *
 * @author     Corey.adams
 * @date       2022
 */


#pragma once

#include "GroupBase.h"

namespace larcv3 {

    typedef GroupBase<larcv3::EventBase<Particle>> GroupParticle;

}