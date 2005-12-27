rm -f plot_reqs_day.txt
rm -f plot_hosts_day.txt

SUM_REQUESTS=0
SUM_HOSTS=0

for day in `seq 15 31`;
do
	# Extract total requests
	REQUESTS=`cat count.txt | grep 2004/12/$day | wc -l`
	SUM_REQUESTS=$(($SUM_REQUESTS+$REQUESTS))
	echo "2004/12/"$day $REQUESTS $SUM_REQUESTS >> plot_reqs_day.txt

	# Extract host count
	HOSTS=`cat count.txt | grep 2004/12/$day | awk '{ print($2) }' | sort -u | wc -l`
	SUM_HOSTS=$(($SUM_HOSTS+$HOSTS))
	echo "2004/12/"$day $HOSTS $SUM_HOSTS >> plot_hosts_day.txt
done

for day in `seq -f %02g 1 24`;
do
        # Extract total requests
        REQUESTS=`cat count.txt | grep 2005/01/$day | wc -l`
        SUM_REQUESTS=$(($SUM_REQUESTS+$REQUESTS))
        echo "2005/01/"$day $REQUESTS $SUM_REQUESTS >> plot_reqs_day.txt

        # Extract host count
        HOSTS=`cat count.txt | grep 2005/01/$day | awk '{ print($2) }' | sort -u | wc -l`
        SUM_HOSTS=$(($SUM_HOSTS+$HOSTS))
        echo "2005/01/"$day $HOSTS $SUM_HOSTS >> plot_hosts_day.txt

done
 
