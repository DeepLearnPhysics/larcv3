#!/usr/bin/env python
import sys,os,argparse
import larcv
import json

# This script is a simplification of run_processordb but without the proddb dependence.
# It incorporates command line arguments but runs stand-alone with just larcv.

parser = argparse.ArgumentParser(description='LArCV ProcessDriver execution script (use proddb)')

parser.add_argument('-c','--config',
                    type=str, dest='cfg',
                    help='string, Config file or json text',required=True)

parser.add_argument('-il','--input-larcv',required=True,
                    dest='larcv_fin',nargs='+',
                    help='string or list, Input larcv file name[s] (Required)')

parser.add_argument('-nevents','--num-events',
                    type=int, dest='nevents', default=0,
                    help='integer, Number of events to process')

parser.add_argument('-nskip','--num-skip',
                    type=int, dest='nskip', default=0,
                    help='integer, Number of events to skip')

parser.add_argument('-ol','--output-larcv',default='',
                    type=str, dest='larcv_fout',
                    help='string,  Output larcv file name (optional)')

parser.add_argument('-oa','--output-ana',default='',
                    type=str, dest='ana_fout',
                    help='string,  Output analysis file name (optional)')

args = parser.parse_args()


if len(args.larcv_fin) == 0:
    print('No input files found')
    sys.exit(1)


proc = larcv.ProcessDriver('ProcessDriver')


# Read the config, if present.  
if "cfg" in args:
    # Could be a file:
    if os.path.exists(args.cfg):
        # Read it in as json:
        with open(args.cfg) as config_file:
            conf = json.load(config_file)
    else:
        # REad it directly from string:
        conf = joson.loads(args.cfg)

    proc.configure(conf)

print(json.dumps(proc.config(), indent=4))


if args.larcv_fout != '':
    proc.override_output_file(args.larcv_fout)


proc.override_input_file(args.larcv_fin)

proc.initialize()

proc.batch_process(args.nskip,args.nevents)

print("Number of entries processed: " + str(proc.io().get_n_entries_out()))
print("Output file name: " + str(proc.io().get_file_out_name()))


proc.finalize()
