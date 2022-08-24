/**
 *
 * \ingroup DataFormat
 * @brief      This file implements Event I/O.
 *
 *  The goal of this class is to be able to generically serialize
 *  and deserialize multiple events at once.
 *  
 *  Doing one event at a time reproduces the old way.
 *  Each EventBase objected is expected to be able to 
 *  figure out it's own serialization, etc, and this class
 *  merges or splits as needed to turn N reads (writes) into 
 *  1 read (write).
 *
 * @author     Corey.adams
 * @date       2022
 * \addtogroup DataFormat
 */


#pragma once