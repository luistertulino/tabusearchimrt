using MAT, JuMP, Gurobi;

TUMOR = 1;
ORGAN = 2;

function parsefile(file::String) 
    instance = readdlm(file);
    indexes = instance[1:end,1];
    structures = instance[1:end,2];
    types = instance[1:end,3];
    indexes = convert(Array{Int64,1}, indexes);
    structures = convert(Array{String,1}, structures);
    types = convert(Array{Int64,1}, types);
    return (indexes, structures, types);
end

(indexes, structures, types) = parsefile(ARGS[1]);

file = replace(ARGS[1], ".txt", ".mat");

matfile = matopen(file);
problem = read(matfile, "problem");

#=
    We want the constraints corresponding to the ids informed in the .txt file.
    To do this, the 'dataID', 'IsConstraint' and 'Objective' data are retrived.
    Then, only the ids and objectives refering to constraints are considered.
    Since the IDs aren't sorted, we obtain a permutation of the indexes of ids array to find where are the ids used in the problem.
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
for i = 3:length(beam_ranges)
    beam_ranges[i] += beam_ranges[i-1];
end
beam_number = length(beam_sizes);

# Read matrixes of dose influence (the A's)
As = read(matfile, "data/matrix/A");
matrixes = [As[i] for i in indexes];
n_voxels = [size(As[i])[1] for i in indexes];
#As = nothing; 3.4 com isso

close(matfile);

open("../sync.txt", "w") do file
    print(file, 1);
end

#=while "2" != (s = open("../sync.txt", "r") do file readstring(file) end)
    if s == "1"
        sleep(0.5); # The C++ code corresponding to the metaheuristic is choosing angles
    else
        # The angles were chosen by the metaheuristic. Solve the intensity model.
    end
end=#

# Read the selected angles
angles = Array{Int64,1}();
n_angles = 0;
if isfile("../angles.txt")
    angles = vec(convert(Array{Int64,2}, readdlm("../angles.txt")));
    n_angles = length(angles);
else
    open("../sync.txt", "w") do file
        print(file, "-1");
    end
end

#################################### INTENSITY MODEL CREATION ####################################

m = Model(solver=GurobiSolver(OutputFlag=0));

#################################### Creating variables ####################################

# Variables corresponding to the dose on the beamlets of selected beams
tic();
@variable(m, beamlet_dose[ i=1:n_angles, j=1:beam_sizes[angles[i]] ] >= 0);

# Variables corresponding to the dose deviation on voxels

@variable(m, positive_dev[ i=1:length(structures), j=1:n_voxels[i] ] >= 0);

@variable(m, negative_dev[ i=1:length(structures), j=1:n_voxels[i] ] >= 0);

@variable(m, dose[ i=1:length(structures), j=1:n_voxels[i] ] >= 0);
toc(); println("variables\n");
################################### Creating constraints ###################################

l_stru = length(structures);
tic();
@constraints(m, begin

    # Constraint that equates the dose on a voxel to the corresponding product
    dose_calc[ i=1:l_stru, j=1:n_voxels[i] ], dose[i,j] == sum(matrixes[i][j,bl] 
                                                                for b in angles 
                                                                    for bl in beam_ranges[b]:beam_ranges[b+1])
end);

@constraints(m, begin
    #Constraint that equates the dose on a voxel to the prescription plus and minus the deviations
    dose_prescription[ i=1:length(structures), j=1:n_voxels[i]],
    dose[i,j] == constraints[i] + positive_dev[i,j] - negative_dev[i,j]
end);
toc(); println("constraints\n");

#delete(m,dose_calc);

############################# Defining the objective functions #############################

# Auxiliary expressions corresponding to each of the original objective functions
tic();
tumor_p_dev  = AffExpr();
tumor_n_dev  = AffExpr();
organs_p_dev = AffExpr();

for i = 1:length(structures)
    if types[i] == TUMOR
        append!(tumor_p_dev,  sum(positive_dev[i,j] for j = 1:n_voxels[i]));
        append!(tumor_n_dev,  sum(negative_dev[i,j] for j = 1:n_voxels[i]));
    elseif types[i] == ORGAN
        append!(organs_p_dev, sum(positive_dev[i,j] for j = 1:n_voxels[i]));
    else
        println("ERROR ON TYPE!!!");
    end
end
toc(); println("sum of deviations\n");
# Read weights for objective function
w_t_p = w_t_n = w_o_p = 0;
if isfile("../weights.txt")
    ws = readdlm("../weights.txt", Float64);
    (w_o_p, w_t_p, w_t_n) = (ws[1,1], ws[1,2], ws[1,3]);
else
    println("ERROR ON WEIGHTS!!!");
end

@objective(m, Min, w_o_p*organs_p_dev + w_t_p*tumor_p_dev + w_t_n*tumor_n_dev);

solve(m);

println("time: ", getsolvetime(m));
println("obj = ", getobjectivevalue(m));


#################################### INTENSITY MODEL CREATION ####################################