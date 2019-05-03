#ifndef __LARCV3BASE_CONFIGMANAGER_CXX__
#define __LARCV3BASE_CONFIGMANAGER_CXX__

#include "larcv3/core/base/ConfigManager.h"
#include "larcv3/core/base/LArCVBaseUtilFunc.h"
namespace larcv3 {

  ConfigManager* ConfigManager::_me = nullptr;

  void ConfigManager::AddConfigFile(const std::string cfg_file)
  {
    if(_cfg_files.find(cfg_file)!=_cfg_files.end()) {
      LARCV_CRITICAL() << "Duplicate addition of config fiel: " << cfg_file << std::endl;
      throw larbys();
    }

    _cfg.add_pset(CreatePSetFromFile(cfg_file));
  }

  void ConfigManager::AddConfigString(const std::string cfg_str)
  {
    PSet p;
    p.add(cfg_str);
    _cfg.add_pset(p);
  }
  
  const PSet& ConfigManager::GetConfig(const std::string cfg)
  {
    return _cfg.get_pset(cfg);
  }

}

#endif
