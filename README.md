[![Tests](https://github.com/DeepLearnPhysics/larcv3/actions/workflows/test.yaml/badge.svg)](https://github.com/DeepLearnPhysics/larcv3/actions/workflows/test.yaml) [![license](https://img.shields.io/github/license/mashape/apistatus.svg)](https://raw.githubusercontent.com/DeepLearnPhysics/larcv2/develop/LICENSE) 

# LArCV (Version 3)
Software framework for image(2D)/volumetric(3D) data processing with APIs to interface deep neural network open-source softwares, written in C++ with extensive Python supports.  Originally developed for analyzing data from [time-projection-chamber (TPC)](https://en.wikipedia.org/wiki/Time_projection_chamber). It is now converted to be a generic tool to handle 2D-projected images and 3D-voxelized data. LArCV is particularly suitable for sparse data processing.


## Installation

You can install larcv through pypi: `pip install larcv` and it should work.  You can also build from source:

```bash
git clone https://github.com/DeepLearnPhysics/larcv3.git
cd larcv3
git submodule update --init  # Pulls pybind11 subpackage
python setup.py build [-j 12] # Optional parallel build for faster compilation
python setup.py install [--user | -prefix ${INSTALLATION_DIR} ] 
```

To verify your larcv installation, after install has completed:
```
cd larcv3/tests
py.test .
```


### Dependencies

* Python
* Numpy
* HDF5 (for IO)
* cmake (for building)
* scikit-build (for installation)
* pytest (for continuous integration)

HDF5 and cmake can all be installed by package managers.  Conda will also work.

For compilation, a gcc > 4.8 is required.  GCC versions 5 to 8 are all known to work, as is clang on MacOS.

To install requirements on ubuntu, you can do:
sudo apt-get install cmake libhdf5-serial-dev python-dev
pip install numpy scikit-build pytest

To install requirements on mac, you can do:
sudo port install cmake hdf5
pip install numpy scikit-build pytest

To install in a generic system, you can try conda or a virtual environment.  It has been shown to work on many linux distributions.

<!-- ## Wiki -->

<!-- Checkout the [Wiki](https://github.com/DeepLearnPhysics/larcv3/wiki) for notes on using this code. -->


## Compatibility

larcv3 works on mac and many flavors of linux.  It has never been tested on windows as far as I know.  If you try to install and need help, please open an Issue.

### Use Cases

Larcv is predominantly used as an IO framework and data preprocessing tool for machine learning and deep learning.  It has run on many systems and in many scenarios.  Larcv has a suite of test cases available that test the serialization, read back, threaded IO tools, and distributed IO tools.

Larcv has run on some of the biggest systems in the world, including Summit (ORNL) and Theta (ANL).  It has been used for distributed io of sparse, non-uniform data up to hundreds of CPUs/GPUs, and had good performance.

If you would like to use larcv for your application and want to benchmark the performance, you are welcome to use the larcv3 open dataset (more info on deeplearnphysics.org) and if you would like help, open an issue or contact the authors directly.





