#include "larcv_base.h"
#include <iomanip>

json larcv3::larcv_base::augment_default_config(const json& default_config, const json & user_config){

    // This function looks at every option in the default config, which is assumed to be the right format.
    // First, make a copy of the default config.
    // For each key, it checks if the key is in the user_config.
    // If the key IS NOT in the user config, continue.
    // If the key IS in the user config:
    //    - if the item is iterable, recurse on the sub json.
    //    - if the item is not iterable, replace the default with the user config.

    json updated_config = default_config;

    // If the parameter's object is in fact an iterable, recurse.

    // std::cout << "  Calling augment_default_config with " << std::endl;
    // std::cout << std::setw(4) << "    default: " << default_config << std::endl;
    // std::cout << std::setw(4) << "    user: " << user_config << std::endl;

// For future corey: this is thinking array objects are actually sub classes.  Whoops.
// Gotta fix that

    for (auto it = default_config.begin(); it != default_config.end(); ++it) {
        // std::cout << it.key() << ": " << *it << '\n';
        if ( user_config.contains(it.key()) ){
            // std::cout << "  Key " << it.key() << " IS in user_config" << std::endl;
            // Check if the object itself is a json class
            if (it.value().is_object()){
                // std::cout << "    Key " << it.key() << " treated as object" << std::endl;
                // recurse
                // Merge the two together and put it into the updated config
                updated_config[it.key()] = augment_default_config(
                    default_config[it.key()],
                    user_config[it.key()]);
                
            }
            else{
                // std::cout << "    Key " << it.key() << " NOT treated as object" << std::endl;
                // Replace the single item:
                updated_config[it.key()] = user_config[it.key()];
            }
        }
        else{
            // std::cout << "  Key " << it.key() << " is NOT in user_config" << std::endl;
            // This key is not in the user config, so we copy the default in entirety
            updated_config[it.key()] = default_config[it.key()];

        }
    }
    // std::cout << "  Finshed augment_default_config with " << std::endl;
    // std::cout << std::setw(4) <<  "    default: " << default_config << std::endl;
    // std::cout << std::setw(4) <<  "    user: " << user_config << std::endl;
    // std::cout << std::setw(4) <<  "    Final config is " << updated_config << std::endl;

    return updated_config;

}


void init_larcv_base(pybind11::module m){
    pybind11::class_<larcv3::larcv_base>(m, "larcv_base")
    .def(pybind11::init<const std::string &>(), pybind11::arg("name")="larcv_base")
    .def("logger", &larcv3::larcv_base::logger)
    .def("set_verbosity", &larcv3::larcv_base::set_verbosity)
    .def("name", &larcv3::larcv_base::name)
    .def("default_config", &larcv3::larcv_base::default_config)
    ;

}
