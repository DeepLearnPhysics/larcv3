import sys


# TO be able to use both 2 and 3 simultaneously, need to remove the python dir from the sys.path of 2:
for p in sys.path:
    if "larcv2" in p: 
        sys.path.remove(p)


from ROOT import larcv as larcv2

import larcv as larcv3
# from larcv import dataformat, pyutil


def get_larcv2_io_manager(file_name):

    from ROOT import larcv as larcv2
    # print(larcv2)
    larcv2.load_pyutil()
    print(larcv2)

    io_manager_2 = larcv2.IOManager()
    print(io_manager_2)
    io_manager_2.add_in_file(file_name)
    print(file_name)
    io_manager_2.initialize()
    return io_manager_2

def main(in_file_name, out_file_name, n_events = -1):
    # main loop, convert all possible dataproducts.

    # This script is not meant to be lightning fast, since it's a 1time cost.

    io_manager_2 = get_larcv2_io_manager(in_file_name)

    io_manager_3 = larcv3.dataformat.IOManager(larcv3.dataformat.IOManager.kWRITE)
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

                if product == "particle":
                    convert_particle(io_manager_2, io_manager_3, producer)


                product_list[product].append(producer)

    print(io_manager_2.event_id().event())

def convert_image2d(io_manager_2, io_manager_3, producer):
    # Get the image2d from the input file:
    input_image_2d = io_manager_2.get_data('image2d', producer)
    output_image_2d = larcv3.dataformat.EventImage2D.to_image2d(io_manager_3.get_data('image2d', producer))

    for image in input_image_2d.as_vector():
        input_image = larcv2.as_ndarray(image).T
        
        # Create a larcv3 meta for this:
        meta = larcv3.dataformat.ImageMeta2D()
        # print()
        meta.set_dimension(0, image.meta().width(),  image.meta().cols(), image.meta().min_x())
        meta.set_dimension(1, image.meta().height(), image.meta().rows(), image.meta().min_y())
        meta.set_projection_id(image.meta().id())
        # print(meta.dump())

        # print(image.meta())
        # print(input_image.shape)

        # Create a new image from the meta:
        new_image = larcv3.pyutil.as_image2d_meta(input_image, meta)

        # Set the new image data:
        output_image_2d.emplace(new_image)
        # 


def convert_particle(io_manager_2, io_manager_3, producer):

    # Get the particles from the input and output file:
    input_particle = io_manager_2.get_data('particle', producer)
    output_particle = larcv3.dataformat.EventParticle.to_particle(io_manager_3.get_data('particle', producer))

    for particle in input_particle.as_vector():
        new_particle = larcv3.dataformat.Particle()

        new_particle.id(particle.id())
        new_particle.mcst_index(particle.mcst_index())
        new_particle.mct_index(particle.mct_index())
        new_particle.shape(particle.shape())
        new_particle.nu_current_type(particle.nu_current_type())
        new_particle.nu_interaction_type(particle.nu_interaction_type())
        new_particle.track_id(particle.track_id())
        new_particle.pdg_code(particle.pdg_code())
        new_particle.momentum(particle.px(), particle.py(), particle.pz())
        new_particle.position(particle.position().x(), particle.position().y(), particle.position().z(), particle.position().t())
        new_particle.end_position(particle.end_position().x(), particle.end_position().y(), particle.end_position().z(), particle.end_position().t())
        new_particle.first_step(particle.first_step().x(), particle.first_step().y(), particle.first_step().z(), particle.first_step().t())
        new_particle.last_step(particle.last_step().x(), particle.last_step().y(), particle.last_step().z(), particle.last_step().t())
        new_particle.distance_travel(particle.distance_travel())
        new_particle.energy_init(particle.energy_init())
        new_particle.energy_deposit(particle.energy_deposit())
        new_particle.creation_process(particle.creation_process())
        new_particle.parent_track_id(particle.parent_track_id())
        new_particle.parent_pdg_code(particle.parent_pdg_code())
        new_particle.parent_position(particle.parent_position().x(), particle.parent_position().y(), particle.parent_position().z(), particle.parent_position().t())
        new_particle.ancestor_track_id(particle.ancestor_track_id())
        new_particle.ancestor_pdg_code(particle.ancestor_pdg_code())
        new_particle.ancestor_position(particle.ancestor_position().x(), particle.ancestor_position().y(), particle.ancestor_position().z(), particle.ancestor_position().t())

# BBox is not really supported yet
# boundingbox_2d
# boundingbox_3d

        output_particle.emplace_back(new_particle)
        


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