/**
 * @file ConfigManager.hh
 * @ingroup core_Base
 * @brief Class def header for a class larcv3::ConfigManager
 * @author drinkingkazu
 */

#pragma once

#include <iostream>
#include <set>

#include "larcv_base.hh"
#include "larbys.hh"
#include "PSet.hh"

/** @addtogroup core_Base
 * @{
 */
namespace larcv3
{
	/**
	 * @brief Utility class to register a set of configurations
	 * Provides also a shared instance through which registered 
	 * configurations can be shared beyond a single owner.
	 * Using larcv3::PSet, the uniqueness of configuration parameters 
	 * is guaranteed (no worry to "overwrite").
	 */
	class ConfigManager : public larcv_base
	{
	public:
		/// Default constructor
		ConfigManager() : larcv_base("ConfigManager")
		{}

		/// Default destructor
		~ConfigManager() 
		{}

		/// Shared static reference getter
		static const ConfigManager &get()
		{
			if (!_me)
			_me = new ConfigManager;
			return *_me;
		}
		/// Adder of configuration from a file
		void AddConfigFile(const std::string cfg_file);
		/// Adder of configuration from parsed string
		void AddConfigString(const std::string cfg_str);
		/// Configuration retrieval method
		const PSet &GetConfig(const std::string cfg);

	private:
		static ConfigManager *_me;
		std::set<std::string> _cfg_files;
		PSet _cfg;
	};
}
/** @} */ // end of doxygen group