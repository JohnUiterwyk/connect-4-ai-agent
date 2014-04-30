#!/bin/sh
# Runs a series of Connect 4 games to test the strength of an agent against the included sample agents


if [ ! -f "$1" ]; then
	/bin/echo "$0 <agent> [rounds]"
	/bin/echo "eg $0 agent_`whoami`.c 20"
	exit
fi

iname=`echo $1 | sed -e 's/agent_\(.*\)\.c$/\1/'`


ROUNDS=`[ -n "$2" ] && echo "$2" || echo 50`;
OPPONENTS="random simple medium"
make clean; make
	
echo " " >> $iname.out
echo "***************** Sample game against simple *****************" >> $iname.out
./c4 $iname simple --fast >> $iname.out

echo " " >> $iname.out
echo "***************** Game results *****************" >> $iname.out

for OPPONENT in $OPPONENTS  
do
	rm -f gameresults.txt
	ROUND=0
	
	while [ $ROUND -lt `expr $ROUNDS / 2` ] 
	do
		./c4 $iname $OPPONENT --fast | tail -1 >> gameresults.txt
		./c4 $OPPONENT $iname --fast | tail -1 >> gameresults.txt

		ROUND=`expr $ROUND + 1`
	done

	PLAYER_WINS=`grep $iname gameresults.txt | wc -l `
	PLAYER_WINS=`expr $PLAYER_WINS`
	OPPONENT_WINS=`grep $OPPONENT gameresults.txt | wc -l `
	OPPONENT_WINS=`expr $OPPONENT_WINS`

	DRAWS=`expr $ROUNDS - $PLAYER_WINS - $OPPONENT_WINS`

	echo " " >> $iname.out
	echo "Results of $iname vs. $OPPONENT ($ROUNDS rounds):" >> $iname.out
	echo " " >> $iname.out

	[ $DRAWS -eq 0 ] && echo "$iname: $PLAYER_WINS   $OPPONENT: $OPPONENT_WINS" >> $iname.out  
	[ $DRAWS -eq 1 ] && echo "$iname: $PLAYER_WINS   $OPPONENT: $OPPONENT_WINS   (1 tie)" >> $iname.out  
	[ $DRAWS -gt 1 ] && echo "$iname: $PLAYER_WINS   $OPPONENT: $OPPONENT_WINS   ($DRAWS ties)" >> $iname.out  

	[ $PLAYER_WINS -gt $OPPONENT_WINS ] && echo "   *** $iname beat $OPPONENT ***" >> $iname.out
done

cat $iname.out

