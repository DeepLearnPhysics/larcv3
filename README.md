[![Build Status](https://travis-ci.org/DeepLearnPhysics/larcv2.svg?branch=develop)](https://travis-ci.org/DeepLearnPhysics/larcv2) [![license](https://img.shields.io/github/license/mashape/apistatus.svg)](https://raw.githubusercontent.com/DeepLearnPhysics/larcv2/develop/LICENSE) [![https://www.singularity-hub.org/static/img/hosted-singularity--hub-%23e32929.svg](https://www.singularity-hub.org/static/img/hosted-singularity--hub-%23e32929.svg)](https://singularity-hub.org/collections/459) [![https://img.shields.io/docker/build/deeplearnphysics/ml-larcv2.svg](https://img.shields.io/docker/build/deeplearnphysics/ml-larcv2.svg)](https://hub.docker.com/r/deeplearnphysics/ml-larcv2/)


# LArCV (Version 3)
Software framework for image(2D)/volumetric(3D) data processing with APIs to interface deep neural network open-source softwares, written in C++ with extensive Python supports.  Originally developed for analyzing data from [time-projection-chamber (TPC)](https://en.wikipedia.org/wiki/Time_projection_chamber). It is then converted to be a generic tool to handle 2D-projected images and 3D-voxelized data. LArCV is particularly suitable for sparse data processing.

***Note for larcv2*** This repository (referring to larcv2) is re-created from LArbys/LArCV repository, referred to as larbys version. The larbys version is still under active development for analysis purpose in MicroBooNE experiment. This repository is split for more generic technical R&D work in October 2017.

***Note for larcv3*** This repository (referring to larcv3) is forked by coreyjadams in early 2019 to remove ROOT dependencies.  It is still under active development but is in "beta" release as the python bindings, build system, dataformat are all working.  The training interface works, and seems to perform reasonably but there have not been benchmark comparisons yet.

## Installation

### Dependencies

* Python
* Numpy
* Swig (for python bindings)
* HDF5 (for IO)
* cmake (for building)
* scikit-build (for installation)
* pytest (for continuous integration)

Swig, hdf5 and cmake can all be installed by package managers.  Conda will also work.

### Setup


1. Clone & build
```
git clone https://github.com/DeepLearnPhysics/larcv2.git
python setup.py build [-j 12]
python setup.py install [--user]
```
That's it. When you want to use the built larcv from a different process, as long as you are using the same python it will just work.

If you want it to be even EASIER, and you have the dependencies installed, you can do "pip install -e larcv3 [--user]" from the level above the repo.

## Wiki

Checkout the [Wiki](https://github.com/DeepLearnPhysics/larcv2/wiki) for notes on using this code.
