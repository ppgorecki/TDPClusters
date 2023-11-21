
# TDP Clusters

Software for computing lower and upper bounds for the k-separator problem.

Cluster extent inference revisited: quantification and localisation of brain activity 
Jelle J Goeman, Paweł Górecki, Ramin Monajemi, Xu Chen, Thomas E Nichols, Wouter Weeda,
Journal of the Royal Statistical Society Series B: Statistical Methodology, Volume 85, Issue 4, September 2023, Pages 1128–1153, https://doi.org/10.1093/jrsssb/qkad067

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

Consider a set *V* of voxels from in *Z<sup>3</sup>*, where *Z* is the set of integers. Two distinct voxels *v* and *w* are neighbors if the vector *v-w* belongs to the set *{-1, 0, 1}<sup>3</sup>*. This structure is recognized as a 26-connectivity graph on V.

The Fgreedy heuristic estimates upper bounds for the following problem:

**Given**: A collection of voxels *V* in *Z<sup>3</sup>* and a positive integer *k*.

**Find**: The size of the smallest k-separator for the 26-connectivity graph on *V*. Here, a k-separator is represented by a subset *W* of *V*, such that the removal of W from the connectivity graph results in a graph wherein all components (called clusters) have a size of at most *k*.

This problem, when extended to general graphs, is commonly known as the k-separator problem (https://doi.org/10.1007/s10878-014-9753-x).

#### Examples

Given a 2D-square *V={1,2,3} x {1,2,3} x {0}* and *k=3*, the solution is 3 with two columns (rows) of the size 3
separated by the middle column (row). See [square.tsv](data/square.tsv) in data directory.

Another example is in the article on Figure 2. See [figure2.tsv](data/figure2.tsv) in the data directory.

Run `fgreedy` the command line after compilation with `make`:
```
cd upperbounds && make && cd ../data
../upperbounds/fgreedy figure2.tsv -k10 -T4
```
Sample output:
```
input:figure2.tsv
Config: k=10 n=-1 N=20.000 S=750344822 T=4.00 PS=dflt 
   1. minscore=24    time=0.002
  16. minscore=23    time=0.008
minscore:23
totaltime_sec:4.000
12854297 cluster insertions(s); 3213493.037 clusters per sec.
outpufiles:fgreedy.log;fgreedy.tsv
```

Here, the global running time is set to 4 seconds using `-T4`. The inferred k-separator has a size of 23 and the detailed clustering with separator is available in the default output file [fgreedy.tsv](data/fgreedy.tsv). 
For more advanced usage, refer to additional sections.

To interactively visualize the output from [fgreedy.tsv](data/fgreedy.tsv) in a browser use `pl3d.py` from the `plot` directory.
```
python3 ../plot/pl3d.py
```

![Visualization](/plot/fig2.png)

## Main programs

- `fgreedy` - computing upper bounds from a single input file 

- `batch7.sh` - processing (large) collection of files; calls `fgreedy` multiple times

Run `fgreedy -h` or `batch7.sh` for detailed help.

## Requirements

Gnu compiler is needed for `fgreedy`.

`batch7.sh` is a bash scrupt that requires `gnu parallel` and `env_parallel`

## Compilation 

Run `make` to compile `fgreedy`.

## Input files 

A single input file is a tsv/csv with 3-5-columns:

 - columns 1-3 represents X, Y, Z coordinates of each voxel,

 - column 4 (optional) indicates division, where 0 is a separator voxel,
      and   positive integers denote cluster IDs

 - column 5 (optional and ignored in input) is an integer representing a connected component ID

Lines containing occurences of x, y or, z are ignored. 

See examples in [data directory](/data).

### Config files (.cnf) 

The configuration files (.cnf) contain parameters and rules essential for heuristic runs. We recommend to utilize the provided `batch7.cnf` file from the repository. To integrate this configuration into the `fgreedy` command, append `-x batch7.cnf` to the command line.

### The k parameter = the max size of a cluster

The parameter k can be set by using `-k NUMBER` or by extracting k directly from csv/tsv file name by locating pattern `kvalNUMBER` or `kNUMBER`  e.g., `id1002_kfix_neg_clusterindex10_clustersize256_kval14.csv` gives `k=14`, `k90.csv` gives `k=90`.
If the `-k` option is used to explicitly set the value of k, it takes precedence over any value extracted from the filename.

For example:
```
./fgreedy k90.csv -x batch7.cnf
```
This command runs fgreedy on the input file `k90.csv` with a specified value of `k=90`. 

However, in 
```
./fgreedy k90.csv -x batch7.cnf -k8
```
The value of k is set to 8.



### Output file

Is a 5-column tsv file in the format described above.

Run `fgreedy` with `k=8`:
```
./fgreedy k90.csv -x batch7.cnf -k8
```
Sample output
```
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
./cat fgreedy.tsv | head -6
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
./fgreedy @ FILE KVAL T N
```
where

- FILE - input file name
- KVAL - k value
- T - global time limit in seconds (see -T)
- N - no improvement time threshold in seconds (see -N)

Set N or T to 0 (or negative) to ignore the limit

An example:
```
./fgreedy @ k90.csv 8 1 2
```

A sample output could be:
```
4616
```

### Processing (large) collections of files

Use `batch7.sh` using the following exemplary steps:

##### Prepare dirs

Go to the TDPClusters directory. Make a project dir `myproject` in home directory and `~/myproject/data` and copy your csv/tsv files to the data dir. Here, we copy just the exemaplary dataset.

```
mkdir -p ~/myproject/data
cp data/sample_data_from_neurovault/*.csv ~/myproject/data
```

Note that each tsv file should have kvalNUMBER or kNUMBER denoting the k parameter in the filename, e.g., 
`id1002_kfix_neg_clusterindex10_clustersize256_kval14.csv`.

##### Initialize project.

- Option A. Using the original location of batch7.sh from the source repository.

```
./batch7.sh -i myproject
```

- Option B. Using the project dir and the script from source repository

```
cd ~/myproject
[path to TDPClusters]/upperbounds/batch7.sh -i .
```

- Option C. By copying all needed files manually.

```
cd upperbounds && cp batch7.* fgreedy ~/myproject
cd ~/myproject
./batch7.sh -i .
```

##### Execute project 

Go to the myproject directory.
```
cd ~/myproject
```

Run on 10 jobs with job shuffling, each execution is approx. 240 seconds (-N240). When completed the output dirname provided in `-E` is updated with the total score, i.e., the sum of scores returned by fgreed from all datasets.

```
./batch7.sh -E N240 -N240 -S -j10
```

Lower the values in options to have a  quicker (and less precise) lower bound inference.
The result will be stored in N240__TOTALSCORE directory.

E.g., if the total score is 49 then the directory is:
```
ls N240__*
N240__49
```

##### Continue execution if it was interrupted

Again 10 jobs, shuffling, continue from a dir computed in the previous step. Remember to replace TOTALSCORE with the proper number.

```
./batch7.sh -E N240__TOTALSCORE -N240 -S -j10
```

##### Print progress summary

```
./batch7.sh -P N240
```

##### Merge project to release datadir

```
./batch7.sh -m N240__*
```

For example, if the total score, i.e., the sum of all upper bounds, is SCORE=49, then in `release__49` dir:

- 49.tgz - will contain all output clusterings for each input file one dir with the individual upper bound
- 49.tsv - two-column summary `inputfile` `upperbound` 
- dtstats.tsv - summary of upper bounds for all all non-cubetest datasets
- scstats.tvs - summary of upper bounds for all input datasets
