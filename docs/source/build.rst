`BLOCK` Installation
********************

Compile
=======

`BLOCK` requires BLAS, LAPACK and BOOST.
MPI library is needed for distributed-memory parallel compilation.
`BLOCK` is compiled using the makefile supplied in the distribution. 
The following customizations need to be made to the makefile placed in the main directory ``./Block``. 

Choose compilers by specifying 
       ``CXX = g++``

For MPI-based parallel execution on distributed-memory machines,
        ``USE_MPI = yes``

        ``MPICXX = mpicxx``

MPI library must be compiled using the same compiler as for compiling `BLOCK`. 
Intel compiler such as ``icpc`` is also supported with approriate compiling flags chosen automatically.

Please note that when choosing your compiler, either GNU or Intel, C++0x/C++11 standards must be appropriately supported,
as `BLOCK` requires new features for some of the modules (eg, `npdm`, `nevpt2`, etc).
Here are our suggested `minimum` GNU/Intel compiler versions in order for the compiling process to be successful: 

* GNU ``g++``: 4.8 or newer,
* or Intel ``icpc``: at least 14.0.1 (2013 SP1 Update 1) or newer.

To enable MKL library,

        ``USE_MKL = yes``

And supply MKL and BOOST libraries by giving the locations,
    
	``MKLLIB = /opt/intel/composer_xe_2013_sp1.0.080/mkl/lib/intel64/`` 

	``MKLFLAGS = /opt/intel/composer_xe_2013_sp1.0.080/mkl/include``

	``BOOSTLIB = /lib64/boost_1_55_0/lib/``

	``BOOSTINCLUDE = /lib64/boost_1_55_0/include/``

When the makefile is configured, run in the directory ``./Block``::

        $ make

The successful compilation generates the executable ``block.spin_adapted``, static and shared DMRG libraries ``libqcdmrg.a`` and ``libqcdmrg.so``.


.. _pyscf-itrf:

Interface to PySCF package
--------------------------

The electronic structure Python module `PySCF <http://chemists.princeton.edu/chan/software/pyscf/>`_
provided an interface to run `BLOCK` code.  If you would like to run
DMRG-SCF, DMRG-NEVPT2 etc with PySCF package, you need create a pyscf
config file ``/path/to/pyscf/future/dmrgscf/settings.py`` and add the
following settings in it::

        BLOCKEXE = "/path/to/Block/block.spin_adapted"
        BLOCKSCRATCHDIR = "/path/to/scratch"
        MPIPREFIX = "mpirun"

Note the parameter ``MPIPREFIX`` should be adjusted according to your
job scheduler, eg::

        # For OpenPBS/Torque 
        MPIPREFIX = "mpirun"
        # For SLURM
        MPIPREFIX = "srun"

If calculation is carried out on interactive node, eg with 4 processors,
the setting looks like::

        MPIPREFIX = "mpirun -n 4"

If ``BLOCK`` and ``PySCF`` are installed successfully, a simple DMRG-SCF
calculation can be input in Python interpereter:: 

        >>> from pyscf import gto, scf, dmrgscf
        >>> mf = gto.M(atom='C 0 0 0; C 0 0 1', basis='ccpvdz').apply(scf.RHF).run()
        >>> mc = dmrgscf.dmrgci.DMRGSCF(mf, 6, 6)
        >>> mc.run()

DMRG-NEVPT2 calculation can be applied::

        >>> from pyscf import mrpt
        >>> mrpt.nevpt2.sc_nevpt(mc)

Optionally, if `MPI4Py <http://mpi4py.scipy.org>`_ was installed, the efficient
DMRG-NEVPT2 implementation can be used, eg::

        >>> from pyscf import mrpt
        >>> mrpt.nevpt2.sc_nevpt(dmrgscf.compress_perturb(mc))


How to run `BLOCK`
==================

The standalone serial code can be executed running::

        $ block.spin_adapted input.dat > output.dat

``input.dat`` is the input file and the output of the program is piped into the output file ``output.dat``.

The MPI parallel mode can be called running::

        $ mpirun -np 4 block.spin_adapted input.dat > output.dat

Testjobs
=========

`BLOCK` can be tested by executing the script in the directory ``./Block/dmrg_tests``::

        $ cd dmrg_tests
        $ ./runtest

The tests require Python to be installed on the system.


