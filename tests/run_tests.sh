if [ ! -d "build" ]; then
  mkdir build
fi
cd build
cmake ..
if [ $? -eq 0 ]; then
    make clean
    make -j $(nproc --all)
    if [ $? -eq 0 ]; then
        ./test
    fi
fi
