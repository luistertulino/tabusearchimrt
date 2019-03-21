using MAT, HDF5, JuMP, Gurobi;

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
beam_ranges = vec(read(matfile, "patient/Beams/ElementIndex"));
beam_ranges = convert(Array{Int64,1}, beam_ranges);

# Read matrixes of dose influence (the A's)
As = read(file, "data/matrix/A");
matrixes = [As[i] for i in p_ids];



