# Makefile for 

CC = g++
DEBUG = -g
INCLUDE = -I $(PWD)
PROFILE =

CFLAGS = -Wall -c $(DEBUG) $(INCLUDE) $(PROFILE)
LFLAGS = -Wall $(DEBUG) $(PROFILE) -L$(PWD)/lib

OTHERLIBS += -lgsl -lgslcblas -lfftw3
# -lboost_system -lboost_thread
MYLIBS = -lpricing -lmathutil -lcsutil -lmathutil 
# todo: sort out the dependencies btw csutil and mathutil

MYLIBDIR = lib
OBJDIR = objectfiles
BINDIR = bin

vpath %.cpp csutils:mathutils:pricing:main
#vpath %.o $(OBJDIR)

MATHUTIL_OBJS = gauss_legendre.o \
	gamma_impl.o \
	levinu_acc.o \
	interpolate.o \
	fft_impl.o \
	summers.o \
	tuple.o

CSUTIL_OBJS = utils.o \
	get_opt.o \
	configfile.o \
	sequtils.o \
	key_value_table.o \
	systemprog.o \
	logger.o \
	prog_init.o

PRICING_OBJS = agpdata.o \
	americanput.o \
	asiangammapricer.o \
	basketpricing.o \
	eurooption.o \
	fteurocalc.o \
	parity.o \
	spreadpricing.o \
	basket.o

FT_OBJS = main.o
FFT_OBJS = fft_test.o
ASIAN_OBJS = asianmain.o
ASIANFFT_OBJS = asian_fft_main.o
SPREAD_OBJS = spreadmain.o
ACCELTEST_OBJS = accel_test.o
CVZTEST_OBJS = cvz_testmain.o
EULERTEST_OBJS = euler_test.o
AMERICANPUTTEST_OBJS = americanputmain.o
GLTEST_OBJS = gltest.o
AITKENTEST_OBJS = aitken_test.o
SEQUENCEUTILSTEST_OBJS = sequenceutilsmain.o
BASKETPRICINGTEST_OBJS = basketmain.o
BASKETPRICINGTEST2_OBJS = basketmain2.o
FINDPARAMS_OBJS = findparams.o
TUPLETEST_OBJS = tupletest.o
BASKET_CMP_OBJS = basket_cmp.o
FT_BASKET_CMP_OBJS = ft_basket_cmp.o

ftOBJS = $(patsubst %, $(OBJDIR)/%, $(FT_OBJS))
fft_testOBJS = $(patsubst %, $(OBJDIR)/%, $(FFT_OBJS))
asian_testOBJS = $(patsubst %, $(OBJDIR)/%, $(ASIAN_OBJS))
asian_fft_testOBJS = $(patsubst %, $(OBJDIR)/%, $(ASIANFFT_OBJS))
spread_testOBJS = $(patsubst %, $(OBJDIR)/%, $(SPREAD_OBJS))
accel_testOBJS = $(patsubst %, $(OBJDIR)/%, $(ACCELTEST_OBJS))
cvz_testOBJS = $(patsubst %, $(OBJDIR)/%, $(CVZTEST_OBJS))
euler_testOBJS = $(patsubst %, $(OBJDIR)/%, $(EULERTEST_OBJS))
americanput_testOBJS = $(patsubst %, $(OBJDIR)/%, $(AMERICANPUTTEST_OBJS))
gl_testOBJS = $(patsubst %, $(OBJDIR)/%, $(GLTEST_OBJS))
aitken_testOBJS = $(patsubst %, $(OBJDIR)/%, $(AITKENTEST_OBJS))
sequenceutils_testOBJS = $(patsubst %, $(OBJDIR)/%, $(SEQUENCEUTILSTEST_OBJS))
basketpricing_testOBJS = $(patsubst %, $(OBJDIR)/%, $(BASKETPRICINGTEST_OBJS))
basketpricing_test2OBJS = $(patsubst %, $(OBJDIR)/%, $(BASKETPRICINGTEST2_OBJS))
findparamsOBJS = $(patsubst %, $(OBJDIR)/%, $(FINDPARAMS_OBJS))
tupletestOBJS = $(patsubst %, $(OBJDIR)/%, $(TUPLETEST_OBJS))
basket_cmpOBJS = $(patsubst %, $(OBJDIR)/%, $(BASKET_CMP_OBJS))
ft_basket_cmpOBJS = $(patsubst %, $(OBJDIR)/%, $(FT_BASKET_CMP_OBJS))

TARGETS = asian_test \
	fft_test \
	ft \
	spread_test \
	asian_fft_test \
	accel_test \
	cvz_test \
	euler_test \
	americanput_test \
	gl_test \
	aitken_test \
	sequenceutils_test \
	basketpricing_test \
	basketpricing_test2 \
	findparams \
	tupletest \
	basket_cmp \
	ft_basket_cmp

libmathutilOBJS = $(patsubst %, $(OBJDIR)/%, $(MATHUTIL_OBJS))
libcsutilOBJS = $(patsubst %, $(OBJDIR)/%, $(CSUTIL_OBJS))
libpricingOBJS = $(patsubst %, $(OBJDIR)/%, $(PRICING_OBJS))

LIBTARGETS = libmathutil libcsutil libpricing

.PHONY: clean all libs
all : libs $(TARGETS)
libs: $(LIBTARGETS)
clean :
	rm -f \
	$(foreach target,$(TARGETS),$(BINDIR)/$(target)) \
	$(OBJDIR)/*.o \
	$(foreach lib,$(LIBTARGETS),$(MYLIBDIR)/$(lib).so) \
	$(foreach lib,$(LIBTARGETS),$(MYLIBDIR)/$(lib).a)

define get_object_files
$(1): $$($(1)OBJS)
endef

$(foreach prog,$(TARGETS),$(eval $(call get_object_files,$(prog))))
$(TARGETS):
	$(CC) $(LFLAGS) $^ $(MYLIBS) $(OTHERLIBS) -o $(BINDIR)/$@

$(foreach lib,$(LIBTARGETS),$(eval $(call get_object_files,$(lib))))
$(LIBTARGETS):
	ar rcs $(MYLIBDIR)/$@.a $^

$(OBJDIR)/%.o : %.cpp
	$(CC) $(CFLAGS) $< -o $@

help :
	@echo $(TARGETS)
	@echo all libs clean

