# Install boost libraries we needed
sudo apt-get install -y libboost-dev libboost-date-time-dev libboost-thread-dev libboost-system-dev libboost-filesystem-dev libboost-regex-dev libboost-signals-dev libboost-iostreams-dev libboost-test-dev libboost-program-options-dev

# Install other libraries we needed
sudo apt-get install -y libbz2-dev libzip-dev openssl libxml2-dev

# Install Log4CPlus
cd log4cplus-1.0.3
cmake -G "Unix Makefiles"
make 
sudo make install
cd -

# Install yaml-cpp
cd yaml-cpp-0.2.5
cmake -G "Unix Makefiles"
make 
sudo make install
cd -
