# Install boost libraries we needed
sudo apt-get install -y libboost-dev libboost-date-time-dev libboost-thread-dev libboost-system-dev libboost-filesystem-dev libboost-regex-dev libboost-signals-dev libboost-iostreams-dev libboost-test-dev

# Install other libraries we needed
sudo apt-get install -y libbz2-dev libzip-dev openssl libxml2-dev

# Install Log4CPlus
wget http://downloads.sourceforge.net/project/log4cplus/log4cplus-stable/1.0.4/log4cplus-1.0.4-rc10.tar.bz2

tar xvf log4cplus-1.0.4-rc10.tar.bz2

cd log4cplus-1.0.4-rc10

./configure

sudo make install
