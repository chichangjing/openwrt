# TEMPLATE for
# Toolkit definitions
#
# <TEMPLATE:comments, toolkit location>

export TOOL=<TEMPLATE:toolkit family:builder,visual,gnu,diab>
export TOOLKIT_MAKE_RULES=<TEMPLATE: use ${TOOLKIT_MAKE_RULES}Defs.mk, ${TOOLKIT_MAKE_RULES}Targets.mk>

export AS=<TEMPLATE: assembler compiler>
export CC=<TEMPLATE: C/C++ command line compiler>
export AR=<TEMPLATE: archiver [library maker] program>
export LD=<TEMPLATE: linker>
export CPP=<TEMPLATE: cpp preprocessor>
export NM=

# optional:
export LDFLAGS="<TEMPLATE: linker flags>"
