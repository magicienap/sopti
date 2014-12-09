if NEED_GETOPT_LONG
sopti_compat = getopt.c getopt1.c
else
sopti_compat = 
endif

bin_PROGRAMS = sopti
sopti_SOURCES = error.cpp group.cpp main.cpp make_message.c schoolcourse.cpp schoolschedule.cpp studentschedule.cpp read_csv.cpp dbloader.cpp configfile.cpp $(sopti_compat)
EXTRA_sopti_SOURCES = getopt.c getopt1.c
EXTRA_DIST = legende_cours_poly.txt data/update data/update_smb web/*.php web/config.php.example web/*.css web/*.gif web/*.jpg web/*.png db/updatedb.pl doc/*.pdf update_courses_xml.sh make_teachers_csv.sh sopti.conf.example emailer/configuration.py  emailer/emailer.conf.example  emailer/emailer.py  emailer/logging_emailer.py  emailer/md5crypt.py  emailer/singletonmixin.py parser_prof.cpp
# noinst_SCRIPTS = data/update
LDADD = /usr/lib/libmysqlclient.so.15

noinst_HEADERS = constraint.hpp group_constraint.hpp globals.hpp group.hpp objective.hpp period.hpp schoolcourse.hpp schoolschedule.hpp studentschedule.hpp getopt.h make_message.h read_csv.hpp main.hpp error.hpp dbloader.hpp configfile.hpp uepoch.h

parser_prof: parser_prof.cpp
	g++ -g -o parser_prof `xml2-config --cflags` `xml2-config --libs` $+

