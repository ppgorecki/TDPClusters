

#include <stdlib.h>
#include <sys/types.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <dirent.h>

#include <libgen.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "fgreedy.h"

void initparams(parameters *p, parameters *src)
{
  	
	if (src)
		*p=*src;	
	else
	{
		p->runs_r = 10;
		p->candidateclusterscnt_s = 5;
		p->allowedmissingvoxelsstep_A = 5;
		p->allowedmissingvoxels_a = 5;
		p->randomizeallowedstep_R = 0;
		p->maxclusterinserted_M = 10000000;
		p->resamplecount_e = 5;
		p->resampleruns_E = 10; //-E
		p->resamplelocalthreshold_q = 0.3;
		p->timelimit_T = -1;
		p->runsset = false;		
		p->conditions = NULL;
		p->insertclusteradv = 0.2;

		p->initial = false;
		p->mscontinue = true;
		p->comp = NULL; 
		p->repeats = 1;
	}

	p->name="dflt";		
}

 
void greedyloginit(char *fn)
{
	FILE *out = fopen(fn,"w"); // do it better...
	if (!out)
	{
		fprintf(stderr, "Cannot open %s\n", fn);
		exit(-1);
	}
	fclose(out);
}
	
void greedylogf(FILE *f, const char *fmt,...)
{	
	va_list ap;    
    va_start(ap, fmt);
    vfprintf(f, fmt, ap);        
    va_end(ap);
}        


double gettime()
{
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    return now.tv_sec + now.tv_nsec*1e-9;
}


double gettimestr(char *s)
{
   	int mn = 1;
   	char *o = strdup(s);
   	if (o[strlen(o)-1]=='m') { mn=60; o[strlen(o)-1]=0; }
   	else if (o[strlen(o)-1]=='h') { mn=3600; o[strlen(o)-1]=0; }       	
  	double res = atof(o)*mn;
  	free(o);
  	return res;
}




int getop(char *op)
{

	if (strlen(op)==1)
		switch (op[0])
		{
			case '>': return OGt;
			case '<': return OLt;
			case '=': return OAssignment;
			case ':': return OPS;
		}
	else if (strlen(op)==2)
	{
		if (op[0]=='<' && op[1]=='=') return OLe;
		if (op[0]=='<' && op[1]=='>') return ONe;
		if (op[0]=='>' && op[1]=='=') return OGe;
		if (op[0]=='=' && op[1]=='=') return OEq;
	}	
	return 0;
}

const char *parameterlabels = "saAreERSMqT@vk";

long getint(char *label, char *val)
{	
	char *ep;
	long ret = strtol(val,&ep,10);
	if (strlen(ep)) 
	{
		fprintf(stderr,"Cannot parse integer from parameter assignment %s=%s <%s>\n",label,val,ep);
		exit(-3);
	}
	return ret;
}


float getfloat(char *label, char *val)
{	
	char *ep;
	float ret = strtof(val,&ep);
	if (strlen(ep)) 
	{
		fprintf(stderr,"Cannot parse float from parameter assignment %s=%s <%s>\n",label,val,ep);
		exit(-3);
	}
	return ret;
}



void setparameter(char *label, char *val, parameters *p, config *cnf)
{
	if (strlen(label)>1)
	{
		if (!strcmp("continue",label)) 
		{
			p->mscontinue = atoi(val);
			return;
		}
		if (!strcmp("initial",label)) 
		{
			p->initial = atoi(val);
			return;
		}
		if (!strcmp("poolreduction_threshold",label)) 
		{
			p->poolreduction_threshold = atof(val);
			return;
		}
		if (!strcmp("poolreduction_eqthreshold",label)) 
		{
			p->poolreduction_eqthreshold = atof(val);
			return;
		}
		if (!strcmp("repeats",label)) 
		{
			p->repeats = atoi(val);
			return;
		}
		if (!strcmp("insertclusteradv",label)) 
		{
			p->insertclusteradv = atof(val);
			return;
		}
		if (!strcmp("reducepool",label)) 
		{
			p->reducepool = atoi(val);
			return;
		}
		if (!strcmp("poolreduction_resetfittness",label)) 
		{
			cnf->poolreduction_resetfittness = atoi(val);
			return;
		}
	}
	else if (strlen(label)==1)
	switch (label[0])
	{		

		case 'a':            
        	p->allowedmissingvoxels_a = getfloat(label,val); 
        	return;
    	
      	case 'A':
        	p->allowedmissingvoxelsstep_A = getfloat(label,val); 
        	return;
        	
      	case 'R':
        	p->randomizeallowedstep_R = getint(label,val);
        	return;


      	case 'q':
      		p->resamplelocalthreshold_q = getfloat(label,val); 
			return;        	

      	case 'e':
        	p->resamplecount_e = getint(label,val);
        	return;

        case 'E':
        	p->resampleruns_E = getint(label,val);
        	return;

       	case 's':
       		p->candidateclusterscnt_s = getint(label,val);
        	return;

        case 'r':
       		p->runs_r = getint(label,val);
        	p->runsset=1;
        	return;

	  	case 'S':
        	cnf->randomseed = getint(label,val);
        	        	
        	return;

        case 'M':
            p->maxclusterinserted_M = getint(label,val);
        	return;

      	case 'T': 
	       {	       	
	      	p->timelimit_T=gettimestr(val);	      	
	      	return;	      	
	      }


	}
	fprintf(stderr,"Unknown parameter assignment %s=%s\n",label,val);
	exit(-3);		
}

void readparameters(config *cnf, char *src)
{
	char *b = src;
	FILE *f = fopen(src,"r");
	if (f)
	{
		fseek(f, 0L, SEEK_END);
		long size = ftell(f);
		fseek(f, 0L, SEEK_SET);
		b = (char*)calloc(size+1, sizeof(char));
		if (!b)
		{
    		fprintf(stderr,"Cannot allocate memory to read parameters from file %s",src);
    		exit(-1);
    	}
    	int q = fread(b, sizeof(char), size, f);
		fclose(f);
	}

	// parse b
	
	// remove comments
	char *s = b;
	while (*s)
	{
		if (*s=='#')				
			while (*s && *s!='\n') { *s=' '; s++; }
		s++;
	}
	
	char *delim = " \t\r\n;";
	char *ptr = strtok(b, delim);		
	parameters *curset = cnf->parsets;
	
	int ignore = 0;

	while (ptr != NULL)
	{
		char label[BUFS];
		char oper[3];

		

		// parse expression
		int i=0;		
		while (ptr[i]!=0 && (isalnum(ptr[i]) || ptr[i]=='_')) label[i++]=ptr[i];

		label[i]=0;

		int j=0;
		while (ptr[i]!=0 && strchr("<>=:",ptr[i])) oper[j++]=ptr[i++];
		oper[j]=0;

		int op = getop(oper);

		if (!op)
		{
			fprintf(stderr,
				"Parse error in PS script. Expected operator :, =, <, <=, ==, <>, >=, > in \"%s\"\n",ptr);
			exit(-1);
		}


		char *val = ptr+i;			
		// v,k - sizes 

		if (op==OPS) // new set
		{
			// go to the next 
			if (cnf->parsetlen==MAXPARAMETERSETS)
			{
				fprintf(stderr,"Too many PSs. Increase macro MAXPARAMETERSETS and recompile\n");
				exit(-4);
			}
			curset++;
			cnf->parsetlen++;

			initparams(curset,cnf->parsets); // copy from default
			curset->name = strdup(val); 
		}
		else if (op==OAssignment)
		{			
			setparameter(label,val,curset, cnf);
		} 
		else if (op>=OEq)
		{	
			condition *cnd = (condition*)malloc(sizeof(condition));
			cnd->op = op;
			cnd->next = curset->conditions;
			curset->conditions = cnd;
			if (!strcmp(label,"k") || !strcmp(label,"kval"))
			{				
				cnd->type = CND_KVAL;
				cnd->val = atoi(val);
			} 
			else if (!strcmp(label,"size"))
			{
				cnd->type = CND_SIZE;
				cnd->val = atoi(val);				
			} 
			else if (!strcmp(label,"k2sizeratio"))
			{
				cnd->type = CND_K2SIZERATIO;
				cnd->valf = atof(val);												
			} 
			else
			{
				fprintf(stderr,"Unknown variable in condition %s. Allowed k, kval or size (component size)\n",ptr);
				exit(-1);
			}
		}
			
		ptr = strtok(NULL, delim);
	}
}



void initconfig(config *cnf)
{
	// init first parset
	cnf->_p = cnf->parsets; // first parameters sets (default)
	cnf->parsetlen = 1;
	initparams(cnf->_p,NULL);

	cnf->optk = -1;
	cnf->projectinfo = NULL;
	cnf->logfile = "fgreedy.log";
	cnf->tsvfile = "fgreedy.tsv";
	cnf->projectname = NULL;
	cnf->brieftsv = NULL;
	cnf->filesuffix = "";
	cnf->checkonly = 0;
	cnf->globaltimelimit = -1;	
	cnf->noimprovementrunthreshold = -1; // additional stopping criterion
	cnf->noimprovementtimethreshold = 20.0;  // additional stopping criterion
	cnf->randomseed = 0; 
	cnf->onecomponentoonly = -1; 
	cnf->poolreduction_resetfittness = 1;	 
	cnf->stoponscore = -1;               // stop when given score is reached (for testing)
}


// Returns file name of min score file
// NULL if such a file does not exist

char *locateminscorefile(char *dirname, int *retminscore, int report)
{
 	DIR *d;
    struct dirent *f;
    d = opendir(dirname);
    int minscore=-1;    
    char buf[1000];
    if (d)
    {
        while ((f = readdir(d)) != NULL)
        {
        	char *fn=f->d_name;
        	if (strlen(fn)<5) continue;
        	if (!strstr(fn+strlen(fn)-4,".tsv")) continue;
        	int c=0;
        	while (fn[c]) if (isdigit(fn[c])) c++; else break;
            char *s = strdup(fn);
            s[c]=0;
            int cscore=atoi(s);            
            if (minscore<0 || cscore<minscore)
            {            
            	minscore=cscore;            	
            	sprintf(buf,"%s/%s",dirname,f->d_name);            	
            	//printf("%s %d %s\n", f->d_name,cscore,buf);
            }
            free(s);
        }
        closedir(d);
    }
    if (report && (minscore<0))
    {
    	fprintf(stderr, "No clustering file in %s\n",dirname);
    	exit(-1);
    }

    *retminscore=minscore;
    if (minscore<0) return NULL;
    return strdup(buf);
}





int addvoxel(voxelbuf *b, int x, int y, int z, voxelid clusterid)
{
	if (sizeof(voxelid)==2 && b->len==32767)
	{
		fprintf(stderr, "Large voxel number. Change voxelid type to int by compling with 'gcc -DLARGE ...' or run 'make large'\n");
		exit(-1);
	}

	if (b->len==b->_vsize)		
	{		
		b->_vsize=3*b->_vsize/2;
		b->v=(voxel*)realloc(b->v,b->_vsize*sizeof(voxel));		
	}
	b->v[b->len].x=x;
	b->v[b->len].y=y;
	b->v[b->len].z=z;
	b->v[b->len].clusterid=clusterid; // can be -1 - no info
	b->v[b->len].pos=b->len;	
	return b->len++;
}



void initvoxelbuf(voxelbuf *vs, int vsize)
{
	vs->_vsize = vsize;
	vs->len = 0;	
	vs->v = (voxel*)malloc(vsize*sizeof(voxel));	
	addvoxel(vs,0,0,0,-1);	   // dummy voxel nr 0
}

void initvoxelidbuf(voxelidbuf *vs, int vsize)
{
	vs->_vsize = vsize;
	vs->len = 0;	
	vs->v = (voxelid*)malloc(vsize*sizeof(voxelid));
}


int addvoxelid(voxelidbuf *b, voxelid v)
{
	if (b->len==b->_vsize)		
	{
		b->_vsize=3*b->_vsize/2;
		b->v=(voxelid*)realloc(b->v,b->_vsize*sizeof(voxelid));
	}
	b->v[b->len++]=v;
	return b->len;
}


int readxyz(const char *fn, voxelbuf *vs)
{
	
	initvoxelbuf(vs,10000);

	if (fn[0]==':')
	{	
		// macro 
		// rectangle 2d NUM x NUM 
		// rectangle 3d NUM x NUM x NUM

		int xd,yd,zd;
		if (sscanf(fn+1,"%dx%dx%d",&xd,&yd,&zd)!=3)
		{
			if (sscanf(fn+1,"%dx%d",&xd,&yd)!=2)
			{
				fprintf(stderr,"Unrecognized voxelset macro. Expected NUMxNUM or NUMxNUMxNUM\n");
				exit(-1);
			}
			zd=1;			
		}
		for (int x=0;x<xd;x++)
			for (int y=0;y<yd;y++)				
				for (int z=0;z<zd;z++)				
					addvoxel(vs,x,y,z,-1);    	
		return -1;
	}

	FILE *f = fopen(fn,"r");
	if (!f)
	{
		fprintf(stderr,"No such file <%s>\n",fn);
		exit(-1);
	}
	char buf[BUFS-1];
	char *r;	
	int lncnt=0;
	
	while (!feof(f))
	{
		r = fgets(buf, BUFS-1, f);				
		lncnt++;		 
		if (r) 
        {   
        	
        	int x,y,z,c,co;        	
        	int sc = sscanf(r,"%d %d %d %d %d",&x,&y,&z,&c,&co);
        	if (sc<3)
        	{
        		sc=sscanf(r,"%d,%d,%d,%d,%d",&x,&y,&z,&c,&co);

        		if (sc<1)
    			{
    				if (!strlen(r) || (strlen(r)==1 && (r[strlen(r)-1]=='\n'))) 
    					continue; // true strip needed        			
    				// Ignore if x, y, z  are present in the line
    				if (!(strchr(r,'x') && strchr(r,'y')) || lncnt!=1)
        				fprintf(stderr,"Warning: incomplete line in input file <%s>", r);
        			continue;
    			}
    		}
    		if (sc==2) { z=0; c=-1; }    		// allow 2d data
    		if (sc==3) c=-1;
    		addvoxel(vs,x,y,z,c);    		    	
        }
        else
        { 	
        	if (feof(f)) break;        	
			fprintf(stderr,"Read error in line %d: <%s>\n",lncnt,fn);
			exit(-1);
		}  
	}

	fclose(f);
    
	
}

int findkval(char *fn)
{
	// try to find kval in filename
	char *p = strstr(fn,"kval");
	int kv;
	if (p)
	{
		if (sscanf(p+4,"%d",&kv))
			return kv;	
	}

	p=(char*)fn;
	// try k
	while ((p = strstr(p,"k"))!=NULL)
	{
	
		if (sscanf(p+1,"%d",&kv))
			return kv;		
		p++;

	}
	
	return -1;
}



char *preparecnf(config *cnf, char *continuedir, char *inputfile)
{
  parameters *ps = cnf->parsets;
  for (int i=0; i<cnf->parsetlen; i++, ps++)
  {
  	 if (ps->timelimit_T>0 && !ps->runsset) 
		ps->runs_r=-1; // unlimited runs when time is set 

	if (cnf->globaltimelimit<0 && ps->resampleruns_E<0)
	{
		fprintf(stderr, "Set time limit either global (-T) or in PS:%s if (E) is negative.\n",ps->name);
	  	exit(-1);
	}
  }
    
  cnf->timetest=0;

  if (verbose_writeoutputfiles_Q)
  	greedyloginit(cnf->logfile);

  cnf->inputfile = NULL;
  if (continuedir)
  {
  	if (!strcmp(continuedir,"-"))
  	{
  		if (cnf->projectname) continuedir = cnf->projectname;
  		else if (inputfile) 
  		{
  			continuedir = (char*)strdup(basename(inputfile));
  			char *dot = strrchr(continuedir,'.');
			if (dot) dot[0]=0;       		       	       	   	
  		}
  	}

  	int score=0; // ignore
  	cnf->inputfile = locateminscorefile(continuedir,&score,0);
  	if (!cnf->inputfile)  	
  		fprintf(stderr, "Warning: no minscore file found in %s\n.", continuedir);  		

  	if (!cnf->projectname)
  	{
  		cnf->projectname = continuedir;
  	}
  }

  if (!cnf->inputfile) cnf->inputfile = inputfile; 
  // if no minscore located

  if (!cnf->projectname)
  {					
 	cnf->projectname = (char*)strdup(basename(cnf->inputfile));
 	char *dot = strrchr(cnf->projectname,'.');
 	if (dot) dot[0]=0;       		       	       	    
  }

  return cnf->inputfile;

}


// t --> minscoreseparator
// Save SP_SEPARATOR ids to minscoreseparator
void cidseparator2minscore(component *comp)
{

	graph *g = comp->g;
	int cid = comp->cid;

	// printf("Store minscore: %d cid=%d miscore_cid_g=%d\n",
	// 	comp->tscore,comp->cid,g->comp[cid]->minscore);

	if (comp->validminscoredata) return; // no need to store
		
	int ps=0;

	for (int i = 1; i <= comp->len; ++i)
		if (comp->status[i] & SP_SEPARATOR)
			comp->minscoreseparator[ps++]=i;

	// update graph score if better score obtained
	

	if (g->comp[cid] == comp)
	{
		// the same 
		comp->g->minscore += (ps-comp->minscore);		
	}
	else 
	{
		// printf("Store minscore: %d cid=%d miscore_cid_g=%d\n",
		// 	comp->cid,
		// 	comp->tscore,
		// 	g->comp[cid]->minscore);

		if (g->comp[cid]->minscore > comp->tscore)
		{
			comp->g->minscore += (ps - g->comp[cid]->minscore);
			g->comp[cid] = comp; // update min clustering comp.			
		}
	}
	
	// int old = comp->g->minscore;	
	comp->hasclustering = true;	
	comp->validminscoredata = true;
	comp->minscore = comp->tscore;

	int s=g->easyplus;
	for (int i=0; i<g->complen; i++)	
		s+=g->comp[i]->minscore;		
	
	if (s!=g->minscore)
	{
		fprintf(stderr,"[Aaargh] Sth is wrong with the graph minscore... %d %d\n",
			s,g->minscore);
		exit(-1);
	}

	if (ps!=comp->minscore)
	{
		fprintf(stderr,"[Aaargh] Sth is wrong with the separator... sepfound=%d minscore=%d\n",ps,comp->minscore);
		exit(-1);
	}		

#ifdef SCORE_CTRL_DEBUG
	ppscoreg(g);
#endif	
	
}




// Gen clusters id from comp.minscoreseparator's
// ids are 0(separator),1(first cluster),2,..,lastid
// returns lastid+1
int genclusterids(component *comp, config *cnf, int clusterid)
{		
	// assign separators

	
	voxel **v = comp->v;
	for (int j = 1; j <= comp->len; ++j)	 // improve it later						
		if (comp->status[j] & SP_SEPARATOR) 		
			v[j]->clusterid=0;					
		else
			v[j]->clusterid=-1;					

	voxelid border[cnf->optk*NEIGHBOURHOODSIZE+1]; 
	int bofirst,bolast;
	for (int j = 1; j <= comp->len; ++j)	 // improve it later		
	{		
		if (v[j]->clusterid==-1)
		{			
			// new cluster							
			v[j]->clusterid = -2; // visited
			border[0] = j;
			bofirst=0;
			bolast=1;
			clusterid++; 

			int clustersize = 0;

			while (bolast>bofirst) // looping
			{
				int cur = border[bofirst++];
				if (v[cur]->clusterid!=-2) continue; 
				
				NEIGHBOURSLOOP2(cur)
				{									
					voxelid nbid = *_a;       				
					if (v[nbid]->clusterid!=-1) continue;
					v[nbid]->clusterid = -2; // visited

					if (bolast==cnf->optk*NEIGHBOURHOODSIZE)
					{
						// Too many neighbours...
						fprintf(stderr, "Too many neighbours detected. Cluster size >k?\n");
						exit(-1);			
					}
					border[bolast++] = nbid;
				}	

				v[cur]->clusterid = clusterid; 
				// printf("cur=%d xy=(%d,%d) clusterid=%d stat=%d\n",cur,v[cur]->x,v[cur]->y,clusterid,comp->status[cur]);
				clustersize++;
			}

			if (clustersize>cnf->optk)
			{
				fprintf(stderr, "Incorrect size of a cluster detected %d>k\n",clustersize);												
				greedylog("Incorrect size of a cluster detected %d>k\n",clustersize);				

				// ppspace(comp);

				exit(-1);
			}
		}
	}
	return clusterid+1;
}

// // minscoreseparator -> t
int genminscoreclusteringcomp(component *comp)
{
	int sep = 0;

	if (comp->validminscoredata) return comp->minscore; 

	// clean
	for (int i = 1; i <= comp->len; ++i)
		comp->status[i] = SP_CLUSTER|SP_VOXEL;

	// set
	for (int k = 0; k < comp->minscore; ++k)			
	{	
		comp->status[comp->minscoreseparator[k]] = SP_SEPARATOR|SP_VOXEL;
		sep++;
	}		

	comp->validminscoredata = 1;

	return sep;
}

// minscoreseparator -> t (whole graph)
int genminscoreclustering(graph *g)
{
	
	// for (int i = 1; i <= g->vlen; ++i)
	// 	STATUS(i) = SP_CLUSTER|SP_VOXEL;	

	int sep=0;
	for (int cid = 0; cid < g->complen; ++cid)			
		sep += genminscoreclusteringcomp(g->comp[cid]);

	return sep;		
}


char *exts[2]={".log",".tsv"};

char *getuniquename(char *b, char *prjname, int score, char *suffix, int *renamed)
{
	struct stat s;   
	int k=0;
	int e;
	do
	{	e=0;
		for (int i = 0; i < 2; ++i)
		{
			if (k==0) sprintf(b,"%s/%d%s%s",prjname,score,suffix,exts[i]);
			else sprintf(b,"%s/%d%s.%d%s",prjname,score,suffix,k,exts[i]);
			if (stat(b,&s)==0) { e++; *renamed=1; }
		}			
		k++;	
	} while (e);
  	return b;
}


// Gen clusters id from comp.minscoreseparator's
// ids are 0(separator),1(first cluster),2,..,lastid
// returns lastid+1
int regularitychecker(graph *g, config *cnf)
{
	// genminscoreclustering(g);	
	// int clusterslen = genclusterids(g,cnf);
	// voxelid borderbuf[cnf->optk*NEIGHBOURHOODSIZE+1]; 
	// int borderbuflen=0;

	// voxelid clusters[clusterslen][cnf->optk];
	// voxelid cluster2size[clusterslen];
	// voxelid *borders[clusterslen];

	// for (int i = 0; i < clusterslen; ++i) cluster2size[i]=0;
	// for (int j = 1; j <= g->vlen; ++j)	 
	// {	
	// 	voxelid clu = g->v[j].clusterid;
	// 	if (clu>0)
	// 		clusters[clu][cluster2size[clu]++]=j;		
	// }

	// for (int i = 1; i < clusterslen; ++i) 
	// {
		
	// 	borderbuflen=0;
	// 	for (int j = 0; j < cluster2size[i]; ++j)	 
	// 	{
	// 		for (voxelid *_a = g->ngh[clusters[i][j]]; *_a; *_a++)			
	// 		{
 //    			voxelid nbid = *_a;   

	// 			if (g->v[nbid].clusterid==0) // 0, -3, or current cluster
 //    			{
 //    				borderbuf[borderbuflen++]=nbid;
 //    				g->v[nbid].clusterid=-3;
 //    			}    		
	// 		}
	// 	}
	// 	borders[i]=(voxelid*)malloc(sizeof(voxelid)*(borderbuflen+1));
	// 	memcpy ( borders[i], borderbuf, sizeof(voxelid)*borderbuflen);
	// 	borders[i][borderbuflen]=0;
	// 	// clean separator
	// 	for (int k = 0; k < borderbuflen; ++k)		
	// 		g->v[borderbuf[k]].clusterid=0;					
	// }

	// int countslen=max(MAXX,MAXZ);
	// countslen=max(countslen,MAXZ)+1;

	// voxel counts[countslen];
	// for (int i = 1; i < clusterslen; ++i) 
	// {		

	// 	for (int j = 0; j < countslen; ++j) 
	// 	{	
	// 		counts[j].x=0;
	// 		counts[j].y=0;
	// 		counts[j].z=0;
	// 	}

	// 	int k;
	// 	for (k=0; borders[i][k]!=0; k++)
	// 	{
	// 		counts[g->v[borders[i][k]].x].x++;
	// 		counts[g->v[borders[i][k]].y].y++;
	// 		counts[g->v[borders[i][k]].z].z++;
	// 	}

	// 	int cx=0,cy=0,cz=0;
	// 	for (int j = 0; j < countslen; ++j) 
	// 	{	
	// 		if (counts[j].x) cx++; 
	// 		if (counts[j].y) cy++; 
	// 		if (counts[j].z) cz++; 			
	// 	}

	// 	printf("Cl:%d s=%d %d.%d.%d\n",i,k,cx,cy,cz);

	// }
}


// check conditions
bool satifiedcondition(condition *c, int variable)
{	
	switch (c->op)	
	{
		case OEq: return variable == c->val;
		case ONe: return variable != c->val;
		case OLt: return variable < c->val;
		case OLe: return variable <= c->val;
		case OGt: return variable > c->val;
		case OGe: return variable >= c->val;
		default: 
			fprintf(stderr,"Unknown operator %d?",c->op);
			exit(-1);			
	}
}

// check conditions
bool satifiedconditionf(condition *c, double variable)
{	
	switch (c->op)	
	{
		case OEq: return variable == c->valf;
		case ONe: return variable != c->valf;
		case OLt: return variable < c->valf;
		case OLe: return variable <= c->valf;
		case OGt: return variable > c->valf;
		case OGe: return variable >= c->valf;
		default: 
			fprintf(stderr,"Unknown operator %d?",c->op);
			exit(-1);			
	}
}


// test if PS is valid
bool validPS(component *comp, config *cnf, parameters *ps)
{	
	if (cnf->onecomponentoonly>=0 && cnf->onecomponentoonly!=comp->cid) return false;	
	
	if (1.0*ps->resamplelocalthreshold_q*comp->len<cnf->optk) return false; 
		// q to restrictive for the component
	
	// conditions
	condition *cnd = ps->conditions;
	while (cnd)
	{		
		switch (cnd->type)
		{
			case CND_KVAL: 
				if (!satifiedcondition(cnd,cnf->optk)) return false;	
				break;

			case CND_SIZE:
				if (!satifiedcondition(cnd,comp->len)) return false;	
				break;

			case CND_K2SIZERATIO:
				if (!satifiedconditionf(cnd,1.0*cnf->optk/comp->len)) 
					return false;	
				break;
			default:
				fprintf(stderr,"Incorrect cond type %d\n",cnd->type);
				exit(-1);
		}		
		cnd=cnd->next;
	}
	return true;
}

// Assign PSs to components depending on the size and kval
int initializecomponentPSs(component *comp, config *cnf)
{
	comp->parsets = (parameters*)malloc(sizeof(parameters)*cnf->parsetlen);
	comp->parsetlen = 0;
	bool hasinitial = false;
	for (int i = 1; i < cnf->parsetlen; ++i)	
	{				
		parameters *ps = cnf->parsets+i;
		if (validPS(comp,cnf,cnf->parsets+i))
		{
			parameters *p = cnf->parsets+i;
			if (p->initial)
			{	
				if (hasinitial) continue; // ignore other initials
				hasinitial = true; // take first initial
				comp->initialps = *p;				
			}
			else
				comp->parsets[comp->parsetlen++]=cnf->parsets[i]; // copy			
		}
	}

	if (!hasinitial)
	{
		// no initial; make from default	
		initparams(&comp->initialps,cnf->parsets);				
	}
	comp->initialps.comp = comp;     // back ref.


	if (!comp->parsetlen)
	{
		// no PS; initialize from default
		initparams(comp->parsets,cnf->parsets);				
		comp->parsetlen=1;
	}

	for (int i = 0; i < comp->parsetlen; ++i)	
	{
		comp->parsets[i].comp = comp;     // back ref.
		comp->parsets[i].fitness = 1.0;   // fitness
	}


	return comp->parsetlen;

	// printf("\n========= %d ======",comp->cid);
	// for (int i = 0; i < comp->parsetlen; ++i)	
	// {
	// 	ppparameters(cnf,comp->parsets+i);
	// 	printf("\n");
	// }
	// printf("===============\n");
}