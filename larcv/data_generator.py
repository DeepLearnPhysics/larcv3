import numpy

import larcv

from random import Random
random = Random()


 
def write_tensor(file_name, event_image_list, dimension): 

    io_manager = larcv.IOManager(larcv.IOManager.kWRITE)
    io_manager.set_out_file(file_name)
    io_manager.initialize()

    for event in range(len(event_image_list)):
        io_manager.set_id(1001, 0, event)
        images = event_image_list[event]

        if dimension == 1:
            ev_tensor = io_manager.get_data("tensor1d","test")
        if dimension == 2:
            ev_tensor = io_manager.get_data("image2d","test")
        if dimension == 3:
            ev_tensor = io_manager.get_data("tensor3d","test")
        if dimension == 4:
            ev_tensor = io_manager.get_data("tensor4d","test")


        for projection in range(len(images)):
            if dimension == 1: tensor = larcv.Tensor1D(images[projection])
            if dimension == 2: tensor = larcv.Tensor2D(images[projection])
            if dimension == 3: tensor = larcv.Tensor3D(images[projection])
            if dimension == 4: tensor = larcv.Tensor4D(images[projection])

            ev_tensor.append(tensor)

        io_manager.save_entry()

    io_manager.finalize()

    return

def read_tensor(file_name, dimensions):

    
    from copy import copy

    io_manager = larcv.IOManager(larcv.IOManager.kREAD)
    io_manager.add_in_file(file_name)
    io_manager.initialize()

    event_image_list = []
     
    for i in range(io_manager.get_n_entries()):
        event_image_list.append([])

        io_manager.read_entry(i)
        
        # Get a piece of data, sparse tensor:
        if dimensions == 1:
            ev_tensor = io_manager.get_data("tensor1d","test")
        if dimensions == 2:
            ev_tensor = io_manager.get_data("image2d","test")
        if dimensions == 3:
            ev_tensor = io_manager.get_data("tensor3d","test")
        if dimensions == 4:
            ev_tensor = io_manager.get_data("tensor4d","test")

        print("Number of images read: ", ev_tensor.size())
        for projection in range(ev_tensor.size()):
            image = ev_tensor.tensor(projection).as_array()
            event_image_list[i].append(copy(image))

    return event_image_list

def build_tensor(rand_num_events, n_projections, dimension=2, shape=None):
    import numpy

    event_image_list = []

    for i in range(rand_num_events):
        event_image_list.append([])

        # Get a piece of data, tensor2d:
        for projection in range(n_projections):
            if shape is None:
                shape = []
                for dim in range(dimension):
                    if dimension < 3:
                        shape.append(random.randint(1, 1e3))
                    else:
                        shape.append(random.randint(1, 20))
                    
            raw_image = numpy.random.random(shape).astype("float32")
            event_image_list[i].append(raw_image)

    return event_image_list

def write_sparse_clusters(file_name, voxel_set_array_list, dimension=2, n_projections=3):


    import copy

    io_manager = larcv.IOManager(larcv.IOManager.kWRITE)
    io_manager.set_out_file(file_name)
    io_manager.initialize()


    # For this test, the meta is pretty irrelevant as long as it is consistent
    meta_list = []
    for projection in range(n_projections):
        if dimension == 2:
            meta_list.append(larcv.ImageMeta2D())
        else:
            meta_list.append(larcv.ImageMeta3D())

        for dim in range(dimension):
            L = 10.
            N = 128
            meta_list[-1].set_dimension(dim, L, N)

        meta_list[-1].set_projection_id(projection)

    for i in range(len(voxel_set_array_list)):
        io_manager.set_id(1001, 0, i)
        # Get a piece of data, sparse tensor:
        if dimension== 2:
            ev_cluster = io_manager.get_data("cluster2d","test")
        else:
            ev_cluster = io_manager.get_data("cluster3d","test")

        # Holder for the voxels to store:


        for projection in range(n_projections):
            clusters = voxel_set_array_list[i][projection]
            if dimension == 2:
                vsa = larcv.SparseCluster2D()
            else:
                vsa = larcv.SparseCluster3D()
            for cluster in range(len(clusters)):
                vs = larcv.VoxelSet()

                vs.id(cluster)
                indexes = clusters[cluster]['indexes']
                values = clusters[cluster]['values']
                for j in range(clusters[cluster]['n_voxels']):
                    vs.emplace(indexes[j], values[j], False)
                vsa.insert(vs)          
            # sparse_cluster.set(vsa, )
            vsa.meta(meta_list[projection])
            # ev_cluster.emplace(vsa, copy.copy(meta_list[projection]))
            ev_cluster.set(vsa)

        io_manager.save_entry()





    # assert(io_manager.get_n_entries_out() == rand_num_events)

    io_manager.finalize()

    return
 
def read_sparse_clusters(file_name, dimension):



    io_manager = larcv.IOManager(larcv.IOManager.kREAD)
    io_manager.add_in_file(file_name)
    io_manager.initialize()


    voxel_set_array_list = []
     
    for event in range(io_manager.get_n_entries()):
        # append a list of projections for this event:
        voxel_set_array_list.append([])

        io_manager.read_entry(event)
        
        # Get a piece of data, sparse cluster:\
        if dimension == 2:
            ev_cluster = io_manager.get_data("cluster2d","test")
        else:
            ev_cluster = io_manager.get_data("cluster3d","test")

        for projection in range(ev_cluster.size()):
            # Append a list of clusters for this projection:
            voxel_set_array_list[event].append([])
            print("Number of clusters: ", ev_cluster.sparse_cluster(projection).size())
            sparse_cluster = ev_cluster.sparse_cluster(projection)
            print("Current voxel_set_array_list length: ", len(voxel_set_array_list))
            print("Current voxel_set_array_list[] length: ", len(voxel_set_array_list[event]))
            for cluster in range(sparse_cluster.size()):
                # Append a dict of values for this cluster
                voxel_set_array_list[event][projection].append({
                    "indexes" : [],
                    "values"  : [],
                    "n_voxels": sparse_cluster.voxel_set(cluster).size()
                    })
                for j in range(sparse_cluster.voxel_set(cluster).size()):
                    voxel_set_array_list[event][projection][cluster]['indexes'].append(sparse_cluster.voxel_set(cluster).as_vector()[j].id())
                    voxel_set_array_list[event][projection][cluster]['values'].append(sparse_cluster.voxel_set(cluster).as_vector()[j].value())



    return voxel_set_array_list

def build_sparse_cluster_list(rand_num_events, n_projections=3):


    voxel_set_array_list = []
     
    for event in range(rand_num_events):
        #  Add space for an event:
        voxel_set_array_list.append([])
        # Get a piece of data, sparse tensor:
        for projection in range(n_projections):
            # In this event, add space for a projection:
            voxel_set_array_list[event].append([])
            n_clusters = random.randint(1,25)
            for cluster in range(n_clusters):
                # print(len(voxel_set_array_list[projection][cluster]))
                n_voxels = random.randint(1,50)
                cluster_d = {                    
                    'values'  : [],
                    'indexes' : random.sample(range(128*128), n_voxels),
                    'n_voxels': n_voxels}
                for j in range(n_voxels):
                    cluster_d['values'].append(random.uniform(-1e3, 1e3) )

                voxel_set_array_list[event][projection].append(cluster_d)

    return voxel_set_array_list



def write_sparse_tensors(file_name, voxel_set_list, dimension, n_projections):


    from copy import copy
    io_manager = larcv.IOManager(larcv.IOManager.kWRITE)
    io_manager.set_out_file(file_name)
    io_manager.initialize()

    # For this test, the meta is pretty irrelevant as long as it is consistent
    meta_list = []
    for projection in range(n_projections):
        if dimension == 2:
            meta_list.append(larcv.ImageMeta2D())
        else:
            meta_list.append(larcv.ImageMeta3D())

        for dim in range(dimension):
            L = 10.
            N = 128
            meta_list[-1].set_dimension(dim, L, N)

        meta_list[-1].set_projection_id(projection)

    for i in range(len(voxel_set_list)):
        io_manager.set_id(1001, 0, i)
        
        # Get a piece of data, sparse tensor:
        if dimension== 2:
            ev_sparse = io_manager.get_data("sparse2d","test")
        else:
            ev_sparse = io_manager.get_data("sparse3d","test")


        for projection in range(n_projections):
            vs = larcv.VoxelSet()
            indexes = voxel_set_list[i][projection]['indexes']
            values = voxel_set_list[i][projection]['values']
            for j in range(voxel_set_list[i][projection]['n_voxels']):
                vs.emplace(indexes[j], values[j], False)

            ev_sparse.set(vs, meta_list[projection])
        io_manager.save_entry()


    print("Finished event loop")



    # assert(io_manager.get_n_entries_out() == rand_num_events)

    io_manager.finalize()

    return
 
def read_sparse_tensors(file_name, dimension):



    io_manager = larcv.IOManager(larcv.IOManager.kREAD)
    io_manager.add_in_file(file_name)
    io_manager.initialize()


    voxel_set_list = []
     
    for i in range(io_manager.get_n_entries()):
        voxel_set_list.append([])

        io_manager.read_entry(i)
        
        # Get a piece of data, sparse tensor:
        if dimension== 2:
            ev_sparse = io_manager.get_data("sparse2d","test")
        else:
            ev_sparse = io_manager.get_data("sparse3d","test")

        for projection in range(ev_sparse.size()):
            voxel_set_list[i].append({
                "indexes" : [],
                "values"  : [],
                "n_voxels": ev_sparse.sparse_tensor(projection).size()
                })
            for j in range(ev_sparse.sparse_tensor(projection).size()):
                voxel_set_list[i][projection]['indexes'].append(ev_sparse.sparse_tensor(projection).as_vector()[j].id())
                voxel_set_list[i][projection]['values'].append(ev_sparse.sparse_tensor(projection).as_vector()[j].value())



    return voxel_set_list

def build_sparse_tensor(rand_num_events, n_projections=3):

    voxel_set_list = []
     
    for i in range(rand_num_events):
        voxel_set_list.append([])
        # Get a piece of data, sparse tensor:
        for projection in range(n_projections):
            n_voxels = random.randint(25,50)
            voxel_set_list[i].append({
                'values'  : [],
                'indexes' : random.sample(range(128*128), n_voxels),
                'n_voxels': n_voxels
                })
            for j in range(voxel_set_list[i][projection]['n_voxels']):
                voxel_set_list[i][-1]['values'].append(random.uniform(-1e3, 1e3) )

    return voxel_set_list



def write_particles(tempfile, rand_num_events, particles_per_event=-1):

    io_manager = larcv.IOManager(larcv.IOManager.kWRITE)
    io_manager.set_out_file(tempfile)
    io_manager.initialize()


    for i in range(rand_num_events):
        io_manager.set_id(1001, 0, i)
        
        # Get a piece of data, particle:
        ev_particle = io_manager.get_data("particle","test")



        if particles_per_event == -1:
            n_particles = i + 1
        else:
            n_particles = particles_per_event

        for j in range(n_particles):
            part = larcv.Particle()
            part.energy_deposit(j)
            part.pdg_code(0)
            ev_particle.append(part)


        io_manager.save_entry()


    assert(io_manager.get_n_entries_out() == rand_num_events)

    io_manager.finalize()

    return
 
def read_particles(tempfile, use_core_driver=False):

    io_manager = larcv.IOManager(larcv.IOManager.kREAD)
    if use_core_driver: 
      io_manager.set_core_driver()
    io_manager.add_in_file(tempfile)
    io_manager.initialize()

    read_events = 0
    for i in range(io_manager.get_n_entries()):
        io_manager.read_entry(i)
        # print(io_manager.current_entry())
        event_id = io_manager.event_id()

        ev_particles = io_manager.get_data('particle', 'test')
        read_events += 1

    return read_events