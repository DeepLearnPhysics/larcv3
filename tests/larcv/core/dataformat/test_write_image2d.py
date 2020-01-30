# import pytest
# import unittest
# import larcv

# from larcv import data_generator

# from random import Random
# random = Random()

# # 
# # For backward compatibility, otherwise see test_write_tensor.py
# # 

# @pytest.fixture()
# def rand_num_events():
#     return random.randint(1, 5)


# # def write_image2d(tempfile, event_image_list):

    

# #     io_manager = larcv.IOManager(larcv.IOManager.kWRITE)
# #     io_manager.set_out_file(tempfile)
# #     io_manager.initialize()


        
# #     for event in range(len(event_image_list)):
# #         io_manager.set_id(1001, 0, event)
# #         images = event_image_list[event]

# #         # Get a piece of data, image2d:
# #         ev_image2d = larcv.EventImage2D.to_image2d(io_manager.get_data("image2d","test"))

# #         for projection in range(len(images)):

# #             image2d = larcv.as_image2d(images[projection])
# #             ev_image2d.append(image2d)

# #         io_manager.save_entry()


# #     print("Finished event loop")



# #     # assert(io_manager.get_n_entries_out() == rand_num_events)

# #     io_manager.finalize()

# #     return
 
# # def read_image2d(tempfile):

    
# #     from copy import copy

# #     io_manager = larcv.IOManager(larcv.IOManager.kREAD)
# #     io_manager.add_in_file(tempfile)
# #     io_manager.initialize()


# #     event_image_list = []
     
# #     for i in range(io_manager.get_n_entries()):
# #         event_image_list.append([])

# #         io_manager.read_entry(i)
        
# #         # Get a piece of data, sparse tensor:
# #         ev_image2d = larcv.EventImage2D.to_image2d(io_manager.get_data("image2d","test"))
# #         print("Number of images read: ", ev_image2d.as_vector().size())
# #         for projection in range(ev_image2d.as_vector().size()):
# #             image = larcv.as_ndarray(ev_image2d.as_vector()[projection])
# #             event_image_list[i].append(copy(image))


# #     return event_image_list

# # def build_image2d(rand_num_events, n_projections):
# #     from larcv import larcv
# #     import numpy

# #     event_image_list = []

# #     for i in range(rand_num_events):
# #         event_image_list.append([])

# #         # Get a piece of data, image2d:
# #         for projection in range(n_projections):
# #             shape = []
# #             for dim in range(2):
# #                 shape.append(random.randint(1, 1e3))
# #             raw_image = numpy.random.random(shape).astype("float32")
# #             event_image_list[i].append(raw_image)

# #     return event_image_list


# @pytest.mark.parametrize('n_projections', [1,2,3])
# def test_write_image2d(tmpdir, rand_num_events, n_projections):

#     event_image_list = data_generator.build_image2d(rand_num_events, n_projections = n_projections)

#     random_file_name = str(tmpdir + "/test_write_image2d.h5")

#     data_generator.write_image2d(random_file_name, event_image_list)


# @pytest.mark.parametrize('n_projections', [1,2,3])
# def test_write_read_image2d(tmpdir, rand_num_events, n_projections):

#     import numpy

#     random_file_name = str(tmpdir + "/test_write_read_image2d.h5")

#     event_image_list = data_generator.build_image2d(rand_num_events, n_projections = n_projections)

#     data_generator.write_image2d(random_file_name, event_image_list)
#     read_event_image_list = data_generator.read_image2d(random_file_name)

#     # Check the same number of events came back:
#     assert(len(read_event_image_list) == rand_num_events)
#     for event in range(rand_num_events):
#         # Check the same number of projections per event:
#         assert(len(read_event_image_list[event]) == len(event_image_list[event]))
#         for projection in range(n_projections):
#             # Check the same number of voxels:
#             input_image = event_image_list[event][projection]
#             read_image = read_event_image_list[event][projection]

#             assert(numpy.sum(input_image) - numpy.sum(read_image) < 1e-3)
#             assert(numpy.sum(numpy.fabs(input_image)) - numpy.sum(numpy.fabs(read_image)) < 1e-3)

#             assert(input_image.shape == read_image.shape)

#             assert(numpy.mean(input_image - read_image) < 1e-3)





# if __name__ == '__main__':
#     tmpdir = "./"
#     rand_num_events = 10
#     n_projections = 3
#     test_write_read_image2d(tmpdir, rand_num_events, n_projections)

