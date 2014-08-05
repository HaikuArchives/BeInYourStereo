dnl $Id: config.m4,v 1.2 2001-11-08 22:43:58 svanegmond Exp $
dnl config.m4 for extension biys


AC_DEFUN(BIYS_INC_CHK,[if test -r $i$1/HttpUI.h; then BIYS_DIR=$i; BIYS_INCDIR=$i$1])

PHP_ARG_WITH(biys, for biys support,
[  --with-biys[=DIR]             Include biys support: Set DIR to the directory
                                 with the Be In Your Stereo source.])


if test "$PHP_BIYS" != "no"; then
  AC_EXPAND_PATH($PHP_BIYS, PHP_BIYS)
  for i in /boot/home/ /boot/home/Download $PHP_BIYS; do
    BIYS_INC_CHK()
    el[]BIYS_INC_CHK()
    el[]BIYS_INC_CHK()
    fi
  done

  if test -z "$BIYS_DIR"; then
    AC_MSG_RESULT(Cannot find HttpUI.h. Please specify the installation path of Be in your Stereo.)
  fi

  if test ! -x "/boot/beos"; then
  	AC_MSG_ERROR(You need the BeOS to build this.)
  fi

  BIYS_INCLUDE="-I$BIYS_INCDIR"

  PHP_EXTENSION(biys, $ext_shared)
  PHP_REQUIRE_CXX
fi

PHP_SUBST(BIYS_INCLUDE)
