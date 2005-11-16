# Function to check for GNU sed command

AC_DEFUN([GNU_PROG_SED], [
	AC_CACHE_CHECK([for GNU sed],
		[gnu_cv_prog_sed],
		[gnu_cv_prog_sed=no
		 for cmd in sed gsed gnused; do
			if ${cmd} --version 2>&1 | grep "GNU sed" > /dev/null 2>&1; then
				gnu_cv_prog_sed=${cmd}
			fi
		 done
		])
	
	if test "x$gnu_cv_prog_sed" == "xno"; then
		true
		$2
	else
		true
		$1
	fi
])
