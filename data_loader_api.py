import larcv


# This script is an example of what a larcv dataloader should look like.


dl = larcv.dataloader()

# Add several input streams.
# Each input stream is from a file (or, maybe, set of files if distributed?)
dl.add_input_stream(key="train", file="example_train_file.h5")
dl.add_input_stream(key="test", file="example_test_file.h5")


# Set target outputs for each stream.
dl.add_output_stream(
    keys="all",             # This is default, but can be configured
    input_format="image2d", # This needs to be a larcv3 dataformat
    input_label="sbndwire", # This is the larcv key under which this is stored.
    output_format="dense",  # This can be "dense" for dense images, "sparse" for SCN, "graph" for torch_geometric
    framework = "numpy",    # This can be "numpy", "torch", "tf"/"tensorflow"
    preload = True          # If set, it will preload data to the GPU/accelerator
)


dl.configure() # Calling this will configure the C++ code and begin loading data