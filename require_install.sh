#!/bin/bash


echo "=================== 1, check QT install ======================="
package=qmake2

#if [ `dpkg -l | grep ${package} |wc -l` -ne 0 ]; then
if [ -d "/opt/Qt5.8.0" ]; then
  echo "软件包 QT 已经安装。"
  echo "==== check QT done."
  exit 0
else
  echo "软件包 QT 没有安装"
fi


echo "=================== 2, check QT be downloaded =================="

qtFile="qt-opensource-linux-x64-5.8.0.run"

cd ~/Downloads

if [ ! -f "$qtFile" ]; then
  #touch "$qtFile"
  echo "-- \"$qtFile\" not exists, begin download. "
  wget http://download.qt.io/archive/qt/5.8/5.8.0/qt-opensource-linux-x64-5.8.0.run
else
  echo "-- \"$qtFile\" exists in \"~/Downloads\" "
fi

echo "inlab" | sudo -S chmod -R 777 $qtFile
echo "-- begin install QT, ===!!!=== choose: -- next -- skip -- next --- ..."
echo "inlab" | sudo -S ./$qtFile
echo "inlab" | sudo -S apt-get install libqt4-dev
echo "inlab" | sudo -S apt-get install build-essential
echo "-- end install QT"

echo "==== check QT done."
