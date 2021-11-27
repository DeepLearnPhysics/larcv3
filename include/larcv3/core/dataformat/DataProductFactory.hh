/**
 * @file DataProductFactory.hh
 * @ingroup DataFormat
 * @brief Class def header for a class DataProductFactory
 * @author kazuhiro
 */
#pragma once

#include <iostream>
#include <map>
#include <sstream>

#include "larcv_base.hh"
#include "larbys.hh"
#include "EventBase.hh"
#include "DataFormatTypes.hh"

/** @addtogroup DataFormat
 * @{
 */
namespace larcv3 
{
	/**
	 * @brief Abstract base class for factory 
	 * (to be implemented per data product)
	 */
	class DataProductFactoryBase 
	{
	public:
		/// Default ctor
		DataProductFactoryBase()
		
		{}
		/// Default dtor (virtual)
		virtual ~DataProductFactoryBase()
		{}

		/// Abstract constructor method
		virtual EventBase* create() = 0;
	};

	/**
	 * @brief Factory class for instantiating event product instance 
	 * by larcv3::IOManager. This factory class can instantiate a specified 
	 * product instance w/ provided producer name. The actual factory core 
	 * (to which each product class must register creation factory instance) 
	 * is a static std::map. Use static method to get a static instance 
	 * (larcv3::DataProductFactory::get) to access a factory.
	 */
	class DataProductFactory : public larcv_base  
	{
	public:
		/// Default constructor, shouldn't be used
		DataProductFactory() 
		: larcv_base("DataProductFactory")
		{}

		/// Default destructor
		~DataProductFactory() 
		{_factory_map.clear();}

		/// Static sharable instance getter
		static inline DataProductFactory& get()
		{ if(!_me) _me = new DataProductFactory; return *_me; }

		/// Factory registration method 
		/// (should be called by global factory instance in algorithm header)
		void add_factory(std::string type, larcv3::DataProductFactoryBase* factory);

		/// Factory creation method 
		/// (should åbe called by clients, possibly you!)
		inline EventBase* create(const std::string& type, const std::string& producer) 
		{ return create(ProducerName_t(type,producer)); }

		/// Factory creation method 
		///	(should be called by clients, possibly you!)
		EventBase* create(const ProducerName_t& id);

		/// List registered products
		void list() const;

		inline size_t unique_product_count() const
		{ return _id_to_type.size(); }

		inline const std::vector<std::string>& product_names() const
		{ return _id_to_type; }

	private:
		/// Factory container
		std::map<std::string,larcv3::DataProductFactoryBase*> _factory_map;
		/// Unique product type ID
		std::vector<std::string> _id_to_type;
		/// Static self
		static DataProductFactory* _me;
	};
}
/** @} */ // end of doxygen group