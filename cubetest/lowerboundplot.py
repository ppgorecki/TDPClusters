import matplotlib.pyplot as plt
import seaborn as sns
from lowerbound import lowerboundsk, cube_sk
sns.set_theme()

import pandas as pd


sns.set(font_scale = 0.5)
sns.set_style("dark")
# Load the example flights dataset and convert to long-form
cubedata = pd.read_csv("cubetest.tsv",sep='\t')
pd.set_option('display.max_rows', 500)
pd.set_option('display.max_columns', 500)
pd.set_option('display.width', 1000)

print(cubedata)

cubedata = cubedata.drop_duplicates(subset=["Kval", "BlobSize"])
cubedata["lowerbound"] = 0.0
cubedata["lowerbounderr"] = 0.0

for i,j in cubedata.iterrows():
   #j["lowerbound"] = 
   lb = lowerboundsk(j["Kval"],*(map(int,j['Blob'][1:].split("x"))))
   csk = cube_sk(j["Kval"],*(map(int,j['Blob'][1:].split("x"))))
   cubedata.loc[i, 'lowerbound'] = lb
   
   cubedata.loc[i, 'lowerbounderr'] = j["OptScore"]-lb  # - draw just cost difference

   # uncomment below to draw percentage difference
   # cubedata.loc[i, 'lowerbounderr'] = (j["OptScore"]-lb)/j["OptScore"]  

   if (j["OptScore"]-lb):
   		print ("Float representation problems detected",j["Kval"],j["Blob"],j["BlobSize"],j["OptScore"],lb,csk)

title="lowerbounderr"
dt = cubedata.pivot( "Kval", "BlobSize", title)

# Draw a heatmap with the numeric values in each cell
f, ax = plt.subplots(figsize=(100, 7))
sns.heatmap(dt, annot=True, fmt=".2f", linewidths=0, ax=ax)
plt.savefig(f"{title}.pdf")
plt.show()
