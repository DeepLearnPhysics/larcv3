# This function is purely to test the IO read write capabilities.

from larcv import base, dataformat

base.logger.default_level(base.kDEBUG )

io_manager = dataformat.IOManager(dataformat.IOManager.kWRITE)
io_manager.set_out_file("test.h5")
io_manager.initialize()

for i in range(5):
    io_manager.set_id(1000, 0, i)
    
    # Get a piece of data, particle:
    ev_particle = dataformat.EventParticle.to_particle(io_manager.get_data("particle","testio"))

    print(type(ev_particle))

    for j in range(3):
        part = dataformat.Particle()
        part.energy_deposit(j)
        ev_particle.emplace_back(part)

    print(ev_particle.size())

    io_manager.save_entry()



print(io_manager.get_n_entries_out())