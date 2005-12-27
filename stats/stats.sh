# Count total requests
TOTAL_REQS=`wc -l count.txt | awk '{ print($1) }'`
# Count single hosts
SINGLE_HOSTS=`cat count.txt | awk '{ print($2) }' | sort -u | wc -l` 

echo `date`
echo "Total requests: $TOTAL_REQS"
echo "Single hosts: $SINGLE_HOSTS"


