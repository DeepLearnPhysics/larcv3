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

                if product == "sparse2d":
                    convert_sparse2d(io_manager_2, io_manager_3, producer)

                if product == "sparse3d":
                    convert_sparse3d(io_manager_2, io_manager_3, producer)

                if product == "cluster2d":
                    convert_cluster2d(io_manager_2, io_manager_3, producer)

                if product == "cluster3d":
                    convert_cluster3d(io_manager_2, io_manager_3, producer)

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
    
    # Get the particles from the input and output file:
    input_sparse2d = io_manager_2.get_data('sparse2d', producer)
    output_sparse2d = larcv3.dataformat.EventSparseTensor2D.to_sparse_tensor(io_manager_3.get_data('sparse2d', producer))


    for sparse2d in input_sparse2d.as_vector():
        
        # Create a larcv3 meta for this:
        meta = larcv3.dataformat.ImageMeta2D()
        # print()
        meta.set_dimension(0, sparse2d.meta().width(),  sparse2d.meta().cols(), sparse2d.meta().min_x())
        meta.set_dimension(1, sparse2d.meta().height(), sparse2d.meta().rows(), sparse2d.meta().min_y())
        meta.set_projection_id(sparse2d.meta().id())

        # Create a place to hold the output sparse2d:
        st = larcv3.dataformat.SparseTensor()
        st.meta(meta)


        # Convert all of the voxels:
        for original_voxel in sparse2d.as_vector():
            vec_of_coords = larcv3.dataformat.VectorOfSizet()
            vec_of_coords.push_back(sparse2d.meta().index_to_row(original_voxel.id()))
            vec_of_coords.push_back(sparse2d.meta().index_to_col(original_voxel.id()))
            new_index = meta.index(vec_of_coords)
            st.emplace(larc3.dataformat.Voxel(new_index, original_voxel.value()))


        # Set the new image data:
        output_sparse2d.emplace(st)
        # 


    pass

def convert_sparse3d(io_manager_2, io_manager_3, producer):
# Get the particles from the input and output file:
    input_sparse3d = io_manager_2.get_data('sparse3d', producer)
    output_sparse3d = larcv3.dataformat.EventSparseTensor3D.to_sparse_tensor(io_manager_3.get_data('sparse3d', producer))


    for sparse3d in input_sparse3d.as_vector():
        
        # Create a larcv3 meta for this:
        meta = larcv3.dataformat.ImageMeta3D()
        # print()
        meta.set_dimension(0, sparse3d.meta().width(),  sparse3d.meta().num_voxel_x(), sparse3d.meta().min_x())
        meta.set_dimension(1, sparse3d.meta().height(), sparse3d.meta().num_voxel_y(), sparse3d.meta().min_y())
        meta.set_dimension(2, sparse3d.meta().depth(), sparse3d.meta().num_voxel_z(), sparse3d.meta().min_z())
        meta.set_projection_id(sparse3d.meta().id())

        # Create a place to hold the output sparse3d:
        st = larcv3.dataformat.SparseTensor()
        st.meta(meta)


        # Convert all of the voxels:
        for original_voxel in sparse3d.as_vector():
            vec_of_coords = larcv3.dataformat.VectorOfSizet()
            vec_of_coords.push_back(sparse3d.meta().id_to_x_index(original_voxel.id()))
            vec_of_coords.push_back(sparse3d.meta().id_to_y_index(original_voxel.id()))
            vec_of_coords.push_back(sparse3d.meta().id_to_z_index(original_voxel.id()))
            new_index = meta.index(vec_of_coords)
            st.emplace(larc3.dataformat.Voxel(new_index, original_voxel.value()))

        # Set the new image data:
        output_sparse3d.emplace(st)
        # 



def convert_cluster2d(io_manager_2, io_manager_3, producer):
    # Get the particles from the input and output file:
    input_cluster2d = io_manager_2.get_data('cluster2d', producer)
    output_cluster2d = larcv3.dataformat.EventSparseCluster2D.to_sparse_cluster(io_manager_3.get_data('cluster2d', producer))


    for cluster2d_set in input_cluster2d.as_vector():
        
        # Create a larcv3 meta for this:
        meta = larcv3.dataformat.ImageMeta2D()
        # print()
        meta.set_dimension(0, cluster2d_set.meta().width(),  cluster2d_set.meta().cols(), cluster2d_set.meta().min_x())
        meta.set_dimension(1, cluster2d_set.meta().height(), cluster2d_set.meta().rows(), cluster2d_set.meta().min_y())
        meta.set_projection_id(cluster2d_set.meta().id())

        # Create a place to hold the output cluster2d:
        output_cluster2d_set = larcv3.dataformat.SparseCluster2D()
        output_cluster2d_set.meta(meta)

        for cluster in cluster2d_set.as_vector():
            #holder for new cluster:
            vs = larcv3.dataformat.VoxelSet()

            # Convert all of the voxels:
            for original_voxel in cluster.as_vector():
                vec_of_coords = larcv3.dataformat.VectorOfSizet()
                vec_of_coords.push_back(cluster2d_set.meta().index_to_row(original_voxel.id()))
                vec_of_coords.push_back(cluster2d_set.meta().index_to_col(original_voxel.id()))
                new_index = meta.index(vec_of_coords)
                vs.emplace(new_index, original_voxel.value(), False)
           
            output_cluster2d_set.emplace(vs)

            

        # Set the new image data:
        output_cluster2d.emplace(output_cluster2d_set)
        # 

def convert_cluster3d(io_manager_2, io_manager_3, producer):
    # Get the particles from the input and output file:
    input_cluster3d = io_manager_2.get_data('cluster3d', producer)
    output_cluster3d = larcv3.dataformat.EventSparseCluster3D.to_sparse_cluster(io_manager_3.get_data('cluster3d', producer))

    print(output_cluster3d)
    print(input_cluster3d)

    i = 0
    for cluster3d_set in input_cluster3d.as_vector():
        
        # Create a larcv3 meta for this:
        meta = larcv3.dataformat.ImageMeta3D()


        meta.set_dimension(0, input_cluster3d.meta().width(),  input_cluster3d.meta().num_voxel_x(), input_cluster3d.meta().min_x())
        meta.set_dimension(1, input_cluster3d.meta().height(), input_cluster3d.meta().num_voxel_y(), input_cluster3d.meta().min_y())
        meta.set_dimension(2, input_cluster3d.meta().depth(),  input_cluster3d.meta().num_voxel_z(), input_cluster3d.meta().min_z())
        meta.set_projection_id(i)

        # Create a place to hold the output cluster3d:
        output_cluster3d_set = larcv3.dataformat.SparseCluster3D()
        output_cluster3d_set.meta(meta)

        # larcv2 only supports one 3D volume

        #holder for new cluster:
        vs = larcv3.dataformat.VoxelSet()

        for original_voxel in cluster3d_set.as_vector():
            if original_voxel.id() > meta.total_voxels(): continue

            # Convert all of the voxels:
            vec_of_coords = larcv3.dataformat.VectorOfSizet()
            vec_of_coords.push_back(input_cluster3d.meta().id_to_x_index(original_voxel.id()))
            vec_of_coords.push_back(input_cluster3d.meta().id_to_y_index(original_voxel.id()))
            vec_of_coords.push_back(input_cluster3d.meta().id_to_z_index(original_voxel.id()))
            new_index = meta.index(vec_of_coords)
            vs.emplace(new_index, original_voxel.value(), False)
           
            output_cluster3d_set.emplace(vs)

            

        # Set the new image data:
        output_cluster3d.emplace(output_cluster3d_set)
        # 
    i += 1


if __name__ == '__main__':
    in_file  = "/Users/corey.adams/data/sbnd_cosmic_samples/unprocessed_samples/sbnd_dl_nueCC_larcv_dev.root"
    out_file = "./sbnd_dl_nueCC_larcv_dev.h5"
    main(in_file, out_file, 5)