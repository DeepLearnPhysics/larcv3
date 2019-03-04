## Dataformat Schema.

This document outlines the principles of data product design and serialization using hdf5.

In general, you can read this document to get an understanding of how the data products fit together, though you may or may not need to read it to use larcv effectively.

(As of 3/4/19, the dataformat package consists of very lightweight objects to test IO and serialization.)

# Core Data Products:
 - EventBase (contains info about run/subrun/event, producer).  All dataproducts inherit from EventBase in their serialization wrapper.  The core data products do not, however the serialization wrapper (Typically named EventProduct, as in Particle is wrapped by EventParticle) will inherit from EventBase.
 - Voxel (and derivatives VoxelSet, VoxelSetArray).  
   - A voxel is effectively a two element object: an unsigned integer (typically size_t) for an index, and a floating point value.  Lone voxels are never serialized, though they can be created.  It is more efficient to add an (index, float) pair to a VoxelSet.
   - A VoxelSet is class containing two std::vectors of identical length.  There is an index vector and a value vector, which combined form a set of voxels.  A VoxelSet is contrained to hold voxels only from one projection if in 2D.  This is to enforce the notion that each voxel set gets it's own meta information.
   - A VoxelSetArray contains a series of VoxelSets.  This can be a series of VoxelSets for semantic segmentation (or instance segmentation), or a series of multiple projections, or both.  VoxelSetArray stores pointers to VoxelSets to allow each voxel set to grow dynamically if needed, without having to reshuffle the other voxel sets.
   - VoxelSetArrays have functions to perform simple slicing on VoxelSets.  You can ask for all VoxelSets with a given projectionID, for example.
 - ImageMeta (which covers 2D, 3D).  This class is to interpret a voxel index as a N-tuple coordinate.  It allows bulk conversion of voxel index to tuple'd indexes, suitable for numpy array indexes.
 - Particle.  This class is the primary label based class.  It can be used flexibly to store event-level labels, or individual particle labels.  Since this repository is designed for high energy physics, the particle object is able to store properties like momentum, energy, etc.  It is stored as a flat table and can easily be inspected with hdfview.

Not all of the core data products are reimplemented for larcv3 yet.  In particular, only the following have been completed with the new IO model.

# Serialization Overview

TODO