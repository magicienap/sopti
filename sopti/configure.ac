AC_INIT(sopti, 0.12)
dnl AC_CONFIG_HEADERS(config.h)
AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE(sopti, 0.12)

AC_PROG_CC

AC_PROG_CXX
AC_STDC_HEADERS

AC_CHECK_FUNCS(getopt_long)
AM_CONDITIONAL(NEED_GETOPT_LONG, test $ac_cv_func_getopt_long = no)

# Need libxml
AC_PATH_PROG(XML_CONFIG, xml2-config, no)
AC_MSG_CHECKING(for libxml2)
if test "$XML_CONFIG" = "no" ; then
  AC_MSG_RESULT(no)
  echo "*** Could not find libxml2. Check out the libxml homepage at "
  echo "*** http://rufus.w3.org/veillard/XML/xml.html or just get "
  echo "*** the appropriate package for your system. If you have Debian,
"
  echo "*** get libxml2-dev"
  AC_MSG_ERROR([libxml2 needed])
else
  AC_MSG_RESULT(yes)

  # Check for proper version
  AC_MSG_CHECKING(for version 2.0.0 (or better) of libxml)

  # Check if old or new style xml-config
  libxml_version=`$XML_CONFIG --version`
  libxml_version=`echo $libxml_version | sed 's/ //'`
  if test `expr $libxml_version : libxml` != 0; then

    xml_config_major_version=`$XML_CONFIG --version | \
      sed 's/libxml \([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    xml_config_minor_version=`$XML_CONFIG --version | \
      sed 's/libxml \([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    xml_config_micro_version=`$XML_CONFIG --version | \
      sed 's/libxml \([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`

  else

    xml_config_major_version=`$XML_CONFIG --version | \
      sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    xml_config_minor_version=`$XML_CONFIG --version | \
      sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    xml_config_micro_version=`$XML_CONFIG --version | \
      sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`

  fi

  # Check if version is good
  #if test \( $xml_config_major_version -ne 1 \) -o \
  #        \( $xml_config_minor_version -ne 8 \) -o \
  #        \( $xml_config_micro_version -lt 2 \) ; then

  if test \( $xml_config_major_version -lt 2 \) ; then
     AC_MSG_RESULT(no)
     echo "*** You need libxml 2.0.0 or greater. Check out the libxml "
     echo "*** homepage at http://rufus.w3.org/veillard/XML/xml.html or just get "
     echo "*** the appropriate package for your system. If you have Debian, "
     echo "*** get the latest libxml-dev"
     AC_MSG_ERROR([libxml 2.0.0 or greater needed])

  else
     AC_MSG_RESULT(yes)
  fi

  XML_LIBS=`$XML_CONFIG --libs`
  XML_CFLAGS=`$XML_CONFIG --cflags`
  LIBS="$LIBS $XML_LIBS"
  CFLAGS="$CFLAGS $XML_CFLAGS"
  CXXFLAGS="$CXXFLAGS $XML_CFLAGS"
fi 

AC_OUTPUT([Makefile])
