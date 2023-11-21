
# TDP Clusters

Software for computing lower and upper bounds for the k-separator problem.
See https://doi.org/10.1093/jrsssb/qkad067

## Directories 

Main directories:
- `lowerbouds` - a python library `lowerbounds.py` for computing lower bounds based on Lemma 5 from the article
- `upperbounds` - `fgreedy`  written in C to compute upper bounds

Other directories:
- `data` - exemplary input data (e.g., figure2.tsv data from the Figure 2)
- `plot` - plotting fgreedy output clusterings
- `cubetest` - heuristic cube test from the article

# Fgreedy - upper bounds

`fgreedy` is a computer program written in C to compute the upper bounds for the k-separator problem on 
26-connectivity graphs defined in the 3-dimensional grid.

## Problem definition 

Consider a set *V* of voxels from in *Z<sup>3</sup>*, where *Z* is the set of integers. Two distinct voxels *v* and *w* are neighbors if the vector *v-w* belongs to the set *{-1, 0, 1}^3*. This structure is recognized as a 26-connectivity graph on V.

The Fgreedy heuristic estimates upper bounds for the following problem:

**Given**: A collection of voxels *V* in *Z^3* and a positive integer *k*.

**Find**: The size of the smallest k-separator for the 26-connectivity graph on *V*. Here, a k-separator is represented by a subset *W* of *V*, such that the removal of W from the connectivity graph results in a graph wherein all components (called clusters) have a size of at most *k*.

This problem, when extended to general graphs, is commonly known as the k-separator problem (https://doi.org/10.1007/s10878-014-9753-x).

#### Examples

Given a 2D-square *V={1,2,3} x {1,2,3} x {0}* and *k=3*, the solution is 3 with two columns (rows) of the size 3
separated by the middle column (row). See [square.tsv](data/square.tsv) in data directory.

Another example is in the article on Figure 2. See [figure2.tsv](data/figure2.tsv) in the data directory.

To test it with `fgreedy` execute from the command line in `upperbounds` directory after compilation with `make`:
```
$ cd upperbounds && make
$ ./fgreedy ../data/figure2.tsv -x batch7.cnf  -k10 -T4
input:../data/figure2.tsv
Config: k=10 n=-1 N=20.000 S=-2122953728 T=4.00 PS=dflt X0 X1 X2 X2 X5 X10 X20 X30 X40 X50 X60 X70 X90 X100 X110 X130 X150 X200 X300 X400 X500 X600 X800 X1002 I60 I300 I400 I600 I1k X1002 I2k I3k 
  20. minscore=24    time=0.022
  22. minscore=23    time=0.101
minscore:23
totaltime_sec:4.000
12249841 cluster insertions(s); 3062390.705 clusters per sec.
outpufiles:fgreedy.log;fgreedy.tsv
```
Here, `-T4` sets the global running time to 4 seconds. The inferred k-separator has the size of 23 and the detailed clustering with separator is in `fgreedy.tsv`.

To visualize the output from `fgreedy.tsv` in a browser use `pl3d.py` from `plot` directory.
```
python3 ../plot/pl3d.py
```

![Visualization](/plot/fig2.png)


## Requirements

- `fgreedy` - computing upper bounds from a single input file 

- `batch7.sh` - processing (large) collection of files; calls `fgreedy` multiple times

Run `fgreedy -h` or `batch7.sh` for detailed help.

## Requirements

Gnu compiler is needed for `fgreedy`.

`batch7.sh` is a bash scrupt that requires `gnu parallel` and `env_parallel`

## Compilation 

Run `make` to compile `fgreedy`.

## Input files 

A single input file is a tsv/csv 3-5-column file:

 - columns 1-3 are X Y Z coordinates of each voxel,

 - column 4 (optional) denotes division, where 0 is a separator voxel,
      and a positive integer denotes a cluster id

 - column 5 (optional, ignored in input) an integer denoting connected component id

 - lines with occurences of x, y or z are ignored

Examples:
```
$ cat k90.csv | head -6
x,y,z
7,27,32
7,28,31
7,28,32
7,28,33
7,28,34
```

### The k parameter = the max size of a cluster

The parameter k can be set by using:

- `-k NUMBER` in `fgreedy` 
- or, by extracting k directly from csv/tsv file name by locating pattern `kvalNUMBER` or `kNUMBER`  e.g., `id1002_kfix_neg_clusterindex10_clustersize256_kval14.csv` gives `k=14`, `k90.csv` gives `k=90`.

Setting with `-k` has higher priority. 
The second variant is required for executing `batch7.sh`. 

Run `fgreedy` on `k90.csv` with with `k=90`.
```
$ fgreedy k90.csv -x batch7.cnf
```

### Config files (.cnf) 

They contain parameters and rules for the heuristic runs. We recommend using `batch7.cnf` attached to the repository. Add `-x batch7.cnf` to the command of `fgreedy`.

### Output file

Is a 5-column tsv file in the format described above.

Run `fgreedy` with `k=8`:
```
$ fgreedy k90.csv -x batch7.cnf -k8
input:k90.csv
Config: k=8 ...
2230. minscore=4599  time=4.835
2231. minscore=4297  time=5.085
...
3552. minscore=4115  time=198.061
3570. minscore=4113  time=199.977
minscore:4113
totaltime_sec:245.359
269077803 cluster insertions(s); 1096671.935 clusters per sec.
outpufiles:fgreedy.log;fgreedy.tsv
```

Output file:
```
$ cat fgreedy.tsv | head -6
7       27      32      314     1
7       28      31      0       1
7       28      32      314     1
7       28      33      0       1
7       28      34      308     1
7       29      31      0       1
```

### Simplified run - just the score (no output files)

To get just the score run `fgreedy` as follows
```
fgreedy @ FILE KVAL T N
```
where

- FILE - input file name
- KVAL - k value
- T - global time limit in seconds (see -T)
- N - no improvement time threshold in seconds (see -N)

Set N or T to 0 (or negative) to ignore the limit

An example:
```
$ fgreedy @ k90.csv 8 1 2
4616
```

### Processing (large) collections of files

Use `batch7.sh` using the following exemplary steps:

##### Make a project dir, e.g., myproject and myproject/data and copy csv/tsv files to data dir

```
mkdir -p myproject/data
cp sample_data_from_neurovault/data/*.csv myproject/data 
```

Note that each tsv file should have kvalNUMBER or kNUMBER denoting the k parameter in the filename, e.g., 
`id1002_kfix_neg_clusterindex10_clustersize256_kval14.csv`.

##### Initialize project.

- Option A. Using the original location of batch7.sh from the source repositoty.

```
cd fMRI_crit_nodes/125_greedy
batch7.sh -i myproject
```

- Option B. Using the project dir and the script from source repository

```
cd myproject
fMRI_crit_nodes/125_greedy/batch7.sh -i .
```

- Option C. By copying all needed files manually.

```
cd neurovault
cp fMRI_crit_nodes/125_greedy/batch7.* fgreedy .
batch7.sh -i .
```

##### Execute project 

Here run on 50 jobs with job shuffling. When completed the output dirname provided -E is updated with the score.

```
batch7.sh -E N240 -N240 -S -j50
```

Lower the values in options to have a quicker (and less precise) lower bound inference.
The result will be stored in N240__SCORE directory.

##### Continue execution

Again 50 jobs, shuffling, continue from a dir computed in the previous step (replace SCORE with the number).

```
batch7.sh -E N240__SCORE -N240 -S -j50
```

##### Print progress summary

```
batch7.sh -P N240
```

##### Merge project to release datadir

```
batch7.sh -m N240__*
```

For example, if the total score, i.e., the sum of all upper bounds, is SCORE=49, then in `release__49` dir:

- 49.tgz - will contain all output clusterings for each input file one dir with the individual upper bound
- 49.tsv - two-column summary `inputfile` `upperbound` 
- dtstats.tsv - summary of upper bounds for all all non-cubetest datasets
- scstats.tvs - summary of upper bounds for all input datasets
