AC_INIT(sopti, 0.14)
dnl AC_CONFIG_HEADERS(config.h)
AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE(sopti, 0.14)

AC_PROG_CC

AC_PROG_CXX
AC_STDC_HEADERS

AC_CHECK_FUNCS(getopt_long)
AC_CHECK_LIB(mysqlclient, mysql_real_connect)
AM_CONDITIONAL(NEED_GETOPT_LONG, test $ac_cv_func_getopt_long = no)

AC_OUTPUT([Makefile])
