PHP_ARG_ENABLE(AOP, whether to enable AOP support,
[ --enable-AOP   Enable AOP support])

if test "$PHP_AOP" = "yes"; then
  AC_DEFINE(HAVE_AOP, 1, [AOP])
  PHP_NEW_EXTENSION(AOP, AOP.c, $ext_shared)
fi
