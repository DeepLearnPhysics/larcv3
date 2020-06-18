import sys
import os
import time

import numpy
import larcv

class larcv_writer(object):

    '''A utility class for writing data as larcv objects to file.
    
    This class doesn't reinvent any wheels, it's just meant to package everything
    into an easy to use interface for writing output files.
    '''

    def __init__(self, io_config, output_file=None):
        '''larcv_writer accesses an IOManager instance directly to manage writing
        
        In the init function, the IOManager is created and prepared for writing.
        
        Arguments:
             {} -- [description]
        '''


        self._io = larcv.IOManager(io_config)

        if output_file is not None:
            self._io.set_out_file(output_file)

        self._io.initialize()

        self._write_workers = {
            'sparse2d' : self._write_sparse2d,
            'image2d'  : self._write_image2d,
            'tensor1d' : self._write_tensor1d
        }

        pass

    def finalize(self):
        # The purpose of this function is to make sure the output file is persisted to disk:
        self._io.save_entry()
        self._io.finalize()

    def _seek(self, entry):
        '''Go to an entry
        
        Saves the current entry and seeks to a new entry.
        
        Arguments:
            entry {int} -- Desired Entry
        '''
        self._io.save_entry()
        self._io.read_entry(entry)

        return

    def _write_sparse2d(self, data, producer):
        '''Write sparse2d data to file.
        
        Data is writting under the provided producer name.  This function takes a list of data
        that it can iterate through, assigning each element of the list contains a dict with the following 
        three elements:
         - 'index' is a flattened 1D array of index values, which are the 'raveled' values
         - 'value' is the list of features
         - 'meta' is the meta data object.  It is meant to have the arugments for the constructor of ImageMeta2d,
            so it is a list in the following order: [xmin, ymin, xmax, ymax, n_x, n_y, projection_id]
        
        Arguments:
            data {list} -- list (by projection ID) of dict objects.  dict must have the keys 'index', 'value' and 'meta'
            producer {[type]} -- [description]
        '''
        _writable_data = self._io.get_data("sparse2d", producer)


        for projection in range(len(data)):
            value = data[projection]['values']
            index = data[projection]['index']
            shape = data[projection]['shape']
            meta = larcv.ImageMeta2D()
            meta.set_projection_id(projection)
            meta.set_dimension(0, shape[0], int(shape[0]))
            meta.set_dimension(1, shape[1], int(shape[1]))

            # We need to put in voxels and indexes together.  Argsort handles this:
            perm = numpy.argsort(index)

            # First, copy all of the values into a VoxelSet object:
            voxel_set = larcv.VoxelSet()
            voxel_set.set(index[perm], value[perm])
            # for i in range(len(value)):
            # _ = [voxel_set.emplace(index[i], value[i], False) for i in range(len(value))]


            # larcv.as_voxelset(value.astype(numpy.float32), index.astype(numpy.uint64))
            # Add the voxel set:
            _writable_data.set(voxel_set, meta)

        return

    def _write_image2d(self, data, producer):
        ''' Write image2d data to file

        Data is written under the provided producer name.  This function takes a list of data
        that it can iterate through, where each list element is a projection ID.
        '''


        ev_image2d = self._io.get_data("image2d", producer)

        for projection_id, image in enumerate(data):
            meta = larcv.ImageMeta2D()
            meta.set_dimension(0, image.shape[0], image.shape[0])
            meta.set_dimension(1, image.shape[1], image.shape[1])
            meta.set_projection_id(projection_id)
            img = larcv.as_image2d_meta(image, meta)
            ev_image2d.emplace(img)
            # print(img)

        return


    def _write_tensor1d(self, data, producer):
        '''
        Write tensor1d data to file

        This can be used to save the softmax output for different categories
        Arguments:
            data {numpy} -- Data to write, a list of values
            producer {str} -- Producer key underwhich to store this data

        '''

        n_classes = len(data)

        ev_tensor = self._io.get_data("tensor1d", producer)

        tensor = larcv.as_tensor1d(data)

        ev_tensor.append(tensor)

        return



    def write(self, data, datatype, producer, entry, event_id):
        '''Write data to file
        
        This function writes data to the output file.  It will check the entries and event ids
        to ensure there is not a mismatch.  You can call this function multiple times per event,
        if there are multiple things to write.  When you call it for the next event, and it has
        a new entry/event_id, it will save the previous event, seek to the next event, and continue.
        
        Arguments:
            data {numpy} -- Data to write, must meet certain formatting depending on datatype
            datatype {str} -- larcv2 datatype descriptor
            producer {srt} -- producer key underwhich to store this data
            entries {vector<int>} -- vector of file entries, size 1 for now
            event_ids {vector<larcv::EventBase>} -- vector of larcv EventBase objects, size 1 for now
        '''

        # First, check if the entries recieved for the data match the current entry
        # for the output file:

        # print("Request recieved to write at entry ", entry)
        # print("Current entry is ", self._io.current_entry())
        # print("Current event ID is ", self._io.event_id())
        # print("Set event ID is ", event_id)

        if self._io.current_entry() != entry:
            self._seek(entry)

        if datatype not in self._write_workers:
            raise Exception("No current support to write datatypes of type " + datatype)

        else:
            self._write_workers[datatype](data, producer)


        pass

