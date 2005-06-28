if NEED_GETOPT_LONG
sopti_compat = getopt.c getopt1.c
else
sopti_compat = 
endif

bin_PROGRAMS = sopti
sopti_SOURCES = error.cpp group.cpp main.cpp make_message.c schoolcourse.cpp schoolschedule.cpp studentschedule.cpp stdsched.cpp read_csv.cpp $(sopti_compat)
EXTRA_sopti_SOURCES = getopt.c getopt1.c
EXTRA_DIST = legende_cours_poly.txt data/update web/*.php web/config.php.example web/*.css web/*.gif db/updatedb.pl doc/*.pdf
# noinst_SCRIPTS = data/update

noinst_HEADERS = constraint.hpp group_constraint.hpp globals.hpp group.hpp objective.hpp period.hpp schoolcourse.hpp schoolschedule.hpp studentschedule.hpp getopt.h make_message.h stdsched.h read_csv.hpp main.hpp error.hpp
