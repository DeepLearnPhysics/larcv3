## How to parallize IO over Events.

The serial IO is pretty well optimized.  It uses several table lookups to figure where to read the real data from, and then reads it.  Serialization is the same but backwards: read the tables to find out where to put data, then update tables with where it was put.

Parallel IO is likely efficient if it's doing one big HDF5 read (or write) and can then parallelize processing of events too.

In practice, this means for deserialization, we should be joining hyper slabs, making one big read, and then chunking data.  The only data that _needs_ to be chunked is the actual event data.

For example, deserializing Particle data has several steps.  We assume we're given a list of events to read as `std::vector<size_t> entries`.

For a single entry, the existing workflow is to do:
- Use the entry to read the `extents` table at row `entry`.
