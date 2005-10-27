# Functions to check for attributes support in compiler

AC_DEFUN([CC_ATTRIBUTE_CONSTRUCTOR], [
	AC_MSG_CHECKING([if compiler supports __attribute__((constructor))])
	AC_COMPILE_IFELSE([
		void ctor() __attribute__((constructor));
		void ctor() { };
		], [
			AC_MSG_RESULT([yes])
			AC_DEFINE([SUPPORT_ATTRIBUTE_CONSTRUCTOR], 1, [Define this if the compiler supports the constructor attribute])
			$1
		], [
			AC_MSG_RESULT([no])
			$2
		])
])

AC_DEFUN([CC_ATTRIBUTE_FORMAT], [
	AC_MSG_CHECKING([if compiler supports __attribute__((format(printf, n, n)))])
	AC_COMPILE_IFELSE([
		void __attribute__((format(printf, 1, 2))) printflike(const char *fmt, ...) { }
		], [
			AC_MSG_RESULT([yes])
			AC_DEFINE([SUPPORT_ATTRIBUTE_FORMAT], 1, [Define this if the compiler supports the format attribute])
			$1
		], [
			AC_MSG_RESULT([no])
			$2
		])
])

AC_DEFUN([CC_ATTRIBUTE_INTERNAL], [
	AC_MSG_CHECKING([if compiler supports __attribute__((visibility("internal")))])
	AC_COMPILE_IFELSE([
		void __attribute__((visibility("internal"))) internal_function() { }
		], [
			AC_MSG_RESULT([yes])
			AC_DEFINE([SUPPORT_ATTRIBUTE_INTERNAL], 1, [Define this if the compiler supports the internal visibility attribute])
			$1
		], [
			AC_MSG_RESULT([no])
			$2
		])
])

AC_DEFUN([CC_ATTRIBUTE_NONNULL], [
	AC_MSG_CHECKING([if compiler supports __attribute__((nonnull()))])
	AC_COMPILE_IFELSE([
		void some_function(void *foo, void *bar) __attribute__((nonnull()));
		void some_function(void *foo, void *bar) { }
		], [
			AC_MSG_RESULT([yes])
			AC_DEFINE([SUPPORT_ATTRIBUTE_NONNULL], 1, [Define this if the compiler supports the nonnull attribute])
			$1
		], [
			AC_MSG_RESULT([no])
			$2
		])
])

AC_DEFUN([CC_FUNC_EXPECT], [
	AC_MSG_CHECKING([if compiler has __builtin_expect function])
	AC_COMPILE_IFELSE([
		int some_function()
		{
			int a = 3;
			return (int)__builtin_expect(a, 3);
		}
		], [
			AC_MSG_RESULT([yes])
			AC_DEFINE([SUPPORT__BUILTIN_EXPECT], 1, [Define this if the compiler supports __builtin_expect() function])
			$1
		], [
			AC_MSG_RESULT([no])
			$2
		])
])
