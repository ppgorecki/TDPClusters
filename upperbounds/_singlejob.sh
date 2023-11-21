
set $*

id=$1
kval=$2
size=$3
basenamedir=$4	
file=$5
cube=$6 

echo $* 

[[ "$cubetestonly" ]] && [[ $cube = 0 ]] && exit 0

lbn=$tmplogs/$basenamedir.$$.$id
mkdir -p $basenamedir

[[ -f $stopfile ]] && exit -1 

if [[ $CONTINUE ]] && [[ -f stats/$id.tsv ]] 
then
	echo $id. $basenamedir from $( basename  $PWD ) already processed 
else

	# attach stoponscore in case of cube data

	if [[ $cube = 1 ]]
	then
		SOPT=$( echo $basenamedir | sed -n "s/.*opt\([0-9]*\).*/\1/p" )
		SOPT="-s$SOPT"
	fi

	echo -e "$RED" "New job: $( basename $FGREEDY ) $file -x $CONFIG $GREEDYOPT -b stats/$id.tsv -p$basenamedir -C$basenamedir -t $lbn.tsv.tmp -l $lbn.log.tmp -vtTxp$fgreedy_addverbose $SOPT $NC"

	if ! $FGREEDY $file -x $CONFIG $GREEDYOPT -b stats/$id.tsv -p$basenamedir -C$basenamedir -t $lbn.tsv.tmp -l $lbn.log.tmp -vtTxp$fgreedy_addverbose $SOPT 2> $lbn.log.err
	then
		[[ -f $stopfile ]] && exit -1 

		touch $stopfile
	    echo "fgreedy error!" 
	    echo "cd $PWD " >> $stopfile
	    cat $lbn.log.tmp >> $stopfile    
	    cat $lbn.log.err >> $stopfile 
	    cat $lbn.log.err
	    echo
	  exit  -2
	fi
fi

set $( cat stats/$id.tsv )
if [[ $# == 14 ]]; then echo -n -e $RED; else echo -n -e $LBLUE; fi		
cat stats/$id.tsv

echo -n -e $NC

