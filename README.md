# Tabu Search IMRT
Tabu search code for Geometry and Intensity problems in IMRT.

The code in this repository implements 2 algorithms: TSchainRad (see 1) and TSrad (see 2). Both of the algorithms use C++11 for the Geometry Problem part and Julia 0.6.4 por the Intensity Problem part.

**For compiling**:
    * TSchainRad: just execute the script ``compile_TSchainRad.sh``
    * TSrad: just execute the script ``compile_TSrad.sh``
    
**For executing**:
    * TSchainRad: ``./exes/main instance_name execution_number tabu_tenure max_falhas [use_irace]``    
    * TSrad: ``./exes/obal instance_name execution_number tabu_tenure``
    
Where:
    * instance_name: self-descriptive. The program will try to read two files with that name: ``instance_name.txt`` and ``instance_name.mat``. The former contains the number of available beams in the instance and listing of the structures in the instance. The latter is the numerical data for the radiotherapy optimization.
        - 
