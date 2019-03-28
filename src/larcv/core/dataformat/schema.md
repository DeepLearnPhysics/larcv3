## Dataformat Schema.

This document outlines the principles of data product design and serialization using hdf5.

In general, you can read this document to get an understanding of how the data products fit together, though you may or may not need to read it to use larcv effectively.

(As of 3/4/19, the dataformat package consists of very lightweight objects to test IO and serialization.)

# Core Data Products:
* EventBase (contains info about run/subrun/event, producer).  All dataproducts inherit from EventBase in their serialization wrapper.  The core data products do not, however the serialization wrapper (Typically named EventProduct, as in Particle is wrapped by EventParticle) will inherit from EventBase.
* Voxel (and derivative collections VoxelSet, VoxelSetArray).  
    * A voxel is effectively a two element object: an unsigned integer (typically size_t) for an index, and a floating point value.  Lone voxels are never serialized, though they can be created.  It is more efficient to add an (index, float) pair to a VoxelSet.
    * A VoxelSet is class containing two std::vectors of identical length.  There is an index vector and a value vector, which combined form a set of voxels.  A VoxelSet is contrained to hold voxels only from one projection if in 2D.  This is to enforce the notion that each voxel set gets it's own meta information.  Since projection ID is not a property of voxels, VoxelSet must hold an optional projection ID 
    * A VoxelSetArray contains a series of VoxelSets.  This can be a series of VoxelSets for semantic segmentation (or instance segmentation), or a series of multiple projections, or both.  VoxelSetArray stores pointers to VoxelSets to allow each voxel set to grow dynamically if needed, without having to reshuffle the other voxel sets.
    * VoxelSetArrays have functions to perform simple slicing on VoxelSets.  You can ask for all VoxelSets with a given projectionID, for example.
* ImageMeta (which covers 2D, 3D).  This class is to interpret a voxel index as a N-tuple coordinate.  It allows bulk conversion of voxel index to tuple'd indexes, suitable for numpy array indexes.
* Particle.  This class is the primary label based class.  It can be used flexibly to store event-level labels, or individual particle labels.  Since this repository is designed for high energy physics, the particle object is able to store properties like momentum, energy, etc.  It is stored as a flat table and can easily be inspected with hdfview.

Not all of the core data products are reimplemented for larcv3 yet.  In particular, only the following have been completed with the new IO model.

# File Organization

The serialization model depends on a carefully organized structure within the file.  To this end, there will be several main groups.

### Group: Events

This group will have one table, the events table, with the columns run/subrun/event.  This table will be used in every other dataproduct at load time so that there is a consistent, and provides the mapping from entry number (index in this table) to run/subrun/event.  The table will be called event_index

### Group: Data

This group contains the meat of the data in a file.  It will have a number of subgroups with the naming convention:
dataproduct_producername_group

Additionally, there may be a final subgroup of contents which holds the list of groups and their producers, and potentially their history.

### How is a data subgroup organized?

For any given subgroup, there will be at least two datasets inside of the group:
* event_extents
* data
* [extents_1]
* [data_2]
* [extents_2]
* ...

The data tables holds the actual content of the dataproduct.  For VoxelSets and their permutations, this will mean a table that has dimensions (N, 2), where N is the total number of voxels in a file.  For an image, the shape might be (N, 512, 512).

The data and it's access/organization is described by the extents tables.  So, the access to the table data_1 will be governed by extents_1.


### Data Product Interface with Serialization

All serialization is handled through the Event* classes (EventParticle, EventVoxel, etc).  In general, each of these classes inherits from EventBase, and contains a vector of their respective dataproducts.  Each class will have to manage flattening and unflattening of it's respective data.  So, for example, EventParticle inherits from EventBase and contains an std::vector<larcv::particle>.  For serialization, 
1. On READING, the IO process will look in the group data/particle_producername_group for the tables:
    * data
    * extents
2. From the event_extents table, which instructs the upper and lower index in the table, the list of particles is read in.
    * The EventParticle class will interpret the data as an array of particle objects, which can be wrapped in an std::vector and returned to the user.

1. On WRITING, the first step will be to update the EventBase Table with the event object for this file.
2. For the EventParticle class, the vector of particles is assumed to be static once the write function is called.
    1. The extents table is appended with the right numbers
    2. The data table is appended with the new data


# Serialization Overview

There are several steps in the workflow of data serialization.  I will first describe the WRITE mode serialization model, then the READ mode.  Finally, I will describe how they can be run simultaneously (that is, a COPY or APPEND operation)

## WRITE Mode Serialization

1. When IOManager's Initialize function is called, an output file is opened.
    * The output file is managed exclusively by IOManager
2. After the output file is opened, the top level groups are created ('Events', and 'Data')
3. The event ID table is created
4. Control passes to the processing loop (whatever is driving it, python or ProcessDriver or ThreadIO)
    * Data products are created in memory using the get_data functions.
    * When a data product is created for the first time, it's subgroup must be created under the 'Data' group.
    * Initialization of datasets inside of a subgroup is managed by the Event* class as serialization is object dependent. 
5. When one iteration of the processing event loop is finished and IOManager::save_entry() is called, all data products are assumed to be finalized.  At this point, the serialization of each object is passed to it's individual Event* class.
    * Each class serializes it's own data into it's tabular format, and clears it's internal data buffers.


## IO Data Management on entry-by-entry basis

When reading and writing files simulatenously, the following workflow is supported:
* Multiple input files are supported.  Files are assumed to be "distinct" in that they do not contain events spread across files.  Instead, each event is entirely contained within one file.  Therefore, the list of producers in each file and their respective products is checked to match across all files, and an exception is raised if it fails.
* Only one input file can be accessed at one time efficiently.  There is one "open" file, which iterates to the next file at it's end of file.  Random access introduces overhead of jumping between files.



### Some details of the Data Model.

## Voxels

Voxels are the core data storage object for sparse data.  There is no distinction on disk between a voxel in 2D and a voxel in 3D, as each voxel is just a combination of a value (float) and index (size_t).  Individual voxels are not useful, instead collections of voxels are useful.  The following combinations are the most useful:
1. A single collection of voxels representing a sparse image.  This can be in either 2D or 3D.
2. A collection of collection of voxels.  This is either a set of sparse images in 2D, or could be a set of cluster information (instance masks) in 3D.
3. A collection of collection of collection of voxels.  This is only applicable to a set of instance masks in 2D, across multiple planes.

The fundamental serialization unit is a VoxelSet, aka std::array<larcv::Voxel>.  So, condidtion 1 above is a very simple serialization model.  This is represented as a SparseImage in larcv.  EventSparse will contain an array of SparseImages which will be either a single element (for a 3D event) or multiple elements (for a 2D event).

For seriallizing cluster information, EventCluster will be used.  This object will store an array of Cluster set information, one for each prjection, or just one for 3D.  A cluster set is an array of VoxelSets, so it will be stored as std::vector< larcv::VoxelSet * >.  The pointers allow a little more flexibity at creation time for adding voxels.

Both EventSparse and EventCluster will have a 1 to 1 correspondance with ImageMeta objects.  ImageMeta will provide a flexible correspondance between voxel index and real coordinates, as well as generic useful functions for getting information about the space the image lives in.

To be concrete, EventSparse will handle the IO for sparse 2D and sparse 3D images.  It will read in a set of Voxels from file and map them to a std::vector<larcv::VoxelSet>, where each element of the array represents a unique projection ID.

EventCluster will handle IO for cluster 2D and cluster3D images.  It will read a flat table of voxels and map them to an std::vector<larcv::VoxelSetArray>.

In both cases, the ImageMeta will be stored in the attributes for the datasets.


## Storing Voxel information


Voxels are, at a fundamental level, a pair of values, and a collection of voxels, voxelsets, voxelsetarrays is a vector of voxels, and meta information about how to slice it up.  Each selection of voxels has a length, an offset from the front of the main vector, and some general index to indicate how it fits into the big picture.  For concreteness, some examples:

* VoxelSets in 2D, aka sparse images, with 3 projections.  Let's say the number of voxels at each projection is N1, N2, N3, for a total of N = N1 + N2 + N3.  The metadata needed to reconstruct the individual voxel sets is:
  1. Voxel set with *ID 0* starts at offset 0 and has length N1
  1. Voxel set with *ID 1* starts at offset N1 and has length N2
  1. Voxel set with *ID 2* starts at offset N3 and has length N3

* VoxelSets in 3D, aka sparse images, with only one projection.  Simpler than above, we need only know that N1 = N, offset is 0, and there is only one ID = 0.


* VoxelSetArray in 3D, aka 3D clusters.  This is almost identical to the first case above, but now the Voxel id's all have a projection ID of 0, and inside of that another ID of 0, 1, 2, 3, etc ...

* VoxelSetArray in 2D, with 3 projections, aka 2D clusters.  In this case, we have a complex situation.  We need to take our flat list of voxels and slice it into a 2D indexed dimension.  So, all of the voxelSets in projection 0 are different than the ones in projection 1, but it makes sense to preserve the the same serialization tools for all of this.

A solution: the problem of flattening a multi dimensional index into a flat index is well solved already by ImageMeta.  So, larcv will use a static, constant instance of an image meta that wil map the projectionID and clusterID indexes into a flat index.  At serialization time, particularly at initialization, the ImageMeta handling the ID raveling will be persisted into the file so that the deserialization can reference it.  This preserves backwards compatibility if we ever need to change the meta information.

In this model, *ALL* IO for voxel data products at it's core level will read in a flat array of Voxels, and map to variable length 2D arrays (ragged array) to manage the splitting of different voxels to different images, cluster, projections, etc.  An important, and necessary, constraint is that at the deepest index the voxels are contiguous in file and not interleaved with any other voxel set.  To be explicit, the groups in the the voxel persistence will be 3:

* *voxels* will contain a long table of larcv::Voxels, only storing index and value pairs

* *voxel_map* will contain the mapping of the voxel table within an event.  Each entry will have the raveled projection/clusterID, the starting voxel (offset), and the length of this object

* *event_extents* will contain the extents of the voxel_map for a particular event.  So, this will have only starting offset and number of voxelset IDs.

After reading, the voxels may be diverted into index/value stored in independent arrays within an object


#### How is the Image meta handled?

Image meta is static with the projectionIDs.  Therefore, all images with the same projectionID, regardless of clusterID, are using the same image meta.  Image meta is also static for all events in a file.

Since Image meta is property of the entire producer/dataproduct for all events/entries in a file, it is stored as a group attribute.  Additionally, the projection/cluster meta is stored as a group attribute.  When the group is initialized, the projection/cluster meta is serialized for the first time.  When the group is read for the first time, the projection/cluster meta is read from file for deserializing.  When the first voxels are written, the image meta for each projection ID is written to file.



