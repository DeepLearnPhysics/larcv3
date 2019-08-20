from enum import Enum

class RandomAccess(Enum):
    # Read all events in order, always:
    serial_access = 0 
    # Read chunks of events in order:
    random_blocks = 1 
    # Read every event randomly from file:
    random_events = 2 

class ReadOption(Enum):
    # Use only the specified root rank to read the data, 
    # and the distribute to all other ranks:
    read_from_single_rank       = 0  

    # Use all ranks to read the data, no distribution is needed
    # This makes a copy of the file on every rank
    read_from_all_ranks_copy    = 1  

    # Use only one *local* rank to read the data, 
    # and the distribute to all other ranks in subgroup
    # This copies the file to the local rank.
    read_from_single_local_rank = 2  

    # Use every rank to read the data with MPI-IO
    # This is best for large files that are 
    # challenging to duplicate to every node.
    read_from_all_ranks_mpi     = 3
