COUNT_TXT="../../data/count.txt"

for year in $(seq 2005 2006); do
	for day in $(seq -w 6 16); do
		echo -n "$year/12/$day "
		grep "$year/12/$day" $COUNT_TXT | wc -l
	done
	echo ""
done
