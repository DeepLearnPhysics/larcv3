# import ROOT,os
# if not 'LARCV_BASEDIR' in os.environ:
#     print('$LARCV_BASEDIR shell env. var. not found (run configure.sh)')
#     raise ImportError
# #force loading larlite libs FIRST because auto-loading in ROOT6 does not properly work
# if 'LARLITE_BASEDIR' in os.environ:
#     from larlite import larlite
# if 'LAROPENCV_BASEDIR' in os.environ:
#     from larocv import larocv
# larcv_dir = os.environ['LARCV_LIBDIR']
# for l in [x for x in os.listdir(larcv_dir) if x.endswith('.so')]:
#     ROOT.gSystem.Load(l)
# from ROOT import larcv
# larcv.Vertex
# larcv.CSVData
# ##k=larcv.logger # this line to load C++ functions
# if 'LARCV_NUMPY' in os.environ and os.environ['LARCV_NUMPY'] == '1':
#     larcv.load_pyutil
# if 'LARCV_OPENCV' in os.environ and os.environ['LARCV_OPENCV'] == '1':
#     larcv.load_cvutil
# #larcv.LoadImageMod

# import os

# # Add the location of the libraries to ld library path:
# extra_ld_path = ":{}/lib/".format(os.path.dirname(os.path.abspath(__file__)))
# if 'LD_LIBRARY_PATH' in os.environ:
#     os.environ['LD_LIBRARY_PATH'] = os.environ['LD_LIBRARY_PATH'] + extra_ld_path
# else:
#     os.environ['LD_LIBRARY_PATH'] = extra_ld_path

# if 'DYLD_LIBRARY_PATH' in os.environ:
#     os.environ['DYLD_LIBRARY_PATH'] = os.environ['DYLD_LIBRARY_PATH'] + extra_ld_path
# else:
#     os.environ['DYLD_LIBRARY_PATH'] = extra_ld_path

from . import base
from . import dataformat
from . import processor
from . import pyutil