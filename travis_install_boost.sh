#!/bin/bash
set -e
BOOST_VERSION="1_61_0"

# PREREQS:  BOOST_ROOT, CC, CXXFLAGS, LINKFLAGS

echo "Boost Root is set to: '${BOOST_ROOT}'"
if [ ! -d "${BOOST_ROOT}" ]; then
	BUILD_TYPE=${CC}
	echo "Build Type: ${BUILD_TYPE}"
	BOOST_FILE="boost_${BOOST_VERSION}.tar.bz2"
	if [ ! -d "/tmp/${BUILD_TYPE}" ]; then
		mkdir "/tmp/${BUILD_TYPE}"
	fi
	cd /tmp
	if [ ! -f "/tmp/${BOOST_FILE}" ]; then
		echo "Downloading Boost"
		wget -O "${BOOST_FILE}" "https://sourceforge.net/projects/boost/files/boost/1.61.0/boost_${BOOST_VERSION}.tar.bz2/download"
	fi
	cd "/tmp/${BUILD_TYPE}"
	echo "Expanding ${BOOST_FILE}" into `pwd`
	tar -xjf /tmp/${BOOST_FILE}

	echo "Building Boost";
	cd "boost_${BOOST_VERSION}";
	echo "Building Boost in '`pwd`'"
	./bootstrap.sh toolset=${CC} --prefix="${BOOST_ROOT}"
	./b2 toolset=${CC} cxxflags="${CXXFLAGS}" linkflags="${LINKFLAGS}" --prefix="${BOOST_ROOT}" install;
else
	echo "using cached boost folder"
fi

