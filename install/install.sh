cd ..
rm -rf build
mkdir build
cd build
cmake ..
make install
cd ..
cd server
cp dz_brightness_control.service /etc/systemd/system/
systemctl daemon-reload
systemctl start dz_brightness_control
systemctl enable dz_brightness_control
echo "*********************************************"
echo "*********************************************"
echo "*********************************************"
echo "--brightness_control installed successfully--"
echo "*********************************************"
echo "*********************************************"
echo "*********************************************"