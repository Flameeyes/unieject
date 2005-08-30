# Function to check for GNU sed command

AC_DEFUN([GNU_PROG_SED], [
	AC_MSG_CHECKING([for GNU sed])
	GSED=no
	for cmd in sed gsed gnused; do
		if ${cmd} --version 2>&1 | grep "GNU sed" > /dev/null 2>&1; then
			GSED=${cmd}
		fi
	done
	
	if test "x$GSED" == "xno"; then
		AC_MSG_RESULT([not found])
		$2
	else
		AC_MSG_RESULT([${GSED}])
		$1
	fi
])
