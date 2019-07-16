# Tabu Search IMRT
Tabu search code for Geometry and Intensity problems in IMRT.

The code in this repository implements 2 algorithms: TSchainRad (see 1) and TSrad (see 2). Both of the algorithms use C++11 for the Geometry Problem part and Julia 0.6.4 por the Intensity Problem part.

### Compiling
TSchainRad: just execute the script ``compile_TSchainRad.sh``.

TSrad: just execute the script ``compile_TSrad.sh``.

### Executing
TSchainRad: ``./exes/main instance_name execution_number tabu_tenure max_fails [use_irace]``.

TSrad: ``./exes/obal instance_name execution_number tabu_tenure``.

Where:
* _instance_name_: self-descriptive. The program will try to read two files with that name: ``instance_name.txt`` and ``instance_name.mat``. The former contains the number of available beams in the instance and listing of the structures in the instance. The latter is the numerical data for the radiotherapy optimization.
* _execution_number_: the number of execution of the program. It is appended in the synchronization and output files, and it is only necessary when multiple executions of the programs run at the same time for the same instance. For example, when parallel executions are somehow run with the following script
```
for k in {1..30}; do
   ./exes/main Liver_01 $k 2 1
done
```
that indexing is necessary to avoid conflict of files.
* _tabu_tenure_: parameter of Tabu Search --- size of tabu "list".
* _max_fails_: parameter of Ejection Chain used within the TSchainRad: allowed number of failures in ejection chain procedure.

**WARNING**: the execution directory MUST be the root of the repository.

### Dependencies

* C++11.
* Gurobi solver: https://www.gurobi.com/resource/starting-with-gurobi/ (make sure to install the software and the proper license for you).
* Julia 0.6.4: https://docs.julialang.org/en/v0.6/ and its packages.
 On terminal, type ``julia`` and install the following:
 * MAT: ``Pkg.install("MAT");`` (you perhaps will be asked to install HDF5 at some point)
 * JuMP: ``Pkg.install("JuMP");``
 * Gurobi: ``Pkg.install("Gurobi")`` (make sure the Gurobi software is installed before installing this package).
 
### Guarantees

I developed, compiled and executed everything on Linux and offer NO guarantee that any piece of this code is functional on Windows or Mac.
Regarding Gurobi solver, this is the one I am more familiar with, but you are free to try any other solver supported by JuMP. I again offer no guarantee that this will work.

#### References

1. TSchainRad: Coming soon.
2. Thalita Monteiro Obal. [Desenvolvimento e avaliação de matheurísticas para o combinado problema do posicionamento dos feixes e distribuição de dose no planejamento de radioterapia.](https://acervodigital.ufpr.br/bitstream/handle/1884/44471/R%20-%20T%20-%20THALITA%20MONTEIRO%20OBAL.pdf?sequence=1&isAllowed=y)
