import sys


# TO be able to use both 2 and 3 simultaneously, need to remove the python dir from the sys.path of 2:
for p in sys.path:
    if "larcv2/python" in p:
        sys.path.remove(p)



from ROOT import larcv as larcv2
# print(larcv2)
larcv2.load_pyutil()

import larcv as larcv3
from larcv import dataformat, pyutil

def main(in_file_name, out_file_name, n_events = -1):
    # main loop, convert all possible dataproducts.

    # This script is not meant to be lightning fast, since it's a 1time cost.



    io_manager_2 = larcv2.IOManager()
    io_manager_2.add_in_file(in_file_name)
    io_manager_2.initialize()

    io_manager_3 = dataformat.IOManager(dataformat.IOManager.kWRITE)
    io_manager_3.set_out_file(out_file_name)
    io_manager_3.initialize()

    if n_events == -1:
        n_events = io_manager_2.get_n_entries()

    for i in range(n_events):
        convert_event(io_manager_2, io_manager_3, i)
        io_manager_3.set_id(io_manager_2.event_id().run(), io_manager_2.event_id().subrun(), io_manager_2.event_id().event())
        io_manager_3.save_entry()

    io_manager_3.finalize()

def convert_event(io_manager_2, io_manager_3, entry):

    print()
    print("Converting entry ", entry)

    # Read the entry for 2:
    io_manager_2.read_entry(entry)



    # Get the event id and set it:

    product_list = {}


    for product in io_manager_2.product_list():

        for producer in io_manager_2.producer_list(product):
            print(producer, product)
            if product not in product_list.keys():
                product_list[product] = []

            if producer not in product_list[product]:
                if product == "image2d":
                    convert_image2d(io_manager_2, io_manager_3, producer)

                product_list[product].append(producer)

    print(io_manager_2.event_id().event())

def convert_image2d(io_manager_2, io_manager_3, producer):
    # Get the image2d from the input file:
    input_image_2d = io_manager_2.get_data('image2d', producer)
    output_image_2d = dataformat.EventImage2D.to_image2d(io_manager_3.get_data('image2d', producer))

    for image in input_image_2d.as_vector():
        input_image = larcv2.as_ndarray(image).T
        
        # Create a larcv3 meta for this:
        meta = dataformat.ImageMeta2D()
        # print()
        meta.set_dimension(0, image.meta().width(),  image.meta().cols(), image.meta().min_x())
        meta.set_dimension(1, image.meta().height(), image.meta().rows(), image.meta().min_y())
        meta.set_projection_id(image.meta().id())
        # print(meta.dump())

        # print(image.meta())
        # print(input_image.shape)

        # Create a new image from the meta:
        new_image = pyutil.as_image2d_meta(input_image, meta)

        # Set the new image data:
        output_image_2d.emplace(new_image)
        # 

    pass

def convert_particle(io_manager_2, io_manager_3, producer):
    pass

def convert_sparse2d(io_manager_2, io_manager_3, producer):
    pass

def convert_sparse3d(io_manager_2, io_manager_3, producer):
    pass

def convert_cluster2d(io_manager_2, io_manager_3, producer):
    pass

def convert_cluster3d(io_manager_2, io_manager_3, producer):
    pass

if __name__ == '__main__':
    in_file  = "/Users/corey.adams/data/sbnd_cosmic_samples/unprocessed_samples/sbnd_dl_nueCC_larcv_dev.root"
    out_file = "./sbnd_dl_nueCC_larcv_dev.h5"
    main(in_file, out_file, 5)