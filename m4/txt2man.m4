# Function to check for txt2man script

AC_DEFUN([PROG_TXT2MAN], [
	AC_ARG_WITH([txt2man],
		AC_HELP_STRING([--with-txt2man=path], [Use txt2man found in path]))

	if test "x$with_txt2man" != "x"; then
		TXT2MAN="$with_txt2man"
	else
		AC_CHECK_PROGS([TXT2MAN], [txt2man], [no])
	fi
	
	if test "x$TXT2MAN" != "xno"; then
		foo=bar; dnl to avoid errors if $2 is unset
		$1
	else
		foo=bar; dnl to avoid errors if $2 is unset
		$2
	fi
	
	AC_SUBST([TXT2MAN])
])
