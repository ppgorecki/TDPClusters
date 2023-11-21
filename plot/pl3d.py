#!/usr/bin/python3

"""
TODO: make it more general with command line options
"""

import plotly.graph_objects as go
import numpy as np
import sys

inputtsvfile = 'fgreedy.tsv'  # default input to visualize

if len(sys.argv)>1:
    inputtsvfile=sys.argv[1];
                    
src = list ( map(lambda l: list(map(int,l.split())) , open(inputtsvfile).readlines() ))

components=set(e[4] for e in src)
print("Components:",components)
clusters=set(e[3] for e in src)
print("Clusters:",clusters)
d=dict((v,0) for v in components)
for e in src: d[e[4]]+=1
print("Components sizes:",d)

vs=src

# Un/comment if you want to see details.
#vs = [ e for e in src if e[4]>=0 ] # skip trivial components
#vs = [ e for e in src if e[4]>=0 and e[3]==0 ] # separators only / non-trivial

#vs = [ e for e in src if e[4]==1  ] # first component (the bigest id=0)
#vs = [ e for e in src if e[4]==2 ] # 2nd component 
#vs = [ e for e in src if e[4]==3 ] # 3rd component

clu=set(e[3] for e in vs)


print(f"Current size of the separator set (id=0): {len(list(e for e in vs if e[3]==0))}")
print("Current sizes of clusters (id>0):") 

for c in clu:
    if c:
        print(f" cluster:{c} size={len(list(e for e in vs if e[3]==c))}")

def ngh(a,col):
    return any( abs(a[0]-b[0])<=1 and abs(a[1]-b[1])<=1 and abs(a[2]-b[2])<=1 for b in col )

# take cluter no ...
#vs = [ e for e in src if e[3]==1  ] # first component (the biggest) separators

def border(vs): return  [ e for e in src if ngh(e,vs) and e not in vs ]


print("Voxels in a diagram:",len(vs))
# see cluster + some local extension
# vs.extend(border(vs))
# vs.extend(border(vs))
# vs.extend(border(vs))


x = [ l[0] for l in vs]
y = [ l[1] for l in vs]
z = [ l[2] for l in vs]
t = [ l[3]+20 if l[3] else 0 for l in vs]  # colors for clusters; separator color is 0 

fig = go.Figure(data=[go.Scatter3d(
    x=x,
    y=y,
    z=z,
    mode='markers',
    marker=dict(
        showscale=True,
        symbol=[ 'square' if q else 'circle'  for q in t ],
        size=12,
        color=t,                # set color to an array/list of desired values
        colorscale='Viridis',   # choose a colorscale
        opacity=0.35 
    )
)])

# tight layout
fig.update_layout(margin=dict(l=0, r=0, b=0, t=0))
fig.show()