PROG=	utools
#WARNS= 6
LDADD+= -lprop
DPADD+= ${LIBPROP} 


.include <bsd.prog.mk>
