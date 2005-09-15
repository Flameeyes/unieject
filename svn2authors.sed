# Simple sed script to convert the name of the SVN committers to author
# names for ChangeLog.

# Leave flameeyes above flame to avoid double-expansion
s:\<flameeyes\>:Diego Pettenò <flameeyes@gentoo.org>:
s:\<flame\>:Diego Pettenò <flameeyes@gentoo.org>:
