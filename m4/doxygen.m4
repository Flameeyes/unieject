# Function to check for Doxygen and enable API documentation building
# First parameter: the path of the Doxyfile (where Doxyfile.in is)

AC_DEFUN([_DOXYFORMAT], [
	AC_MSG_CHECKING([whether to build API doc in $1 format])
	if grep -w $1 >/dev/null <<<$DOXYFORMATS; then
		AC_MSG_RESULT([yes])
		AC_SUBST(DOXYFORMAT_$1, [YES])
	else
		AC_MSG_RESULT([no])
		AC_SUBST(DOXYFORMAT_$1, [NO])
	fi
])

AC_DEFUN([DOXYGEN_DOC], [
	AC_ARG_ENABLE([doc], [AC_HELP_STRING([--enable-doc],
		[Enable API documentation building in Doxygen])])
	AC_ARG_WITH([docpath], [AC_HELP_STRING([--with-docpath],
		[Where to install documentation to (default to /usr/share/doc/$PACKAGE_NAME)])])
	AC_ARG_VAR([DOXYFORMATS], [Formats to create the doxygen doc in, selected from html (default) htmlhelp latex rtf man xml (more than one is fine)])
	
	if test "x$enable_doc" != "xno"; then
		AC_CHECK_PROGS([DOXYGEN], [doxygen], [no])
		if test "x$DOXYGEN" = "xno"; then
			AC_MSG_ERROR([Doxygen not found but documentation requested])
		fi
		
		AC_CONFIG_FILES([$1])
		
		if test "x$DOXYFORMATS" = "x"; then
			DOXYFORMATS="html"
		fi
		
		_DOXYFORMAT(html)
		_DOXYFORMAT(htmlhelp)
		_DOXYFORMAT(latex)
		_DOXYFORMAT(rtf)
		_DOXYFORMAT(man)
		_DOXYFORMAT(xml)
	fi
])

