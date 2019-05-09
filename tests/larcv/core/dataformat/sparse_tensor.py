from test_write_sparse_cluster import *

rand_num_events = 10
dimension = 3
n_projections = 3


voxel_set_list = build_sparse_cluster_list(rand_num_events, n_projections=n_projections)

tempfile = "cluster.h5"
write_sparse_clusters(tempfile, voxel_set_list, dimension, n_projections)
read_voxel_set_list = read_sparse_clusters(tempfile, dimension)
