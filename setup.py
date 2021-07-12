from skbuild import setup  # This line replaces 'from setuptools import setup'
import argparse

import io
import os
this_directory = os.path.abspath(os.path.dirname(__file__))
with io.open(os.path.join(this_directory, 'README.md'), encoding='utf-8') as f:
    long_description = f.read()

# Several environment variables control special build features:
if 'LARCV_WITH_MPI' in os.environ and os.environ['LARCV_WITH_MPI']:
    mpi_value='ON'
else:
    mpi_value='OFF'

if 'LARCV_WITH_OPENMP' in os.environ and os.environ['LARCV_WITH_OPENMP']:
    openmp_value='ON'
else:
    openmp_value='OFF'

if 'LARCV_WITHOUT_PYBIND' in os.environ and os.environ['LARCV_WITHOUT_PYBIND']:
    pybind_value='OFF'
else:
    pybind_value='ON'


setup(
    name="larcv",
    version="3.3.4",
    cmake_source_dir='src/',
    use_scm_version=True,
    include_package_data=True,
    cmake_args=[
        '-DCMAKE_OSX_DEPLOYMENT_TARGET:STRING=10.9',
        '-DCMAKE_PYTHON_BINDINGS={}'.format(pybind_value),
        # '-DMPI_CXX_COMPILER={}'.format(mpicxx),
        # '-DMPI_C_COMPILER={}'.format(mpicc),
        '-DMPI:BOOL={}'.format(mpi_value),
        '-DOPENMP:BOOL={}'.format(openmp_value),
    ],
    author=['Corey Adams', 'Kazuhiro Terao', 'Taritree Wongjirad', 'Marco del Tutto'],
    author_email='corey.adams@anl.gov',
    description='C++ IO and Preprocessing package for sparse neutrino data, with H5 for IO and python bindings.',
    license='MIT',
    keywords='larcv larcv3 neutrinos hdf5 h5 deep learning IO sparse',
    project_urls={
        'Source Code': 'https://github.com/DeepLearnPhysics/larcv3'
    },
    url='https://github.com/DeepLearnPhysics/larcv3',
    scripts=['bin/merge_larcv3_files.py', 'bin/run_processor.py'],
    packages=['larcv','src/pybind11'],
    install_requires=[
        'numpy',
        'scikit-build',
    ],
    long_description=long_description,
    long_description_content_type='text/markdown',
)
