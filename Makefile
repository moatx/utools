PROG=	power
WARNS= 6
MAN=
LDADD+= -lprop
DPADD+= ${LIBPROP} 


.include <bsd.prog.mk>
