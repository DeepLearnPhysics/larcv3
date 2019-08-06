import time
import numpy

from larcv import larcv, data_generator, queueloader

from collections import OrderedDict

def test_sparsetensor2d_queueio(batch_size, n_reads=10):



    # Prepare data managers:
    io_config = {
        'filler_name' : "TrainIO",
        'filler_cfg'  : "queue.cfg",
        'verbosity'   : 3,
        'make_copy'   : False
    }

    data_keys = OrderedDict({
        'label': "label", 
        'data': "data", 
        })



    li = queueloader.queue_interface(random_access_mode="serial_access")
    li.prepare_manager('primary', io_config, batch_size, data_keys)


    start = time.time()
    global_start = time.time()
    for i in range(n_reads):
        if i == 0:
            pop = False
        else:
            pop = True
        # Get the first batch of data:
        data = li.fetch_minibatch_data('primary', pop=pop, fetch_meta_data=True)
        print(data['entries'])
        # Start the next batch of data reading:
        start2 = time.time()
        t = li.prepare_next('primary')
        print(t)
        print(t.isAlive())
        print("Time to return from prepare next: ", time.time() - start2)
        # print(data['label'])
        # print(data['label'].shape)
        # print(data.keys())
        assert(data['label'].shape[0] == batch_size)
        end = time.time()

        # Pretend to do work:
        s = numpy.sum(data['label'])
        s2 = numpy.sum(data['data'])
        s3 = numpy.prod(data['label'])

        print("Time to get a batch of size {}: ".format(batch_size), end-start)
        start = time.time()

        # li.next("primary")

    print("Time to read {} batches of size {}".format(n_reads, batch_size), end - global_start)
    
if __name__ == "__main__":
    test_sparsetensor2d_queueio(batch_size=100, n_reads=4)
    print("Success")
