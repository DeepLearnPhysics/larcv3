from skbuild import setup  # This line replaces 'from setuptools import setup'
import argparse




setup(
    name="larcv",
    version="3.0a01",
    cmake_source_dir='src/',
    cmake_args=[
        '-DCMAKE_OSX_DEPLOYMENT_TARGET:STRING=10.9',
        '-DCMAKE_NO_SWIG=False'
    ],
    # extra_link_args=["-stdlib=libc++", "-mmacosx-version-min=10.9"],
    include_package_data=True,
    author=['Corey Adams', 'Kazuhiro Terao', 'Taritree Wongjirad'],
    description='C++ IO and Preprocessing package for sparse neutrino data, with H5 for IO and python bindings.',
    license='MIT',
    keywords='larcv larcv3 neutrinos hdf5 h5 deep learning',
    project_urls={
        'Source Code': 'https://github.com/coreyjadams/larcv2'
    },
    scripts=['bin/merge_larcv3_files.py'],
    packages=['larcv'],   
)