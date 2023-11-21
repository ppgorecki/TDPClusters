import matplotlib.pyplot as plt
import seaborn as sns
sns.set_theme()

import pandas as pd

sns.set(font_scale = 0.5)
sns.set_style("dark")
# Load the example flights dataset and convert to long-form
cubedata = pd.read_csv("cubetest.tsv",sep="\t")
pd.set_option('display.max_rows', 500)
pd.set_option('display.max_columns', 500)
pd.set_option('display.width', 1000)


cubedata["PrcMinDiff"] = 100*(cubedata["Score"]-cubedata["OptScore"])/cubedata["OptScore"]
cubedata = cubedata.drop_duplicates(subset=["Kval", "BlobSize"])

#title="OptCnt"
#title="PrcAvgDiff"
title="PrcMinDiff"

dt = cubedata.pivot( "Kval", "BlobSize", title)

# Draw a heatmap with the numeric values in each cell
f, ax = plt.subplots(figsize=(100, 7))
sns.heatmap(dt, annot=True, fmt=".0f", linewidths=0, ax=ax)
plt.savefig(f"{title}.pdf")
plt.show()


