echo 'apt install -y software-properties-common'
apt install -y software-properties-common

echo 'add-apt-repository -y ppa:deadsnakes/ppa'
add-apt-repository -y ppa:deadsnakes/ppa

echo 'apt install -y python3.7'
apt install -y python3.7

#ln -s /usr/bin/python3.7 /usr/bin/python

#ln -fs /usr/share/zoneinfo/America/New_York /etc/localtime
#apt-get install -y tzdata
#dpkg-reconfigure --frontend noninteractive tzdata

#echo 'apt install -y python3-distutils'
#apt install -y python3-distutils

echo 'rm -rf /tmp/pip/'
rm -rf /tmp/pip/

echo 'mkdir /tmp/pip'
mkdir /tmp/pip

echo 'cp get-pip.py -p /tmp/pip'
cp get-pip.py -p /tmp/pip/

echo 'python3.7 /tmp/pip/get-pip.py'
python3.7 /tmp/pip/get-pip.py

echo 'rm -rf /tmp/pip/*'
rm -rf /tmp/pip/*


