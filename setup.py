from skbuild import setup  # This line replaces 'from setuptools import setup'
import argparse

from os import path
import io
this_directory = path.abspath(path.dirname(__file__))
with io.open(path.join(this_directory, 'README.md'), encoding='utf-8') as f:
    long_description = f.read()



setup(
    name="larcv",
    version="3.0.0",
    cmake_source_dir='src/',
    cmake_args=[
        '-DCMAKE_OSX_DEPLOYMENT_TARGET:STRING=10.9',
        '-DCMAKE_NO_SWIG=False',
        '-DMPI:BOOL=OFF',
        '-DOPENMP:BOOL=OFF',
    ],
    include_package_data=True,
    author=['Corey Adams', 'Kazuhiro Terao', 'Taritree Wongjirad', 'Marco del Tutto'],
    description='C++ IO and Preprocessing package for sparse neutrino data, with H5 for IO and python bindings.',
    license='MIT',
    keywords='larcv larcv3 neutrinos hdf5 h5 deep learning IO sparse',
    project_urls={
        'Source Code': 'https://github.com/DeepLearnPhysics/larcv3'
    },
    scripts=['bin/merge_larcv3_files.py', 'bin/run_processor.py'],
    packages=['larcv'],   
    install_requires=[
        'numpy',
        'scikit-build',
    ],
    long_description=long_description,
    long_description_content_type='text/markdown',
)
