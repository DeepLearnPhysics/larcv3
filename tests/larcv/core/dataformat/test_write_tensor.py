import pytest
import unittest
import larcv

from larcv import data_generator

from random import Random
random = Random()

@pytest.fixture()
def rand_num_events():
    return random.randint(1, 5)


@pytest.mark.parametrize('dimension', [1, 2, 3])
@pytest.mark.parametrize('n_projections', [1,2,3])
def test_write_tensor(tmpdir, rand_num_events, dimension, n_projections):

    event_image_list = data_generator.build_tensor(rand_num_events, n_projections=n_projections, dimension=dimension)

    random_file_name = str(tmpdir + "/test_write_tensor.h5")

    data_generator.write_tensor(random_file_name, event_image_list, dimension)


@pytest.mark.parametrize('dimension', [1, 2, 3])
@pytest.mark.parametrize('n_projections', [1,2,3])
def test_write_read_tensor(tmpdir, rand_num_events, dimension, n_projections):

    import numpy

    random_file_name = str(tmpdir + "/test_write_read_tensor.h5")

    event_image_list = data_generator.build_tensor(rand_num_events, n_projections=n_projections, dimension=dimension)

    data_generator.write_tensor(random_file_name, event_image_list, dimension)

    read_event_image_list = data_generator.read_tensor(random_file_name, dimension)

    # Check the same number of events came back:
    assert(len(read_event_image_list) == rand_num_events)
    for event in range(rand_num_events):
        # Check the same number of projections per event:
        assert(len(read_event_image_list[event]) == len(event_image_list[event]))
        for projection in range(n_projections):
            # Check the same number of voxels:
            input_image = event_image_list[event][projection]
            read_image = read_event_image_list[event][projection]

            assert(numpy.sum(input_image) - numpy.sum(read_image) < 1e-3)
            
            assert(numpy.sum(numpy.fabs(input_image)) - numpy.sum(numpy.fabs(read_image)) < 1e-3)

            assert(input_image.shape == read_image.shape)

            assert(numpy.mean(input_image - read_image) < 1e-3)

            
            break
        break





if __name__ == '__main__':
    tmpdir = "./"
    rand_num_events = 3
    dimension = 1
    n_projections = 3
    # test_write_tensor(tmpdir, rand_num_events, dimension, n_projections)
    test_write_read_tensor(tmpdir, rand_num_events, dimension, n_projections)

