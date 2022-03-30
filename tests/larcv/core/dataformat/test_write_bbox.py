import pytest
import unittest
import larcv

from random import Random
random = Random()

from larcv import data_generator

@pytest.fixture()
def rand_num_events():
    return random.randint(5, 100)



@pytest.mark.parametrize('dimension', [2, 3])
@pytest.mark.parametrize('n_projections', [1, 2, 3])
def test_write_bboxes(tmpdir, rand_num_events, dimension, n_projections):

    bbox_list, meta_list = data_generator.create_bbox_list(rand_num_events, n_projections = n_projections, dimension =dimension)


    random_file_name = str(tmpdir + "/test_write_bboxes.h5")

    data_generator.write_bboxes(random_file_name, bbox_list, meta_list, dimension, n_projections)






@pytest.mark.parametrize('dimension', [2, 3])
@pytest.mark.parametrize('n_projections', [1, 2, 3])
def test_read_write_bboxes(tmpdir, rand_num_events, dimension, n_projections):

    import numpy

    random_file_name = str(tmpdir + "/test_write_read_bboxes.h5")

    bbox_list, meta_list = data_generator.create_bbox_list(rand_num_events, n_projections = n_projections, dimension =dimension)

    print(meta_list)

    data_generator.write_bboxes(random_file_name, bbox_list, meta_list, dimension, n_projections)
    read_bbox_list = data_generator.read_bboxes(random_file_name, dimension)

    # print(meta_list)
    # print(read_bbox_list[0][0].meta())
    # print([r.meta() for r in read_bbox_list])

    # print("Input BBoxes:")
    # for event in range(len(bbox_list)):
    #     for projection_id in range(len(bbox_list[event])):
    #         print()
    #         print("Projection: ", projection_id)
    #         print(bbox_list[event][projection_id])

    # print("Read BBoxes:")
    # for event in range(len(read_bbox_list)):
    #     for projection_id in range(len(read_bbox_list[event])):
    #         print()
    #         print("Projection: ", projection_id)
    #         print([b for b in read_bbox_list[event][projection_id].as_vector() ])


    # Check the same number of events came back:
    assert(len(read_bbox_list) == rand_num_events)
    for event in range(rand_num_events):
        # Check the same number of projections per event:
        assert(len(read_bbox_list[event]) == len(bbox_list[event]))
        for projection in range(n_projections):

            print(meta_list[event][projection])
            print(read_bbox_list[event][projection].meta())

            # Check the meta matches:
            assert meta_list[event][projection].total_voxels() == read_bbox_list[event][projection].meta().total_voxels()
            assert meta_list[event][projection].total_volume() == read_bbox_list[event][projection].meta().total_volume()

            # Check the same number of voxels:
            this_input_bbox_list = bbox_list[event][projection]
            this_read_bbox_list  = read_bbox_list[event][projection]
            # print(this_read_bbox_list.size())
            # print(len(this_input_bbox_list))
            # print(this_input_bbox_list)
            # print(this_read_bbox_list.bbox(0))
            assert(this_read_bbox_list.size() == len(this_input_bbox_list))

            for i_box in range(len(this_input_bbox_list)):
                for i in range(dimension):
                    print(this_input_bbox_list[i_box]['half_length'][i]) 
                    print(this_read_bbox_list.bbox(i_box).half_length()[i])
                    assert abs(this_read_bbox_list.bbox(i_box).half_length()[i] - this_input_bbox_list[i_box]['half_length'][i]) < 1e-3
                    assert abs(this_read_bbox_list.bbox(i_box).centroid()[i] - this_input_bbox_list[i_box]['centroid'][i]) < 1e-3



if __name__ == '__main__':
    tmpdir = "./"
    rand_num_events = 2
    n_projections = 3
    dimension = 2
    # test_write_bboxes(tmpdir, rand_num_events, dimension, n_projections)
    test_read_write_bboxes(tmpdir, rand_num_events, dimension, n_projections)








