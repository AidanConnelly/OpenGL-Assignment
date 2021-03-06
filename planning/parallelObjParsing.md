### OBJ Parsing ###

The main barrier to parallelism of OBJ parsing is that indecies are specified in order of appearance. This means final
index determination cannot occur until all lines have had their type determined. The solution may be of the form:

 * PARALLEL MAP - parse all lines via distributing character indexes, this let's us do it without worrying about 
 splitting lines, i.e. if barrier crosses halfway along line, whole line will be parsed by the left of the split (not 
 sure if this makes sense) 
 * PARALLEL REDUCE - report count of appearance per type of indexable item in the file
 * SYNCHRONISE
 * PARALLEL MAP - now the indexes for all the items are known, they can be put in unshared copies of the array
 * SERIAL VECTORISED COMBINE - combine the unshared arrays into shared arrays using vectorised OR operator, or
 branching, or addition

