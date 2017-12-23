
export TARGET_ROOT=/mnt/work/mt7688/openWrtLinkit
export STAGING_DIR=$TARGET_ROOT/staging_dir
export GCC_PATH=$STAGING_DIR/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2/bin
export PROTOBUF_PATH=$TARGET_ROOT/build_dir/target-mipsel_24kec+dsp_uClibc-0.9.33.2/libprotobuf-c-v1.0.1
export PROTOINC=$PROTOBUF_PATH
export PROTOLIB=$PROTOBUF_PATH/protobuf-c/.libs
export PATH=$PATH:$GCC_PATH
export CC=mipsel-openwrt-linux-uclibc-gcc
