/**
 * @defgroup   GROUPBASE Group Base
 *
 * @brief      This file implements Group Base, an abstraction of a collection of
 *             EventBase Objects.
 *
 * @author     Corey.adams
 * @date       2022
 */

#pragma once

template <clase dataproduct>
class GroupBase
{
public:
    GroupBase();
    /// Destructor
    ~GroupBase();
  


    /**
     * @brief      index access pass through
     *
     * @param[in]  index  The index
     *
     * @return     { Returns the object at the index requested }
     */
    inline dataproduct at(size_t index) {return _group_v.at(index);}



    /**
     * @brief      Set the entire collection for this event
     *
     * @param[in]  object_v  The object vector
     */
    void set(const std::vector<EventBase<dataproduct>>& object_v);

    /**
     * @brief      append an object to the end
     *
     * @param[in]  object  The dataproduct
     */
    void append(const EventBase<dataproduct>& object);

    /**
     * @brief      Emplace an object to the end with std::move(dataproduct)
     *
     * @param      object  The object
     */
    void emplace_back(EventBase<dataproduct>&& object);


    /**
     * @brief      emplace a set of objects to the end with std::move
     *
     * @param      object_v  The object v
     */
    void emplace(std::vector<EventBase<dataproduct>>&& object_v);

    inline const std::vector<EventBase<dataproduct>>& as_vector() const
    { return _group_v; }

    // inline const larcv3::Particle& at(size_t index) const {return _group_v.at(index);}

    inline size_t size() const {return _group_v.size();}



    /**
     * @brief      Clears the object.
     */
    void clear();

    /**
     * @brief      Initialization
     *
     * @param[in]  group        The group
     * @param[in]  compression  The compression
     */
    void initialize (hid_t group, uint compression) ;

    /**
     * @brief      Serialize this object
     *
     * @param[in]  group  The group
     */
    void serialize  (hid_t group);

    /**
     * @brief      Deserialize this object at specified entry
     *
     * @param[in]  group          The group
     * @param[in]  entry          The entry
     * @param[in]  reopen_groups  For re-opening groups, if file changed for example
     */
    void deserialize(hid_t group, size_t entry, bool reopen_groups = false);

    /**
     * @brief      Finalize this series of objects by closing HDF5 objects
     */
    void finalize();

    /**
     * @brief      Opens in datasets.
     *
     * @param[in]  group  The group
     */
    void open_in_datasets(hid_t group );
    
    /**
     * @brief      Opens out datasets.
     *
     * @param[in]  group  The group
     */
    void open_out_datasets(hid_t group );

    std::vector<hid_t> _open_in_datasets;
    std::vector<hid_t> _open_in_dataspaces;
    std::vector<hid_t> _open_out_datasets;
    std::vector<hid_t> _open_out_dataspaces;
    std::vector<hid_t> _data_types;

    /**
     * @brief      Gets the number objects.
     *
     * @param[in]  group  The group
     *
     * @return     The number objects.
     */
    int get_num_objects(hid_t group);


private:

    // This represents the actual groups
    std::vector<EventBase<dataproduct>> _group_v;

};