import sys,time,os,signal
import numpy
import larcv
import random

class dataloader(object)

    '''Dataloader for larcv3
    
    This class manages the data loading from larcv, including preprocessing

    To configure this, you create the dataloader with a configuration dictionary like so:
    {
        'train': {
            'data' : {
                'input_data' : 'sparse2d'
                'preprocess' : {
                    'Downsample' : ....
                }
            }
        }
    }

    '''

    def __init__(self, verbose=False, random_access_mode="random_blocks", seed=None):
        '''init function

        Not much to store here, just a dict of dataloaders and the keys to access their data.

        Queue loaders are manually triggered IO, not always running, so
        '''
        object.__init__(self)

        # Hold a config for each process stream (for example, "train" or "test")
        self.config = {}
        self._queue_processor = {}
        self._batch_holder    = {}


    def add_data_stream(self, configure):


