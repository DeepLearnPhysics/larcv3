/**
 * @defgroup   DATAPRODUCT Data Product
 *
 * @brief      This file implements Data Product.
 *
 * @author     Corey.adams
 * @date       2022
 */


#pragma once

#include "larcv3/core/dataformat/DataFormatTypes.h"

namespace larcv3 {
    
    class DataProduct
    {
    public:
        DataProduct(){};
        ~DataProduct(){};
    
        void id(InstanceID_t i){};
        hid_t get_datatype(){return -1;}
    private:

    };



}