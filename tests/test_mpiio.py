# from mpi4py import MPI 

from larcv import larcv
print("imported larcv")

a = larcv.MPIIOManager()

print("Constructed IO Manager")

a.initialize()