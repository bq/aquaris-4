WHAT IS THIS?
=============

Linux Kernel source code for the device bq Aquaris 4

BUILD INSTRUCTIONS?
===================

Specific sources are separated by branches and each version is tagged with it's corresponding number. First, you should
clone the project:

	$ git clone git@github.com:bq/aquaris-4.git

After it, choose the version you would like to build:

	$ cd aquaris-4/
	$ git checkout 1.1.0-20131218-1950

Finally, build the kernel:

	$ ./makeMtk -t ztenj77_sz_cu_jb n k

