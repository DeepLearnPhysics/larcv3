from mpi4py import MPI 
rank = MPI.COMM_WORLD.rank
from larcv import larcv
print("imported larcv")


# f_list = larcv.VectorOfString()
# f_list.push_back("/Users/corey.adams/data/dlp_larcv3/sbnd_cosmic_samples/cosmic_tagging_downsample_train_sparse.h5")

config = "processes.cfg"
driver = larcv.ProcessDriver("ProcessDriver")
driver.configure(config)
driver.initialize()

print("Constructed Driver")

factory = larcv.BatchDataStorageFactoryFloat.get()
factory.make_storage("data", 1)
factory.make_storage("label", 1)
  # BatchDataStorageFactory<float>::get_writeable().make_storage(name, _num_batch_storage); break;


driver.process_entry(rank)

print("Processed Entry Successfully")

data = factory.get_storage("data")
label = factory.get_storate("label")

# a.initialize()