#! /path/to/openwalnut/bin/openwalnut-script -f

# 1. Introduction
#
# This is an example on how to use the script interpreter that comes with OpenWalnut.
# We will be loading all NIfTI datasets in a directory given via script parameter, apply a
# gaussian blur and then save the data to another directory also given via script parameter.
#
# However, before we start, let's have a look at OpenWalnut's script interpreter.



# 2. Script interpreter
#
# The script interpreter can be started via
#
# openwalnut-script -i language-name ,
#
# Where 'language-name' is the name of the language to use (currently "lua" and/or "python", depending on which
# prerequisite libraries were available when compiling).
# Start in python mode:
#
# openwalnut-script -i python
#
# You can use this python interpreter just as you would use the normal one. For example, type
#
# print i
#
# This yields an error, as 'i' was not defined. Try
#
# i = 1
# print i
#
# This yields the correct answer. You can also use all modules available to your python installation:
#
# import datetime
# print datetime.datetime.now().strftime( "%A %Y-%m-%d %H:%M" )
#
# This will print the current day and time. You can also define and use functions and classes. Feel free to experiment a bit.
# You can quit by typing 'quit'.



# 3. Executing script files
#
# To execute a script, switch to your OpenWalnut installation and execute:
#
# openwalnut-script -f script-name
#
# Where 'script-name' is the path to and name of your script file. For example, to execute this script, type
#
# openwalnut-script -f template.py
#
# Depending on whether you installed OpenWalnut or built it yourself, you might need to change the path to
# something like:
#
# openwalnut-script -f ../resources/scripting/template.py
#
# The script will tell you that no input and output directories were found and OpenWalnut will quit afterwards.
# The directories can be set via script parameters:
#
# openwalnut-script -f ../resources/scripting/template.py ./inDir ./outDir
#
# or
#
# ../resources/scripting/template.py ./inDir ./outDir    (this requires the script to be executable and the path to your
#                                                         OpenWalnut installation/build-directory set correctly in the
#                                                         first line of the script)



# 4. Implementing a batch processing pipeline for OpenWalnut
#
# Now that we know how to execute the script, let's have a look at what it actually does.
# The first part is importing what we need:

import sys
import os
import signal
import time

# This makes the interpreter react to Ctrl+C.
signal.signal( signal.SIGINT, signal.SIG_DFL )

# Now we can proceed by checking the parameters:

if len( sys.argv ) < 3:
    print "Too few parameters, quitting."
    exit( 1 )

# This checks the input dir:

inputDir = sys.argv[ 1 ]
print "Input dir was set to", inputDir
if not os.path.exists( inputDir ):
    print "The directory", inputDir, "does not exist!"
    exit( 1 )
if not os.path.isdir( inputDir ):
    print inputDir, "is not a directory!"
    exit( 1 )

# Now the same for our output directory:

outputDir = sys.argv[ 2 ]
print "Output dir was set to", outputDir
if not os.path.exists( outputDir ):
    print "The directory", outputDir, "does not exist!"
    exit( 1 )
if not os.path.isdir( outputDir ):
    print outputDir, "is not a directory!"
    exit( 1 )

# The next step is to find all the files that we want to process:

niftiList = list()
filesList = os.listdir( inputDir )
for f in filesList:
    if not os.path.isdir( os.path.join( inputDir, f ) ):
        filename, extension = os.path.splitext( f )
        if extension is not None and extension == '.nii':
            niftiList.append( f )
        if extension is not None and extension == '.gz':
            filename, extension = os.path.splitext( filename )
            if extension is not None and extension == '.nii':
                niftiList.append( f )
print "List of nifti files in input directory:", niftiList
if len( niftiList ) == 0:
    print "No nifti files found!"
    exit( 1 )

# Now that we have found all the datasets, we can proceed by initializing the
# OpenWalnut module pipeline that will do the gauss filter on our datasets. The
# pipeline will look like this:
#
# 'Data Module' -> 'Gauss Filtering' -> 'Write NIfTI'
#
# We start by creating the 'Gauss Filtering' and 'Write NIfTI' modules using the
# 'rootContainer' global:

gauss = rootContainer.create( "Gauss Filtering" )
writer = rootContainer.create( "Write NIfTI" )

# We can now change some properties of the modules we just created. We get access
# to a module's properties with the 'getProperties()' and 'getInformationProperties()' funtions.
# These return the respective property groups.
# The groups can then be asked for properties or nested property groups via the 'getProperty()'
# and 'getGroup()' functions, both taking a string parameter denoting the name of the property
# or group to get.

iterProp = gauss.getProperties().getProperty( "Iterations" )

# Let's increase the width of the kernel:

iterProp.setInt( 2 )

# The properties provide various functions for getting and setting, for example the
# 'setInt()' function we just used. There are more such functions, such as
#
# 'setBool()'
# 'setDouble()'
# 'setFilename()'  - the parameter is a string denoting a path/filename
# 'setSelection()' - the parameter is an integer denoting which element of a selection to select
#                    0 is the first one, 1 the second one etc....
#
# There are also the respective getters.

# We now need to connect the modules. To be more precise, we need to connect
# the writer's input connector 'in' to the gauss filter's output connector 'out'.
# The names of the connectors can be looked up in the GUI version of OpenWalnut.
# (The names of the properties too.)

writer.getInputConnector( "in" ).connect( gauss.getOutputConnector( "out" ) )

# The modules can be disconnected again using the 'disconnect()' function of either
# the input- or the output connectors. We'll see this in action later.

# Now that we have done the setup, let's start working on the datasets.
for dataset in niftiList:
    fileToLoad = os.path.join( inputDir, dataset )

    # We start with loading the dataset, which is done by creating a data module:
    data = rootContainer.createDataModule( fileToLoad )

    # Now we check if the data we loaded is a scalar dataset:
    if not data.getInformationProperties().getProperty( "Dataset type" ).getString( True ) == "WDataSetScalar":
        print fileToLoad, "does not contain scalar data! Skipping."
        continue

    # If the dataset is a scalar dataset, we proceed by setting the output filename in the writer:
    fileToSave = os.path.join( outputDir, dataset )
    writer.getProperties().getProperty( "Filename" ).setFilename( fileToSave )

    # Now we connect the data module, this will start the pipeline:
    gauss.getInputConnector( "in" ).connect( data.getOutputConnector( "out" ) )

    # The tricky part is waiting for the correct events, as all OpenWalnut modules run
    # in their own threads. We cannot just keep on loading data and passing them to the gauss
    # module, as it would just ignore the new inputs until it has finished calculating. By the time
    # this happens, we might already be done loading the datasets, so OpenWalnut would just
    # close because we reached the end of the script without waiting for the calculations to finish.
    #
    # So what we need to do now is wait for the input of the writer to update:
    writer.getInputConnector( "in" ).waitForInput()

    # We can now issue the saving of the result by pushing the 'Save' button.
    writer.getProperties().getProperty( "Do save" ).click()

    # We could now wait for the button to be reset. (Which means the saving is done.)
    #
    # writer.getProperties().getProperty( "Do save" ).waitForUpdate()
    #
    # You can wait for any property to be changed via its 'waitForUpdate()' function.
    #
    # However, for really small datasets, the saving might be done really fast, so
    # the update may happen before we actually start waiting for it, resulting in us waiting
    # forever. And we most certainly do not have that much time. So we just wait a few seconds.
    time.sleep( 3 )

    # Now our data should be written to disk, so we can now delete the dataset from the pipeline
    # by removing the data module. This is done via the 'remove()' function of the 'rootContainer'
    # global. We might also want to disconnect the data- and gauss modules beforehand.
    data.getOutputConnector( "out" ).disconnect()
    rootContainer.remove( data )
    # Now we are ready for the next dataset.




# Well, and that's it for this simple batch processing example.
time.sleep( 3 )
print "Everything done, bye!"

# As python provides quite a lot of functionality, you can write lots of cool scripts to control
# your pipelines. For example, using sockets, you could control multiple OpenWalnut instances on
# remote computers. Have an entire army of OpenWalnuts do as you command!
#
# Have fun!
