#!/bin/bash
SCRIPT=`pwd`/$0
FILENAME=`basename $SCRIPT`
PATHNAME=`dirname $SCRIPT`
ROOT=$PATHNAME/..
INSTALL_PATH=$ROOT/deps
THIRD_PARTY_PATH=$ROOT/third_party

CLEANUP=false

SUDO=""
if [[ $EUID -ne 0 ]]; then
   SUDO="sudo -E"
fi

# exit when any command fails
set -e

install_log4cxx(){
  echo "- start install log4cxx."

  if ! `pkg-config --exists liblog4cxx`; then
    pushd $DEPS

    if [ ! -d $DEPS/logging-log4cxx-0.13.0-RC1 ] ; then
      wget https://github.com/apache/logging-log4cxx/archive/refs/tags/v0.13.0-RC1.zip
      unzip v0.13.0-RC1.zip
      rm v0.13.0-RC1.zip
    else
        echo "log4cxx already downloaded."
    fi

    pushd logging-log4cxx-0.13.0-RC1
    cmake . --install-prefix=/usr -Bbuild -DBUILD_TESTING=OFF -DBUILD_SITE=OFF -DLOG4CXX_QT_SUPPORT=OFF -DBUILD_SHARED_LIBS=ON
    cmake --build build -- -j 8
    cmake --install build
    popd
    popd

  else
    echo "liblog4cxx already installed." && return 0
  fi

}

install_fmt(){
  echo "- start install fmt."
  
  if ! `pkg-config --exists fmt`; then
    pushd $DEPS

    if [ ! -d $DEPS/fmt-9.0.0 ] ; then
      wget https://cdn.resource.100mix.cn/CPlusPlusDependency/fmt-9.0.0.zip
      unzip fmt-9.0.0.zip
      rm fmt-9.0.0.zip
    else
        echo "fmt already downloaded."
    fi

    pushd fmt-9.0.0
    cmake . --install-prefix=/usr -Bbuild  -DFMT_TEST=FALSE -DFMT_FUZZ=FALSE -DBUILD_SHARED_LIBS=1
    cmake --build build
    cmake --install build
    popd
    popd

  else
    echo "fmt already installed." && return 0
  fi

}

install_uv(){
  echo "- start install uv."
  
  if ! `pkg-config --exists libuv`; then
    pushd $DEPS

    if [ ! -d $DEPS/libuv-v1.33.1 ] ; then
      wget https://dist.libuv.org/dist/v1.33.1/libuv-v1.33.1.tar.gz
      tar -zxvf libuv-v1.33.1.tar.gz
      rm libuv-v1.33.1.tar.gz
    else
        echo "libuv already downloaded."
    fi

    pushd libuv-v1.33.1
    cmake . --install-prefix=/usr -Bbuild -DLIBUV_BUILD_TESTS=FALSE
    cmake --build build -- -j 8
    cmake --install build
    popd
    popd

  else
    echo "uv already installed." && return 0
  fi
}

install_deps(){
  $SUDO apt-get update -y
  $SUDO apt-get install git build-essential cmake golang-go zlib1g-dev -y
}

install_absl(){
    echo "- install_absl."

    if [ ! -d $THIRD_PARTY_PATH/abseil-cpp ] ; then
      pushd $THIRD_PARTY_PATH
      git clone https://github.com/abseil/abseil-cpp.git
      pushd abseil-cpp
      git checkout 32d314d0f5bb0ca3ff71ece49c71a728c128d43e
      popd
      popd
    else
        echo "absl already installed." && return 0
    fi 
}

install_protobuf(){
    echo "- install_protobuf."

    if [ ! -d $THIRD_PARTY_PATH/protobuf ] ; then
      pushd $THIRD_PARTY_PATH
      git clone https://github.com/protocolbuffers/protobuf.git
      pushd protobuf
      git checkout v4.23.4

      cmake . --install-prefix=$INSTALL_PATH -Bbuild -Dprotobuf_BUILD_TESTS=OFF -DCMAKE_CXX_STANDARD=17 -DABSL_PROPAGATE_CXX_STD=ON -DABSL_ROOT_DIR=$THIRD_PARTY_PATH/abseil-cpp
      cmake --build build -- -j 8
      cmake --install build

      popd
      popd
    else
        echo "protobuf already installed." && return 0
    fi 
}

install_boringssl(){
    echo "- install_boringssl."

    if [ ! -d $THIRD_PARTY_PATH/boringssl ] ; then
      pushd $THIRD_PARTY_PATH
      git clone https://github.com/google/boringssl.git
      pushd boringssl
      git checkout 62ab404cb560a6886196fe65cd3381f2ae3166ca

      cmake . --install-prefix=$INSTALL_PATH -Bbuild
      cmake --build build -- -j 8
      cmake --install build
      popd
      popd
    else
        echo "boringssl already installed." && return 0
    fi 
}

install_googleurl(){
    echo "- install_googleurl."

    if [ ! -d $THIRD_PARTY_PATH/googleurl ] ; then
      pushd $THIRD_PARTY_PATH
      git clone https://github.com/0-u-0/googleurl.git
      pushd googleurl
      git checkout 1c9228e9b7836a203e62f7cb08f578cb72aedd19
      popd
      popd
    else
        echo "googleurl already installed." && return 0
    fi 
}

install_quic_trace(){
    echo "- install_quic_trace."
    
    if [ ! -d $THIRD_PARTY_PATH/quic-trace ] ; then
      pushd $THIRD_PARTY_PATH
      git clone https://github.com/google/quic-trace.git
      pushd quic-trace
      git checkout c7b993eb750e60c307e82f75763600d9c06a6de1
      popd
      popd
    else
        echo "quic_trace already installed." && return 0
    fi 
}

install_quiche(){
    echo "- install_quiche."

    if [ ! -d $THIRD_PARTY_PATH/quiche ] ; then
      pushd $THIRD_PARTY_PATH
      git clone https://github.com/google/quiche.git
      pushd quiche
      git checkout d907cd88312862f0577833c73c13e1b3bd51b090
      popd
      popd
    else
        echo "quiche already installed." && return 0
    fi 
}


parse_arguments(){
  while [ "$1" != "" ]; do
    case $1 in
      "--cleanup")
        CLEANUP=true
        ;;
    esac
    shift
  done
}

parse_arguments $*

if [ ! -d $INSTALL_PATH ]; then
  mkdir -p $INSTALL_PATH
fi

if [ ! -d $THIRD_PARTY_PATH ]; then
  mkdir -p $THIRD_PARTY_PATH
fi

install_deps
install_absl
install_protobuf
install_boringssl
install_googleurl
install_quic_trace
install_quiche
# install_log4cxx
# install_fmt
# install_uv
