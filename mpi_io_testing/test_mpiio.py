split = True


if split:
    from mpi4py import MPI 
    rank = MPI.COMM_WORLD.rank
    print("Rank: ", rank)
else:
    rank = 0

from larcv import larcv

a = larcv.IOManager(larcv.IOManager.kREAD)
a.set_verbosity(5)
if rank % 2 == 0:
    a.add_in_file("/Users/corey.adams/data/dlp_larcv3/sbnd_cosmic_samples/cosmic_tagging_downsample_train_sparse.h5")
else:
    a.add_in_file("/Users/corey.adams/data/dlp_larcv3/sbnd_cosmic_samples/cosmic_tagging_downsample_test_sparse.h5")


a.initialize(rank % 2)
# a.reset()
# a.initialize(rank % 2)

a.read_entry(rank)

sparse = larcv.EventSparseTensor2D.to_sparse_tensor(a.get_data("sparse2d", "sbndwire"))

print(a.event_id().event_key())
print(sparse.sparse_tensor(0).size())