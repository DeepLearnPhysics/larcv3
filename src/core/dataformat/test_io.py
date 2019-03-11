# This function is purely to test the IO read write capabilities.

from larcv import dataformat

io_manager = dataformat.IOManager(dataformat.IOManager.kWRITE)
io_manager.set_out_file("test.h5")


io_manager.initialize()