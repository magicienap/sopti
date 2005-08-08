SOPTI=./sopti
COURSEFILE=data/courses.csv
CLOSEDFILE=data/closed.csv
OUTPUTDIR=data/courses_xml

$SOPTI listcourses > courses.txt
echo "Got" `wc -l courses.txt` "courses"

for course in `cat courses.txt`;
do
	echo $course
	wget --mirror "http://www.cours.polymtl.ca/cours/fiche_xml.php?sigle=$course" -O "$OUTPUTDIR/$course.xml";
done
