RAW_HOSTS=`cat count.txt | awk '{ print($2) }'`
echo "Raw Hosts: $RAW_HOSTS"
SINGLE_HOSTS=`echo "$RAW_HOSTS" | sort -u`
echo "Single Hosts: $SINGLE_HOSTS"
HOST_COUNTS=""

for HOST in $SINGLE_HOSTS;
do
	echo "Processing host $HOST"
	MODIF_HOST=`echo $HOST | sed 's/\./\\\\./g'`
	echo "Modif Host: $MODIF_HOST"
	COUNT="`echo $RAW_HOSTS | grep $MODIF_HOST | wc -l`"
	echo "Host $HOST has $COUNT occurences"
	HOST_COUNTS="$HOST_COUNTS $COUNT $HOST\n"
done

echo -e $HOST_COUNTS