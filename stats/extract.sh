rm -f plot_reqs_per_hour.txt
rm -f plot_hosts_per_hour.txt

for day in `seq 15 31`;
do
	for hour in `seq --equal-width 0 23`;
	do
		# Extract total requests
		echo "2004/12/"$day $hour `cat count.txt | grep 2004/12/$day-$hour | wc -l` >> plot_reqs_per_hour.txt
		# Extract host count
		echo "2004/12/"$day $hour `cat count.txt | grep 2004/12/$day-$hour | awk '{ print($2) }' | sort -u | wc -l` >> plot_hosts_per_hour.txt
	done
done
 
