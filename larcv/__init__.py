
from . pylarcv import *

import os
from . _version import __version__

from . config_builder import ConfigBuilder

def get_includes():
    return os.path.dirname(__file__) + "/include/"

def get_lib_dir():
    return os.path.dirname(__file__) + "/lib/"
