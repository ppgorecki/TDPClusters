

#include <string.h>
#include <stdlib.h>

#include "fgreedy.h"


void configsummary(config *cnf, int prallsets)
{
	greedylog("input:%s\n",cnf->inputfile);
	if (cnf->projectinfo)
		greedylog("projectinfo; %s\n",cnf->projectinfo);	
	ppconfig(cnf, prallsets);			
}


#define RESETTEXT  "\x1B[0m" // Set all colors back to normal.
#define FOREBLK  "\x1B[30m" // Black
#define FORERED  "\x1B[31m" // Red
#define FOREGRN  "\x1B[32m" // Green
#define FOREYEL  "\x1B[33m" // Yellow
#define FOREBLU  "\x1B[34m" // Blue
#define FOREMAG  "\x1B[35m" // Magenta
#define FORECYN  "\x1B[36m" // Cyan
#define FOREWHT  "\x1B[37m" // White

#define BACKBLK "\x1B[40m"
#define BACKRED "\x1B[41m"
#define BACKGRN "\x1B[42m"
#define BACKYEL "\x1B[43m"
#define BACKBLU "\x1B[44m"
#define BACKMAG "\x1B[45m"
#define BACKCYN "\x1B[46m"
#define BACKWHT "\x1B[47m"


void _ppspacem(component *comp, 
	int mark, char *markinfo,
	voxelid *others, int otherslen, char *othersinfo,
	voxelid *others2, int otherslen2, char *othersinfo2
	)
{


 	// printf("\e(0\x6a\e(B "); // 188
  //   printf("\e(0\x6b\e(B "); // 187
  //   printf("\e(0\x6c\e(B "); // 201
  //   printf("\e(0\x6d\e(B "); // 200
  //   printf("\e(0\x6e\e(B "); // 206
  //   printf("\e(0\x71\e(B "); // 205
  //   printf("\e(0\x74\e(B "); // 204
  //   printf("\e(0\x75\e(B "); // 185
  //   printf("\e(0\x76\e(B "); // 202
  //   printf("\e(0\x77\e(B "); // 203
  //   printf("\e(0\x78\e(B "); // 186

#define BAR "\e(0\x78\e(B"


	voxelid space2id[comp->g->range.x+3][comp->g->range.y+3][comp->g->range.z+3]; 
	int err=0;
	for (int x = 0; x < comp->g->range.x+2; ++x)
		for (int y = 0; y < comp->g->range.y+2; ++y)
			for (int z = 0; z < comp->g->range.z+2; ++z)			
				space2id[x][y][z]=OUTSIDE_SPECID; 

	statustype vstatus[comp->len+1];	
	
	for (int i = 1; i <= comp->len; ++i)
	{
		space2id[comp->v[i]->x][comp->v[i]->y][comp->v[i]->z]=i;
		vstatus[i]=0;
	}


	for (int cur = 1; cur <= comp->len; ++cur)
	{
		voxel *v = comp->v[cur];
		vstatus[space2id[v->x][v->y][v->z]]=comp->status[cur];		
		// printf("%d: (%d,%d,%d) %d %d stat=%d\n",cur,v->x,v->y,v->z,v->clusterid,v->compid,comp->status[cur]);
	}


	for (int z = 1; z < comp->g->range.z+1; ++z)
	{
		printf("Slice z=%d\n",z);
	
		for (int y = comp->g->range.y+1; y>=0; --y)
		{
			printf("y=%-3d%s",y,BAR);
			for (int x = 0; x < comp->g->range.x+2; ++x)
			{
				int s=0;
				int ocnt=0,ocnt2=0;
				if (space2id[x][y][z]!=OUTSIDE_SPECID)
				{
					s = vstatus[space2id[x][y][z]];

					if (others)
					{
						// check 
						for (int i = 0; i < otherslen; ++i)
							if (others[i]==space2id[x][y][z]) ocnt++;
					}
					if (others2)
					{
						// check 
						for (int i = 0; i < otherslen2; ++i)
							if (others2[i]==space2id[x][y][z]) ocnt2++;
					}
				}
#define VIS  BACKRED FOREBLK "v"
#define VIS2 BACKMAG FOREBLK "V"
#define VISA BACKWHT FOREBLK "W"
				int src = s;
				int s0=s & ~(SP_VISITED|SP_VISITED2);

#define CLU	BACKCYN FOREBLK 
#define SEP	BACKBLK FOREBLU  

#define MARKED BACKYEL FOREBLK
#define OTHERS BACKGRN FOREBLK
#define OTHERS2 BACKBLU FOREWHT
#define CVOXEL "\uF04D "
#define CSEP "\uF00D "

				char *l = "";

				printf(RESETTEXT);	

				int sp=0;
				

				if (s&SP_VISITED && s&SP_VISITED2) printf(VISA);	
				else if (s&SP_VISITED) printf(VIS);
				else if (s&SP_VISITED2) printf(VIS2);	
				else if (s0==(SP_CLUSTER|SP_VOXEL)) printf(CLU " ");
				else if (s0==(SP_VOXEL|SP_SEPARATOR)) printf(SEP " ");
				else sp++;

				if (mark>=0) 
					if (mark==space2id[x][y][z])
						printf(MARKED "X");
					else sp++;
				else sp++;

				if (others)
				{				
						if (ocnt)
							printf(OTHERS "%d",ocnt);
						else sp++;
					}
				else sp++;
				if (others2)
				{				
						if (ocnt2) printf(OTHERS2 "%d",ocnt2);
						else sp++;
					}
				else sp++;

				if (s0==(SP_CLUSTER|SP_VOXEL)) printf(CLU);
				else if (s0==(SP_VOXEL|SP_SEPARATOR)) printf(SEP);
				else printf(RESETTEXT);

				while (sp-->0) printf(" ");

				int dbl=0;

				if      (s0==SP_OUTSIDE)   printf("\033[44m  ");
				else if (s0==SP_VOXEL)     printf(CVOXEL);
				else if (s0==(SP_CLUSTER|SP_VOXEL))  printf(CLU "  ");
				else if (s0==(SP_SEPARATOR|SP_VOXEL)) printf(SEP CSEP);
				else if (s0==SP_SEPARATOR) printf("s?");
				else if (s0==SP_CLUSTER) printf("c?");
				else { 
					printf("  ");
					// dbl=1;
					// if (s)
					// 	printf("%-2d",s);
					// else printf("  ");
				}

				// if  (sg)   printf(" ");
				// else if (!dbl) printf(" ");
				if (src)
				 	printf("%-2x",src);	
				else printf("  ");

				printf(RESETTEXT);		
				printf(BAR);	

			}
			printf("\n");
		}
		printf("     " BAR);
		for (int x = 0; x < comp->g->range.x+2; ++x)
			printf(" x=%-2d   %s",x,BAR);
		printf("\n");
		printf(".=voxel "
			CLU "=clu+vox \033[0m"
			SEP "=sep+vox\033[0m " 
			VISA"=vis+vis2\033[0m " VIS "=vis\033[0m " 
			VIS2 "=vis2\033[0m "
			MARKED "X=%s\033[0m " 
			OTHERS "O=%s(len=%d)" RESETTEXT 
			OTHERS2 "O=%s(len=%d)" RESETTEXT 
			"\n", 
			(markinfo)?markinfo:"marked",
			(othersinfo)?othersinfo:"others1",
			(others)?otherslen:-1,
			(othersinfo2)?othersinfo2:"others2",
			(others2)?otherslen2:-1
			);
			
	}			

	if (err) exit(-14);
}

void ppcode(statustype n, graph *g) 
{ 		
		printf("##%d\t",n);		
		if (n&SP_VOXEL) printf(CVOXEL " " RESETTEXT);  else printf("  ");
		if (n&SP_SEPARATOR) printf(CSEP RESETTEXT); else printf("  ");
		if (n&SP_VISITED2) printf(VIS2 RESETTEXT); else printf("  ");
		if (n&SP_VISITED) printf(VIS RESETTEXT); else printf("  ");
		if (n&SP_CLUSTER) printf(CLU "c" RESETTEXT);else printf("  ");
		if (n&SP_OUTSIDE) printf(" outside"); 
}


void ppcomponent(component *comp)
{
	printf("\ncid=%d\n",comp->cid);
	printf("tscore=%d\n",comp->tscore);
	printf("minscore=%d\n",comp->minscore);
	printf("validminscoredata=%d\n",comp->validminscoredata);
	printf("len=%d\n",comp->len);
	printf("hasclustering=%d\n",comp->hasclustering);
}


void ppparameters(config *cnf, parameters *p)
{	
	if (p->comp) greedylog("#%d ",p->comp->cid);
	greedylog("PS:%s r=%d T=%.2f s=%d a=%.2f A=%.2f R=%d q=%.2f M=%d E=%d e=%d continue=%d",
		p->name,p->runs_r,p->timelimit_T,p->candidateclusterscnt_s, 
		p->allowedmissingvoxels_a,p->allowedmissingvoxelsstep_A,p->randomizeallowedstep_R,
		p->resamplelocalthreshold_q,p->maxclusterinserted_M,p->resampleruns_E,p->resamplecount_e, p->mscontinue);
	if (p->initial)
		 greedylog(" repeats=%d",p->repeats);
}


void ppconfig(config *cnf, int prallsets)
{
	greedylog("Config: k=%d n=%d N=%.3f S=%d T=%.2f",
		cnf->optk,
		cnf->noimprovementrunthreshold,
		cnf->noimprovementtimethreshold,
		cnf->randomseed,
		cnf->globaltimelimit
		);
		
	if (prallsets)
	{
		greedylog("\n");
		for (int i=0;i<cnf->parsetlen;i++)
		{
			ppparameters(cnf,cnf->parsets+i);
		 	greedylog("\n");
		}
	}
	else
	{

		greedylog(" PS=");
		
		for (int i=0;i<cnf->parsetlen;i++)
		{
			greedylog("%s ",cnf->parsets[i].name);
		}
		greedylog("\n");
	}		
}



void ppgraph(graph *g, int printvoxels, config *cnf)
{
	greedylog("voxels:%d\n",g->vlen);		
	greedylog("dim:");
	ppvoxellog(g->range);	
	greedylog("\n");
	if (printvoxels)
	 	for (int i = 1; i <= g->vlen; ++i)	
	 		ppvoxellog(g->vx[i]);
}

void pptimestats(graph *g, config *cnf, int step)
{	
	for (int cid = 0; cid < g->complen; cid++)
		greedylog("%6.2f|%-6.2f",g->comp[cid]->totaltime,g->comp[cid]->resamplingtime);						
	
}

void ppimpstats(graph *g, config *cnf, int step)
{	
	char buf[1000];
	for (int cid = 0; cid < g->complen; cid++)
	{
		sprintf(buf,"%d",g->comp[cid]->minscore);		
			// printf("%6d",g->comp[cid].minscore);	
			// //printf("%2d",step-g->comp[cid].improvementstep);
		if (step==g->comp[cid]->improvementstep && g->comp[cid]->impscore>0)
		{
			sprintf(buf+strlen(buf),"|%d",g->comp[cid]->impscore);
			if (g->comp[cid]->resamplingimproved) 
				sprintf(buf+strlen(buf),":%d",g->comp[cid]->resampleruns);
		}
		greedylog("%-12s ",buf);							
	}
}

void ppscoreg(graph *g, bool activeonly, config *cnf)
{	

	int s=g->easyplus;
	for (int i=0; i<g->complen; i++)	
	{
		if (!g->comp[i]->hasclustering) return; // ignore
		s+=g->comp[i]->minscore;		
	}

	int t=g->easyplus;
	int err=0;
	for (int j=0; j<g->complen; j++)	
	{
		int curscore = g->comp[j]->minscore;				
		for (int i=0; i<g->cpoollen; i++)						
			if (g->cpool[i]->cid==j && g->cpool[i]->hasclustering && g->cpool[i]->minscore < curscore)		 
			{
				err++; 
				curscore = g->cpool[i]->minscore;
			}
		t+=curscore;
	}
	
	greedylog("ScoreData: easyplus=%d minscorebycomp=%d g.minscore=%d truescore=%d err=%d\n",
		g->easyplus,s,g->minscore,t,err);

	double now = gettime();

	for (int i=0; i<g->cpoollen; i++)	
	{
		component *comp = g->cpool[i];
		if (activeonly && !comp->active) continue;
		greedylog("   %d. #%d ms=%d time=%.3f imprtime=%.3f %s%s\n",i,
			comp->cid,
			comp->minscore,
			// comp->tscore,
			comp->totaltime,
			(comp->timeimproved>0)?(now - comp->timeimproved):0,
			(g->comp[comp->cid]==comp)?"*":"",
			(!comp->active)?"-":"");	
	}
	if (err)
		exit(-1);



}

void usage(config *cnf)
{
	printf(		
"fgreedy - minimizing separators  version 0.91\n"

"Usage: fgreedy options FILE\n"
"   or  fgreedy @ FILE KVAL GLOBALTIMELIMIT NOIMPROVEMENTTIMETHRESHOLD\n"
"...\n"

"Input is a tsv file with 3-5 columns\n"
" - columns 1-3 are X Y Z coordinates of each voxel,\n"
" - column 4 (optional) denotes division, where 0 is a separator voxel,\n"
"      and a positive integer denotes a cluster id\n"
" - column 5 (optional, ignored in input) an integer denoting connected component id\n"
" - lines with occurences of x, y or z are ignored\n"
"\n"
"Output is a tsv file with 5 columns. Trivial clusters, i.e. belonging to the\n"
"connected components with the size at most <=k+1, will have negative identifiers in the 5th column.\n"
"Additionaly, fgreedy writes a log file.\n"

"\n"
"Command line options:\n"
" -h - help\n\n"
" -k NUM - the value of k; if not provided k value is extracted from filename using pattern kvalNUM or kNUM.\n"
" -x STR|FILE - defines sets of parameters; the string consists of assignment of paramteres separated by white spaces or semicolons. Alternatively parameters can be stored in a file.\n"
" -b STATSFILE - writes a summary tsv file with: project name, score, k, voxel count, size >k components, all components, total runs (in executed PSs), total time, score improvements, last improvement run, time of the last improvement, clusters inserted in millions, speed in millions of clusters inserted per second; to print header see -vb option\n"

"Global parameters (-x):\n"
"\tS=NUM - set random seed to NUM\n"
"\tcontinue=NUM - 1 indicates that the PS will continue from the last clustering\n"
"\tinitial=NUM - 1 indicates that the PS will be executed as first\n"

"Refill clustering parameters in -x:\n"
"\tr=NUM - then number of runs\n"
"\ts=NUM - the number of candidate clusters\n"
"\tT=NUM[m|h] - timelimit in seconds (def. unlimited); NUMm - minutes, NUMh - hours\n"
"\ta=NUM|RNUM - allowed missing voxels\n"
"\tA=NUM|RNUM - allowed missing voxels step\n"
"\t\tin -a and -A: if RNUM is between [0..1.0) the value is set to RNUM*kval\n"
"\tR=0|1 - if R=1 A-step will be randomized\n"
"Local sampling parameters in -x:\n"
"\tq=RNUM - max. part of the component to be cleared (between 0.0 and 1.0)\n"
"\tM=NUM - max. number of clusters to be removed (similar to q)\n"
"\te=NUM  - the number of small runs of resampling\n"
"\tE=[-]NUM - max number resamplings per each big run and each component without improvement\n"
"\t\twith negative the number of runs should be controlled by time constraints\n"
"Other instructions in -x:\n"
"\tPS:LABEL - defines a new set of parameters LABEL\n\n"
"\tsize|kval(<|<=|==|<>|>=|>)(NUM|RNUM) - constraints to limit applications of PS depending on kval and/or size\n\n" 

"Global time limit (when running many paremeter sets)\n"
"\t-T=NUM[m|h] - time limit in seconds (def. unlimited); NUMm - minutes, NUMh - hours \n"
"\n"

"Global stopping criterions:\n"
"\t-N=NUM[m|h] - stop when no improvement after given time (def. %.2f sec.)\n"
"\t-n=NUM - stop when no improvement after NUM PSs (def. %d)\n"

"Other options:\n"
"\t-p PROJECTNAME - set PROJECTNAME; default from the input filename; now only used with -vx\n"
"\t-l logfile - sets log file name\n"
"\t-t tsvfile - sets tsv file name (separator+clusters; 5 columns)\n"
"\n"
"\t-f FILESUFFIX - defines addition suffix in filenames; def. empty string\n"
"\t-i STR - project info\n"
"\t-c - read file only (checking)\n"
"\t-C DIR - continue from the last best clustering; if DIR is - then DIR is determined by PROJECTNAME (-p) or FILE (if -p is not used)\n"
"\t\tinput file is located using pattern DIR/MINSCORE[^0-9]*.tsv\n"
"\t\tif the best clustering does not exists FILE is used\n"
"\t-O NUM - optimize component no #NUM only (some random clustrings are also inferred for other components)\n"
"\t-s SCORE - stop when the give SCORE is reached\n"

"Verbose:\n"
"\t-v SUBOPTs\n"
"\t\ti - skip easy components when writing tsv file\n"
"\t\tt - print improvements stats\n"
"\t\tT - print timing stats\n"
"\t\tS - save tsv file after each improvement\n"
"\t\tx - move tsv and log to MINSCOREFILESUFFIX.tsv and MINSCOREFILESUFFIX.log in PROJECTNAME dir\n"
"\t\tX - move tsv to MINSCOREFILESUFFIX.tsv in PROJECTNAME dir\n"
"\t\tq - do not print on stdout\n"
"\t\tQ - do now write output files\n"
"\t\tf - force writing tsv even if there is no improvement vs initial clustering\n"
"\t\tG - print input graph stats\n"
"\t\tr - print improvements in the local resampling algorithm\n"
"\t\t0 - print minscores in the initial score (from comp. pool)\n"
"\t\t1 - details on phase 1 of algorithm\n"
"\t\t2 - details on phase 2 of algorithm\n"
"\t\t3 - print scores after phases\n"
"\t\t4 - print scores after all cluster insertions\n"
"\t\t5 - print PS for each component\n"
"\t\ts - print final PS stats and fitness for component pool\n"
"\t\tF - print detailed PS fitness stats\n"
"\t\tm - clever writing to PROJECTNAME dir (do not multiple copies, minscore tsv's)\n"
"\t\tp - print file info with minscore improvements\n"
"\t\tP - print parameter sets in config summary\n"
"\t\tR - print improvements in resampling (minscore prints)\n"
"\t\tb - print brieftsv header and exit\n"
"\n"
"Examples:\n"
"\n"
"Run using batch7.cnf and print input graph stats\n"
"\tfgreedy -x batch7.cnf -vs -k657 k657.csv\n\n"

"Run for 10 seconds\n"
"\tfgreedy -x batch7.cnf -vs -k657 -T10 k657.csv\n\n"

"As above but store log and tsv in project directory (here k657).\n The score will be encoded in the filename.\n"

"\tfgreedy -x batch7.cnf -vsx -k657 -T10 k657.csv\n\n"

"As above but continue from the best clustering found the directory k657.\n Output written to dir k657\n"
"\tfgreedy -x batch7.cnf -vsx -k657 -T10 -C- k657.csv\n\n"

"Simplified runs:\n"
" fgreedy @ FILE KVAL T N\n"
"\tFILE - input file name\n"
"\tKVAL - k value\n"
"\tT - global time limit in seconds (see -T)\n"
"\tN - no improvement time threshold in seconds (see -N)\n"
"Set N or T to 0 (or negative) to ignore the limit\n"
"An example:\n"
"fgreedy @ k90.csv 90 0 2\n"

"\nRun using batch7.cnf and print PS stats (quite long in batch7.cnf)\n"
"\tfgreedy -x batch7.cnf -vGs -k657 k657.csv\n\n"

,
cnf->noimprovementtimethreshold,
cnf->noimprovementrunthreshold

		);
}


