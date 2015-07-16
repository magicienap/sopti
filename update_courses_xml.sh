SOPTI=./build/src/sopti
COURSEFILE=data/courses.csv
CLOSEDFILE=data/closed.csv
OUTPUTDIR=data/courses_xml

mkdir -p $OUTPUTDIR

$SOPTI listcourses > courses.txt
echo "Got" `wc -l courses.txt` "courses"

for course in `cat courses.txt`;
do
	echo $course
	wget --mirror "http://www.polymtl.ca/etudes/cours/utils/ficheXML.php?sigle=$course" -O "$OUTPUTDIR/$course.xml";
done
