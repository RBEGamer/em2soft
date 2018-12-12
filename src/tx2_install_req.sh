#!bin/bash


sudo apt-get update -y
sudo apt-get upgrade -y

# INSTALL PYTHON
sudo apt-get install nano
sudo apt-get install python3-numpy swig python3-dev python3-pip python-pip python3-wheel -y


# INSTALL NODE
curl -sL https://deb.nodesource.com/setup_8.x | sudo -E bash -
sudo apt-get install -y nodejs build-essential





# INSTALL ROS
cd ~
git clone https://github.com/jetsonhacks/installROSTX2.git
cd installROSTX2
git reset --soft ae6ba4d639
./installROS.sh -p ros-kinetic-desktop -p ros-kinetic-rgbd-launch
./setupCatkinWorkspace.sh default_ros_workspace
