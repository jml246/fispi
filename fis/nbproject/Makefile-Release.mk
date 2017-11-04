#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc.exe
CCC=g++.exe
CXX=g++.exe
FC=gfortran
AS=as.exe

# Macros
CND_PLATFORM=Cygwin-Windows
CND_DLIB_EXT=dll
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/157685732/LED0_OFF.o \
	${OBJECTDIR}/_ext/157685732/LED3_ON.o \
	${OBJECTDIR}/_ext/157685732/LED1_OFF.o \
	${OBJECTDIR}/_ext/157685732/LED2_ON.o \
	${OBJECTDIR}/testing/fis_test.o \
	${OBJECTDIR}/_ext/157685732/LED3_OFF.o \
	${OBJECTDIR}/fis_post_proc.o \
	${OBJECTDIR}/_ext/157685732/LED2_OFF.o \
	${OBJECTDIR}/fis.o \
	${OBJECTDIR}/_ext/1601161056/wiringPi.o \
	${OBJECTDIR}/_ext/157685732/LED0_ON.o \
	${OBJECTDIR}/_ext/157685732/LED1_ON.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/fis_ai_pump.exe

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/fis_ai_pump.exe: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/fis_ai_pump ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/_ext/157685732/LED0_OFF.o: ../wiringPi/fisled/led/LED0_OFF.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/157685732
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/157685732/LED0_OFF.o ../wiringPi/fisled/led/LED0_OFF.c

${OBJECTDIR}/_ext/157685732/LED3_ON.o: ../wiringPi/fisled/led/LED3_ON.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/157685732
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/157685732/LED3_ON.o ../wiringPi/fisled/led/LED3_ON.c

${OBJECTDIR}/_ext/157685732/LED1_OFF.o: ../wiringPi/fisled/led/LED1_OFF.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/157685732
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/157685732/LED1_OFF.o ../wiringPi/fisled/led/LED1_OFF.c

${OBJECTDIR}/_ext/157685732/LED2_ON.o: ../wiringPi/fisled/led/LED2_ON.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/157685732
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/157685732/LED2_ON.o ../wiringPi/fisled/led/LED2_ON.c

${OBJECTDIR}/testing/fis_test.o: testing/fis_test.c 
	${MKDIR} -p ${OBJECTDIR}/testing
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/testing/fis_test.o testing/fis_test.c

${OBJECTDIR}/_ext/157685732/LED3_OFF.o: ../wiringPi/fisled/led/LED3_OFF.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/157685732
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/157685732/LED3_OFF.o ../wiringPi/fisled/led/LED3_OFF.c

${OBJECTDIR}/fis_post_proc.o: fis_post_proc.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/fis_post_proc.o fis_post_proc.c

${OBJECTDIR}/_ext/157685732/LED2_OFF.o: ../wiringPi/fisled/led/LED2_OFF.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/157685732
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/157685732/LED2_OFF.o ../wiringPi/fisled/led/LED2_OFF.c

${OBJECTDIR}/fis.o: fis.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/fis.o fis.c

${OBJECTDIR}/_ext/1601161056/wiringPi.o: ../wiringPi/fisled/wiringPi.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1601161056
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1601161056/wiringPi.o ../wiringPi/fisled/wiringPi.c

${OBJECTDIR}/_ext/157685732/LED0_ON.o: ../wiringPi/fisled/led/LED0_ON.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/157685732
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/157685732/LED0_ON.o ../wiringPi/fisled/led/LED0_ON.c

${OBJECTDIR}/_ext/157685732/LED1_ON.o: ../wiringPi/fisled/led/LED1_ON.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/157685732
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/157685732/LED1_ON.o ../wiringPi/fisled/led/LED1_ON.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/fis_ai_pump.exe

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
