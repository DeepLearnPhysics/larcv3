import argparse
import glob

import h5py, numpy

def main():


  parser = argparse.ArgumentParser(description='LArCV ProcessDriver execution script (use proddb)')

  parser.add_argument('-il','--input-larcv',required=True,
                      dest='larcv_fin',nargs='+',
                      help='string or list, Input larcv file name[s] (Required)')

  parser.add_argument('-ol','--output-larcv',default='',
                      type=str, dest='larcv_fout',
                      help='string,  Output larcv file name (optional)')


  args = parser.parse_args()

  file_list = args.larcv_fin
  if type(file_list) == str:
    file_list = [file_list,]

  output_file = h5py.File(args.larcv_fout, 'w')


  for f in file_list: 
    print("Adding file to output: ", f)
    input_file = h5py.File(f,'r')
    copy_file_to_output(input_file,output_file)
    input_file.close()


def copy_file_to_output(input_file, output_file):

  copy_event_id_table(input_file, output_file)

  # Make sure the data group is in the output file:
  if 'Data' not in output_file:
    output_file.create_group("Data")

  for group in input_file['Data'].keys():
    copy_data_product(input_file, output_file, group)



def copy_event_id_table(input_file, output_file):

  # First, open the table from the input file:
  input_event_id_table = input_file['Events/event_id']

  # Next, see if the table is in the output file:
  if 'Events' not in output_file:
    output_file.create_group("Events")

  if 'event_id' not in output_file["Events/"]:
    output_file.create_dataset("Events/event_id",
      data=input_event_id_table,
      # shape=input_event_id_table.dims,
      chunks=input_event_id_table.chunks,
      dtype=input_event_id_table.dtype,
      maxshape=input_event_id_table.maxshape,
      compression=input_event_id_table.compression)

  else:
    # The dataset already exists, so resize and assign it new elements:
    output_event_id_table = output_file['Events/event_id']
    original_offset = output_event_id_table.shape[0]
    new_shape = numpy.asarray(input_event_id_table.shape) + numpy.asarray(output_event_id_table.shape)
    
    output_event_id_table.resize(new_shape) 

    output_event_id_table[original_offset:] = input_event_id_table


def copy_data_product(input_file, output_file, group_name):

  # Converting a dataproduct means copying all tables from a group into
  # the same tables in the output group.  The datasets that are holding the actual 
  # data (images, voxels, particles) are all unchanged.
  # Any table that has 'extents' in it's name has to be updated.
  # In particular, the 'first' column in every extents dataset must be modified
  # by adding the total number of the last entry in the existing table (first + n)


  full_path = "Data/" + group_name + "/"


  if full_path not in output_file:
    output_file.create_group(full_path)


  for dataset in input_file[full_path].keys():


    # Get the input dataset:
    input_dataset = input_file[full_path + dataset]

    # Make sure the dataset is in the output file:
    if dataset not in output_file[full_path]:
      # print ("Creating new dataset ", dataset)
      output_file.create_dataset(full_path + dataset,
        data=input_dataset,
        # shape=input_dataset.dims,
        chunks=input_dataset.chunks,
        dtype=input_dataset.dtype,
        maxshape=input_dataset.maxshape,
        compression=input_dataset.compression)
    else:
      # print ("Appending to dataset ", dataset)
      # The output dataset already exists.  So resize it and assign new elements:
      output_table = output_file[full_path + dataset]
      # print("Existing shape: ", output_table.shape)
      original_offset = output_table.shape[0]
      new_shape = numpy.asarray(input_dataset.shape) + numpy.asarray(output_table.shape)
      # print("Input shape: ", input_dataset.shape)
      # print("New shape: ", new_shape)
    
      # print("Updated shape: ", output_table.shape)

      # Deal with the extents issue:

      if 'extents' in dataset:

        # Get the offsets for the new table before resizing:
        existing_offset = output_table['first'][-1]+ output_table['N'][-1]
        index = len(output_table)



      output_table.resize(new_shape) 

      output_table[original_offset:] = input_dataset

      # Update the offset values:
      if 'extents' in dataset:
        output_table['first', original_offset:] += existing_offset

  pass


if __name__ == "__main__":
  main()