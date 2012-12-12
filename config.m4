PHP_ARG_ENABLE(AOP, whether to enable AOP support,
[ --enable-AOP   Enable AOP support])

    AC_MSG_CHECKING([for PCRE headers location])
    for i in  /usr/include /usr/include/pcre /usr/local/include; do
      test -f $i/pcre.h && PCRE_INCDIR=$i
    done

    if test -z "$PCRE_INCDIR"; then
      AC_MSG_ERROR([Could not find pcre.h in $PHP_PCRE_REGEX])
    fi
    AC_MSG_RESULT([$PCRE_INCDIR])

if test "$PHP_AOP" = "yes"; then
  AC_DEFINE(HAVE_AOP, 1, [aop])
  PHP_NEW_EXTENSION(aop, aop.c Lexer.c aop_joinpoint.c, $ext_shared)
fi
