import unittest
import larcv

# queue_io_sparsetensor2d_cfg_template = '''
# {name}: {{
#   Verbosity:       2
#   EnableFilter:    false
#   RandomAccess:    0 
#   RandomSeed:      0
#   InputFiles:      [{input_files}]
#   ProcessType:     ["BatchFillerSparseTensor2D"]
#   ProcessName:     ["test_{name}"]

#   ProcessList: {{
#     test_{name}: {{
#       Tensor2DProducer: "{producer}"
#       MaxVoxels: 100
#       UnfilledVoxelValue: -999
#       Channels: {channels}
#     }}
#   }}
# }}
# '''

def test_pset_creation():

  IOPSet = larcv.PSet("IOManager")
  IOPSet.add_value("Verbosity", "2")
  assert int(IOPSet["Verbosity"]) == 2

def test_pset_modification():

  IOPSet = larcv.PSet("IOManager")
  IOPSet.add_value("Verbosity", "2")
  IOPSet["Verbosity"] = str(1)
  assert int(IOPSet["Verbosity"]) == 1

def test_pset_nesting():

  overall_pset = larcv.PSet("TrainIO")

  overall_pset["Verbosity"]    = str(2)
  overall_pset["EnableFilter"] = str("false")
  overall_pset["RandomAccess"] = str(0 )
  overall_pset["RandomSeed"]   = str(0)
  overall_pset["InputFiles"]   = str([""])
  overall_pset["ProcessType"]  = str(["BatchFillerSparseTensor2D"])
  overall_pset["ProcessName"]  = str(["test_{name}"])

  process_pset = larcv.PSet("ProcessList")
  process_pset["test"] = "yes"

  overall_pset.add_pset(process_pset)




if __name__ == "__main__":
  test_pset_creation()