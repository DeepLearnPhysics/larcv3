import subprocess, os

#...
extensions = [ "breathe" ]
#...

# Breathe Configuration
breathe_default_project = "larcv3"
html_theme = 'sphinx_rtd_theme'

def configureDoxyfile(input_dir, output_dir):
    with open('Doxyfile.in', 'r') as file :
        filedata = file.read()

    filedata = filedata.replace('@DOXYGEN_INPUT_DIR@', input_dir)
    filedata = filedata.replace('@DOXYGEN_OUTPUT_DIR@', output_dir)

    with open('Doxyfile', 'w') as file:
        file.write(filedata)

# Check if we're running on Read the Docs' servers
read_the_docs_build = os.environ.get('READTHEDOCS', None) == 'True'

breathe_projects = {}

if read_the_docs_build:
    input_dir = '../src/larcv3'
    output_dir = 'build'
    configureDoxyfile(input_dir, output_dir)
    subprocess.call('doxygen', shell=True)
    breathe_projects['larcv3'] = output_dir + '/xml'
