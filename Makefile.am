if NEED_GETOPT_LONG
sopti_compat = getopt.c getopt1.c
else
sopti_compat = 
endif

bin_PROGRAMS = sopti
sopti_SOURCES = error.cpp group.cpp main.cpp make_message.c schoolcourse.cpp schoolschedule.cpp studentschedule.cpp stdsched.cpp $(sopti_compat)
EXTRA_sopti_SOURCES = getopt.c getopt1.c
EXTRA_DIST = legende_cours_poly.txt data/update web/*.php web/config.php.example web/*.css web/*.gif
# noinst_SCRIPTS = data/update

noinst_HEADERS = constraint.hpp globals.hpp group.hpp objective.hpp period.hpp schoolcourse.hpp schoolschedule.hpp studentschedule.hpp getopt.h make_message.h stdsched.h
