using MAT, JuMP, Gurobi;

# Files for synchronization of execution
const sync_file    = "env/" * ARGS[1] * "." * ARGS[2] * ".sync.txt";
const angles_file  = "env/" * ARGS[1] * "." * ARGS[2] * ".angles.txt";
const objs_file    = "env/" * ARGS[1] * "." * ARGS[2] * ".objs.txt";
const weights_file = "env/" * ARGS[1] * "." * ARGS[2] * ".weights.txt";

# File for reporting more complex results of intensity model
const report_file  = "results/" * ARGS[1] * "/report." * ARGS[2] * ".txt";

const GUROBI_ENV = Gurobi.Env(); # Permanent environment for gurobi

########################################## FUNCTIONS ##########################################

function parsefile(file::String) # Read the file with id and name of the structures
    instance = readdlm(file);
    indexes = instance[2:end,1];
    structures = instance[2:end,2];
    types = instance[2:end,3];
    indexes = convert(Array{Int64,1}, indexes);
    structures = convert(Array{String,1}, structures);
    types = convert(Array{Int64,1}, types);
    return (indexes, structures, types);
end

function readtestcase(case::String)
    # Read test case description
    (indexes, structures, types) = parsefile("testcases/" * case * ".txt");

    ######################## READ TEST CASE DATA ########################
    matfile = matopen("testcases/" * case * ".mat");
    problem = read(matfile, "problem");

    #=
        We want the constraints corresponding to the ids informed in the .txt file.
        To do this, the 'dataID', 'IsConstraint' and 'Objective' data are retrived.
        Then, only the ids and objectives refering to constraints are considered.
        Since the IDs aren't sorted, we obtain a permutation of the indexes of ids array 
            to find where are the ids used in the problem.
        Finally, the correponding objectives are selected.
    =#
    ids = vec(problem["dataID"]);
    consts = vec(problem["IsConstraint"]);
    objs = vec(problem["Objective"]);
    consts_objs = [objs[i] for i in 1:length(ids) if consts[i] == true];
    consts_ids  = [ids[i]  for i in 1:length(ids) if consts[i] == true];
    s = sortperm(consts_ids);
    constraints = [consts_objs[s[i]] for i in indexes];

    # Extract the number of beamleats of each beam
    beam_sizes = convert(Array{Int64,1}, vec(read(matfile, "patient/Beams/ElementIndex")));
    beam_ranges = [1; beam_sizes];
    for i = 2:length(beam_ranges)
        beam_ranges[i] += beam_ranges[i-1];
    end
    beam_number = length(beam_sizes);

    # Read matrixes of dose influence (the A's)
    As = read(matfile, "data/matrix/A");
    matrixes = [As[i] for i in indexes];
    n_voxels = [size(As[i])[1] for i in indexes];
    close(matfile);

    return (indexes, structures, types, beam_sizes, beam_ranges, matrixes, 
            n_voxels, constraints);
end

function readangles() # Read the selected angles
    angles = Array{Int64,1}();
    n_angles = 0;
    if isfile(angles_file)
        angles = vec(convert(Array{Int64,2}, readdlm(angles_file)));
        n_angles = length(angles);

        return (angles, n_angles);
    else
        open(sync_file, "w") do file
            print(file, "-1");
        end
        return (-1,-1);
    end
end

function changestate(state::String) # Change the state
    open(sync_file, "w") do file
        print(file, state);
    end
end

function writeobjs(obj::Float64, organs::Float64, tumor_p::Float64, tumor_n::Float64) # Write the objective functions to the file
    open(objs_file, "w") do file
        print(file, obj, " ", 
                    organs, " ", 
                    tumor_p, " ", 
                    tumor_n);
    end
end

function finalreport(l_stru::Int64,
                     n_voxels::Array{Int64,1},
                     dose::JuMP.JuMPDict{JuMP.Variable,2},
                     positive_dev::JuMP.JuMPDict{JuMP.Variable,2},
                     negative_dev::JuMP.JuMPDict{JuMP.Variable,2})
    println("final_report");
    #=for i in 1:l_stru
        meandose = getvalue( sum( dose[i,1:n_voxels[i]] ) ) / n_voxels[i];
    end=#
    
    doses  = getvalue(dose);
    posdev = getvalue(positive_dev);
    negdev = getvalue(negative_dev);
    for i in 1:l_stru
        meandose = mean(doses[i,:]); # Mean dose on organ i
        stdvdose = stdm(doses[i,:], mean_dose); # Standard deviation of dose on organ i
        abovepresc = count(x->x>constraints[i],  doses[i,:]); # Number of voxels whose dose is above the prescription
        belowpresc = count(x->x<=constraints[i], doses[i,:]); # Number of voxels whose dose is below the prescription
        meanposdev = mean(posdev[i,:]); # Mean positive dose deviation on organ i
        stdvposdev = stdm(posdev[i,:], meanposdev); # Standard deviation of positive dose deviation on organ i
        meannegdev = mean(negdev[i,:]); # Mean negative dose deviation on organ i
        stdvnegdev = stdm(negdev[i,:], meanposdev); # Standard deviation of negative dose deviation on organ i
        open(report_file, "a") do file
            print(file, structures[i], " ",
                        meandose,      " ",
                        stdvdose,      " ",
                        abovepresc,    " ",
                        belowpresc,    " ",
                        meanposdev,    " ",
                        stdvposdev,    " ",
                        meannegdev,    " ",
                        stdvnegdev,    "\n");
        end
    end
    
end

function solvemodel(l_stru::Int64, # Solve the intensity model
                    beam_sizes::Array{Int64,1}, 
                    beam_ranges::Array{Int64,1},
                    matrixes::Array{AbstractArray{T,2} where T,1},
                    n_voxels::Array{Int64,1}, 
                    constraints::Array{Float64,1},
                    angles::Array{Int64,1}, 
                    n_angles::Int64, 
                    types::Array{Int64,1},
                    structures::Array{String,1},
                    final_report::Bool=false) 

    #m = Model(solver=GurobiSolver(OutputFlag=0));
    m = Model(solver=GurobiSolver(OutputFlag=0, GUROBI_ENV));

    #################################### Creating variables ####################################
    
    # Variables corresponding to the dose on the beamlets of selected beams
    @variable(m, beamlet_dose[ i=1:n_angles, j=1:beam_sizes[angles[i]] ] >= 0);

    # Variables corresponding to the dose deviation on voxels
    @variable(m, positive_dev[ i=1:l_stru, j=1:n_voxels[i] ] >= 0);
    @variable(m, negative_dev[ i=1:l_stru, j=1:n_voxels[i] ] >= 0);

    # Variables of dose on voxels
    @variable(m, dose[ i=1:l_stru, j=1:n_voxels[i] ] >= 0);

    println(typeof(positive_dev));
    println(typeof(dose));
    
    ################################### Creating constraints ###################################
    
    @constraints(m, begin
        # Constraint that equates the dose on a voxel to the corresponding product
        dose_calc[ i=1:l_stru, j=1:n_voxels[i] ], 
            dose[i,j] == sum(matrixes[i][j,beam_ranges[angles[b]]+bl-1] * beamlet_dose[b,bl]
                            for b in 1:n_angles 
                                for bl in 1:beam_sizes[angles[b]]
                            )
        #=
          matrixes[i] contains the dose matrix for structure i
          matrixes[i][j] contains the dose influence of all beamlets on voxel j of structure i
          matrixes[i][j,beam_ranges[angles[b]]+bl-1] is dose influence of beamlet bl of beam indexed by b (angles[b]) on
                                                     voxel j of structure i

          beam_sizes[angles[b]] is the number of beamlets of beam indexed by b (angles[b])
        =#
    end);

    @constraints(m, begin
        #Constraint that equates the dose on a voxel to the prescription plus and minus the deviations
        dose_prescription[ i=1:l_stru, j=1:n_voxels[i] ],
        dose[i,j] == constraints[i] + positive_dev[i,j] - negative_dev[i,j]
    end);
    
    ############################# Defining the objective functions #############################
    
    # Auxiliary expressions corresponding to each of the original objective functions
    tumor_p_dev  = AffExpr();
    tumor_n_dev  = AffExpr();
    organs_p_dev = AffExpr();

    TUMOR = 1;
    ORGAN = 2;
    for i = 1:l_stru
        if types[i] == TUMOR
            append!(tumor_p_dev,  sum(positive_dev[i,j] for j = 1:n_voxels[i]));
            append!(tumor_n_dev,  sum(negative_dev[i,j] for j = 1:n_voxels[i]));
        elseif types[i] == ORGAN
            append!(organs_p_dev, sum(positive_dev[i,j] for j = 1:n_voxels[i]));
        else
            println("ERROR ON TYPE!!!");
        end
    end
    
    # Read weights for objective function
    w_t_p = w_t_n = w_o_p = 0;
    if isfile(weights_file)
        ws = readdlm(weights_file, Float64);
        (w_o_p, w_t_p, w_t_n) = (ws[1,1], ws[1,2], ws[1,3]);
    else
        println("ERROR ON WEIGHTS!!!");
    end

    @objective(m, Min, w_o_p*organs_p_dev + w_t_p*tumor_p_dev + w_t_n*tumor_n_dev);
    solve(m);

    if final_report
        finalreport(l_stru, n_voxels, dose, positive_dev, negative_dev);
    end

    return (getobjectivevalue(m), getvalue(organs_p_dev), getvalue(tumor_p_dev), getvalue(tumor_n_dev));
end

####################################### END OF FUNCTIONS ######################################



######################################### MAIN PROGRAM ########################################

function main()
    TS_SLEEP = "0";
    MODEL_SLEEP = "1";
    MODEL_STOP = "2";
    REPORT = "82";

    changestate(TS_SLEEP); # The C++ program will not try to send any beam set while the state is 0

    # Read test case data
    (indexes, structures, types, beam_sizes, beam_ranges, 
        matrixes, n_voxels, constraints) = readtestcase(ARGS[1]);

    changestate(MODEL_SLEEP);    

    # This program ends when there's a "2" in sync file
    while MODEL_STOP != (s = open(sync_file, "r") do file readstring(file) end)
        if s == MODEL_SLEEP
            sleep(0.5); # The C++ code corresponding to the metaheuristic is choosing angles
        else
            # The angles were chosen by the metaheuristic. Solve the intensity model.
            (angles, n_angles) = readangles();
            if n_angles == -1 # Something went wrong in reading angles
                break;
            end
            
            (obj,organs,tumor_p,tumor_n) = solvemodel(length(structures), beam_sizes, beam_ranges, matrixes, 
                                                      n_voxels, constraints, angles, n_angles, types,
                                                      structures, (s == REPORT) );
            writeobjs(obj,organs,tumor_p,tumor_n);
            changestate(MODEL_SLEEP);
        end
    end

end

main();