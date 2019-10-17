/**
 * \file Voxel.h
 *
 * \ingroup core_DataFormat
 *
 * \brief Class def header for a class larcv3::Voxel
 *
 * @author kazuhiro
 */

/** \addtogroup core_DataFormat

    @{*/
#ifndef __LARCV3DATAFORMAT_VOXEL_H
#define __LARCV3DATAFORMAT_VOXEL_H

#include "larcv3/core/dataformat/DataFormatTypes.h"
#include "larcv3/core/dataformat/ImageMeta.h"

namespace larcv3 {

  /**
     \class Voxel
     @brief  voxel definition element class consisting of ID and stored value
  */
  class Voxel {

  public:

    /// Default constructor
    Voxel(VoxelID_t id = kINVALID_VOXELID, float value = kINVALID_FLOAT);
    /// Default destructor
    ~Voxel() {}

    /// ID getter
    inline VoxelID_t id() const { return _id; }
    /// Value getter
    inline float  value() const { return _value; }

    /// Value setter
    inline void set(VoxelID_t id, float value) { _id = id; _value = value; }

    //
    // uniry operators
    //
    inline Voxel& operator += (float value)
    { _value += value; return (*this); }
    inline Voxel& operator -= (float value)
    { _value -= value; return (*this); }
    inline Voxel& operator *= (float factor)
    { _value *= factor; return (*this); }
    inline Voxel& operator /= (float factor)
    { _value /= factor; return (*this); }

    //
    // binary operators
    //
    inline bool operator == (const Voxel& rhs) const
    { return (_id == rhs._id); }
    inline bool operator <  (const Voxel& rhs) const
    {
      if ( _id < rhs._id) return true;
      if ( _id > rhs._id) return false;
      return false;
    }
    inline bool operator <= (const Voxel& rhs) const
    { return  ((*this) == rhs || (*this) < rhs); }
    inline bool operator >  (const Voxel& rhs) const
    { return !((*this) <= rhs); }
    inline bool operator >= (const Voxel& rhs) const
    { return !((*this) <  rhs); }

    inline bool operator == (const float& rhs) const
    { return _value == rhs; }
    inline bool operator <  (const float& rhs) const
    { return _value <  rhs; }
    inline bool operator <= (const float& rhs) const
    { return _value <= rhs; }
    inline bool operator >  (const float& rhs) const
    { return _value >  rhs; }
    inline bool operator >= (const float& rhs) const
    { return _value >= rhs; }

  private:
    VoxelID_t _id; ///< voxel id
    float  _value; ///< Pixel Value

#ifndef SWIG
  public: 
    static hid_t get_datatype() {

      hid_t datatype;
      herr_t status;
      datatype = H5Tcreate (H5T_COMPOUND, sizeof (Voxel));
      status = H5Tinsert (datatype, "id",
                  HOFFSET (Voxel, _id), larcv3::get_datatype<unsigned long>());
      status = H5Tinsert (datatype, "value", 
                  HOFFSET (Voxel, _value), larcv3::get_datatype<float>());
      return datatype;
    }
#endif


  };


  class kINVALID_VOXEL : public Voxel
    {
    public:
        static kINVALID_VOXEL& getInstance()
        {
            static kINVALID_VOXEL    instance; // Guaranteed to be destroyed.
                                  // Instantiated on first use.
            return instance;
        }
    private:
        kINVALID_VOXEL() {}                    // Constructor? (the {} brackets) are needed here.

        // // C++ 03
        // // ========
        // // Don't forget to declare these two. You want to make sure they
        // // are unacceptable otherwise you may accidentally get copies of
        // // your singleton appearing.
        // kINVALID_VOXEL(kINVALID_VOXEL const&);              // Don't Implement
        // void operator=(kINVALID_VOXEL const&); // Don't implement

        // C++ 11
        // =======
        // We can use the better technique of deleting the methods
        // we don't want.
    public:
        kINVALID_VOXEL(kINVALID_VOXEL const&)  = delete;
        void operator=(kINVALID_VOXEL const&)  = delete;

        // Note: Scott Meyers mentions in his Effective Modern
        //       C++ book, that deleted functions should generally
        //       be public as it results in better error messages
        //       due to the compilers behavior to check accessibility
        //       before deleted status
  };


  // static const larcv3::Voxel kINVALID_VOXEL(kINVALID_VOXELID,0.);


  /**
     \class VoxelSet
     @brief Container of multiple voxels consisting of ordered sparse vector and meta data
   */
  class VoxelSet {
  public:
    /// Default ctor
    VoxelSet() {_id=0;}
    /// Default dtor
    virtual ~VoxelSet() {}

    //
    // Read-access
    //
    /// InstanceID_t getter
    inline InstanceID_t id() const { return _id; }
    /// Access as a raw vector
    inline const std::vector<larcv3::Voxel>& as_vector() const { return _voxel_v; }
    /// Returns a const reference to a voxel with specified id. if not present, invalid voxel is returned.
    const Voxel& find(VoxelID_t id) const;
    /// Sum of contained voxel values
    inline float sum() const { float res=0.; for(auto const& vox : _voxel_v) res+=vox.value(); return res;}
    /// Mean of contained voxel values
    inline float mean() const { return (_voxel_v.empty() ? 0. : sum() / (float)(_voxel_v.size())); }
    /// Max of contained voxel values
    float max() const;
    /// Min of contained voxel values
    float min() const;
    /// Size (count) of voxels
    inline size_t size() const { return _voxel_v.size(); }

    /// Get the value of all voxels in this set
    std::vector<float> values() const;

    /// Get the index of all voxels in this set
    std::vector<size_t> indexes() const;

    //
    // Write-access
    //    
    /// Clear everything
    inline virtual void clear_data() { _voxel_v.clear(); }
    /// Reserve
    inline void reserve(size_t num) { _voxel_v.reserve(num); }
    /// Thresholding voxels by an upper and lower end values
    void threshold(float min, float max);
    /// Thresholding by only lower end value
    void threshold_min(float min);
    /// Thresholding by only upper end value
    void threshold_max(float max);

    /// Add a new voxel. If another voxel instance w/ same VoxelID exists, value is added
    void add(const Voxel& vox);
    /// Set a voxel. If another voxel instance w/ same VoxelID exists, value is set
    void insert(const Voxel& vox);
    /// Emplace a new voxel. Same logic as VoxelSet::add but consumes removable reference.
    void emplace(Voxel&& vox, const bool add);
    /// Emplace a new voxel from id & value
    inline void emplace(VoxelID_t id, float value, const bool add) 
    { emplace(Voxel(id,value),add); }
    /// InstanceID_t setter
    inline void id(const InstanceID_t id) { _id = id; }

    //
    // Uniry operations
    //
    inline VoxelSet& operator += (float value)
    { for(auto& vox : _voxel_v) vox += value; return (*this); }
    inline VoxelSet& operator -= (float value)
    { for(auto& vox : _voxel_v) vox -= value; return (*this); }
    inline VoxelSet& operator *= (float factor)
    { for(auto& vox : _voxel_v) vox *= factor; return (*this); }
    inline VoxelSet& operator /= (float factor)
    { for(auto& vox : _voxel_v) vox /= factor; return (*this); }

  private:
    /// Instance ID
    InstanceID_t _id;
    /// Ordered sparse vector of voxels
    std::vector<larcv3::Voxel> _voxel_v;
  };

  /**
     \class VoxelSetArray
     @brief Container of multiple VoxelSet (i.e. container w/ InstanceID_t & VoxelSet pairs)
  */
  class VoxelSetArray {
  public:
    /// Default ctor
    VoxelSetArray() {}
    /// Default dtor
    virtual ~VoxelSetArray() {}

    //
    // Read-access
    //
    /// Get # of VoxelSet
    inline size_t size() const { return _voxel_vv.size(); }
    /// Access specific VoxelSet
    const larcv3::VoxelSet& voxel_set(InstanceID_t id) const;
    /// Access all VoxelSet as a vector
    inline const std::vector<larcv3::VoxelSet>& as_vector() const
    { return _voxel_vv; }
    float sum() const;
    /// Mean of contained voxel values
    float mean() const;
    /// Max of contained voxel values
    float max() const;
    /// Min of contained voxel values
    float min() const;

    //
    // Write-access
    //
    /// Thresholding voxels by an upper and lower end values
    inline void threshold(float min, float max)
    { for(auto& vox_s : _voxel_vv) vox_s.threshold(min,max); }
    /// Thresholding by only lower end value
    inline void threshold_min(float min)
    { for(auto& vox_s : _voxel_vv) vox_s.threshold_min(min); }
    /// Thresholding by only upper end value
    inline void threshold_max(float max)
    { for(auto& vox_s : _voxel_vv) vox_s.threshold_max(max); }
    /// Clear everything
    inline void clear_data() { _voxel_vv.clear(); }
    /// Resize voxel array
    inline void resize(const size_t num)
    { _voxel_vv.resize(num); for(size_t i=0; i<num; ++i) _voxel_vv[i].id(i); }
    /// Access non-const reference of a specific VoxelSet 
    larcv3::VoxelSet& writeable_voxel_set(const InstanceID_t id);
    /// Move an arrray of VoxelSet. Each element's InstanceID_t gets updated
    void emplace(std::vector<larcv3::VoxelSet>&& voxel_vv);
    /// Set an array of VoxelSet. Each element's InstanceID_t gets updated
    // void insert(const std::vector<larcv3::VoxelSet>& voxel_vv);
    /// Move a VoxelSet into a collection. The InstanceID_t is respected.
    void emplace(larcv3::VoxelSet&& voxel_v);
    /// Set a VoxelSet into a collection. The InstanceID_t is respected.
    void insert(const larcv3::VoxelSet& voxel_v);
    /// Mover
    void move(larcv3::VoxelSetArray&& orig)
    { _voxel_vv = std::move(orig._voxel_vv); }

  private:
    std::vector<larcv3::VoxelSet> _voxel_vv;
  };

  /**
     \class SparseTensor
     @brief Container of multiple voxel set array
  */
  template<size_t dimension>
  class SparseTensor : public VoxelSet {
  public:
    /// Default ctor
    SparseTensor() {}
    SparseTensor(VoxelSet&& vs, ImageMeta<dimension> meta);

    /// Default dtor
    virtual ~SparseTensor() {}
    SparseTensor& operator= (const VoxelSet& rhs)
    { *((VoxelSet*)this) = rhs; this->meta(this->meta()); return *this;}

    //
    // Read-access
    //
    /// Access ImageMeta of specific projection
    inline const larcv3::ImageMeta<dimension>& meta() const { return _meta; }

    //
    // Write-access
    //
    /// Create & add/set a single voxel
    void emplace(const larcv3::Voxel & vox, const bool add=true);

    /// Emplace the whole voxel set w/ meta
    inline void emplace(VoxelSet&& vs, const ImageMeta<dimension>& meta)
    {*((VoxelSet*)this) = std::move(vs); this->meta(meta);}
    
    /// Set the whole voxel set w/ meta
    inline void set(const VoxelSet& vs, const ImageMeta<dimension>& meta)
    {*((VoxelSet*)this) = vs; this->meta(meta);} 
    
    /// Clear everything
    inline void clear_data() { VoxelSet::clear_data(); _meta = ImageMeta<dimension>(); }

    /// Meta setter
    void meta(const larcv3::ImageMeta<dimension>& meta);

  private:
    larcv3::ImageMeta<dimension> _meta;
    
  };

  /**
     \class SparseCluster
     @brief Container of multiple (-projected) voxel set array
  */
  template<size_t dimension>
  class SparseCluster : public VoxelSetArray {
  public:
    /// Default ctor
    SparseCluster() {}
    SparseCluster(VoxelSetArray&& vsa, ImageMeta<dimension> meta);
    
    /// Default dtor
    virtual ~SparseCluster() {}

    //
    // Read-access
    //
    /// Access ImageMeta of specific projection
    inline const larcv3::ImageMeta<dimension>& meta() const { return _meta; }

    //
    // Write-access
    //
    /// Clear everything
    inline void clear_data() { VoxelSetArray::clear_data(); _meta = ImageMeta<dimension>(); }
    /// set VoxelSetArray
    inline void set(VoxelSetArray&& vsa, const ImageMeta<dimension>& meta)
    { *((VoxelSetArray*)this) = std::move(vsa); this->meta(meta); }
    /// emplace VoxelSetArray
    // inline void emplace(const VoxelSetArray& vsa, const ImageMeta& meta)
    // { *((VoxelSetArray*)this) = vsa; this->meta(meta); }
    /// Meta setter
    void meta(const larcv3::ImageMeta<dimension>& meta);

  private:
    larcv3::ImageMeta<dimension> _meta;
  };  


// Define typedefs to specify sparse objects:
typedef SparseTensor<2>  SparseTensor2D;
typedef SparseTensor<3>  SparseTensor3D;
typedef SparseCluster<2> SparseCluster2D;
typedef SparseCluster<3> SparseCluster3D;

}

#endif
/** @} */ // end of doxygen group

