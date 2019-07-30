import time

from larcv import larcv, data_generator, larcv_interface

from collections import OrderedDict



def test_sparsetensor2d_threadio(batch_size, n_reads=10):



    # Prepare data managers:
    io_config = {
        'filler_name' : "TrainIO",
        'filler_cfg'  : "processes.cfg",
        'verbosity'   : 3,
        'make_copy'   : True
    }

    data_keys = OrderedDict({
        'label': "label", 
        'data': "data", 
        })



    li = larcv_interface.larcv_interface()
    li.prepare_manager('primary', io_config, batch_size, data_keys)


    start = time.time()
    global_start = time.time()
    for i in range(n_reads):
        li.next("primary")
        data = li.fetch_minibatch_data('primary')
        print(data['label'].shape)
        print(data.keys())
        assert(data['label'].shape[0] == batch_size)
        end = time.time()
        print("Time to get a batch of size {}: ".format(batch_size), end-start)
        start = time.time()

    print("Time to read {} batches of size {}".format(n_reads, batch_size), end - global_start)
    
if __name__ == "__main__":
    test_sparsetensor2d_threadio(batch_size=20, n_reads=10)
    print("Success")
