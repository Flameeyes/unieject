dnl Check for C99 support
dnl First check if -std=c99 is supported, then try to build C99 code with it;
dnl if it fails, it means that C99 can't be built.

AC_DEFUN([LANG_C99], [
	AC_CACHE_CHECK([if compiler supports -std=c99 flag],
		[c99_cv_stdc99_flag],
		[
		save_CFLAGS=$CFLAGS
		CFLAGS="$CFLAGS -std=c99"
		AC_COMPILE_IFELSE([int a;],
			[c99_cv_stdc99_flag=yes],
			[c99_cv_stdc99_flag=no])
		CFLAGS="$save_CFLAGS"
		])
	
	if test "x$c99_cv_stdc99_flag" = "xyes"; then
		C99FLAGS="-std=c99"
	fi
	
	AC_SUBST(C99FLAGS)
	
	AC_CACHE_CHECK([if compiler can build C99 code],
		[c99_cv_support],
		[
		save_CFLAGS=$CFLAGS
		CFLAGS="$CFLAGS $C99FLAGS"
		AC_COMPILE_IFELSE(
			[
			void func()
			{
			  int j = 0;
			  for(int i = 0; i < 10; i++)
			    j = i+1;
			  for(int i = 0; i < 10; i++)
			    j = i+1;
			}
			],
			[c99_cv_support=yes],
			[c99_cv_support=no])
		CFLAGS="$save_CFLAGS"
		])

	if test "x$c99_cv_support" == "xno"; then
		true
		$2
	else
		true
		$1
	fi
])
