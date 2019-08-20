import time
import numpy
import os

from larcv import larcv, data_generator
from larcv.distributed_queue_interface import queue_interface

from collections import OrderedDict

from mpi4py import MPI 

import tempfile
import argparse




cfg_template = '''
TrainIO:{{
  Verbosity: 5
  EnableFilter: false
  InputFiles: [{input_file}]
  RandomSeed: 0
  RandomAccess: false
  ProcessType: ["BatchFillerSparseTensor2D","BatchFillerSparseTensor2D"]
  ProcessName: ["data","label"]


  ProcessList: {{
    data: {{
      Verbosity: 3
      Tensor2DProducer: sbndwire
      IncludeValues: true
      MaxVoxels: 35000
      Channels: [0,1,2]
      UnfilledVoxelValue: -999
      Augment: false
    }}
    label: {{
      Verbosity: 3
      Tensor2DProducer: sbnd_cosmicseg
      IncludeValues: true
      MaxVoxels: 35000
      Channels: [0,1,2]
      UnfilledVoxelValue: -999
      Augment: false
    }}
  }}
}}
'''


def iterate_queueio(batch_size, n_reads, input_file):

    # Generate a named temp file:
    config_file = tempfile.NamedTemporaryFile(mode='w', delete=False)
    config_file.write(cfg_template.format(input_file=input_file))

    config_file.close()

    # Prepare data managers:
    io_config = {
        'filler_name' : "TrainIO",
        'filler_cfg'  : config_file.name,
        'verbosity'   : 3,
        'make_copy'   : False
    }


    data_keys = OrderedDict({
        'label': "label", 
        'data': "data", 
        })


    global_start = time.time()

    li = queue_interface(random_access_mode="random_blocks")
    li.prepare_manager('primary', io_config, batch_size, data_keys, color = MPI.COMM_WORLD.rank)

    local_batch_size = batch_size / MPI.COMM_WORLD.Get_size()
    start = time.time()
    for i in range(n_reads):
        if i == 0:
            pop = False
        else:
            pop = True
        # Get the first batch of data:
        data = li.fetch_minibatch_data('primary', pop=pop, fetch_meta_data=True)
        # Start the next batch of data reading:
        start2 = time.time()
        # next_entries = li.coordinate_next_batch_indexes('primary', MPI.COMM_WORLD)
        t = li.prepare_next('primary')
        # print("Time to return from prepare next: ", time.time() - start2)
        # print(data['label'])
        # print(data['label'].shape)
        # print(data.keys())
        assert(data['label'].shape[0] == local_batch_size)
        end = time.time()


        # print("Time to get a batch of size {}: ".format(batch_size), end-start)
        start = time.time()

        # li.next("primary")
    os.remove(config_file.name)

    if MPI.COMM_WORLD.rank == 0:
        print("RESULT: Time to read {} batches of size {}, local size {} ".format(n_reads, batch_size, local_batch_size), end - global_start)
    
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Configuration Flags")
    parser.add_argument("-f",  "--input-file", help="Input File")
    parser.add_argument("-bs", "--batch-size", help="Batch Size",type=int, default=64)
    parser.add_argument("-i",  "--iterations", help="Iterations",type=int, default=25)

    args = parser.parse_args()

    iterate_queueio(
        batch_size=args.batch_size, 
        n_reads=args.iterations,
        input_file=args.input_file)
    if MPI.COMM_WORLD.rank == 0:
        print("Success")
