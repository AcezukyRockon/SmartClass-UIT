Installations for simple_facenet:
1. Download and Install python 3.7 (sudo apt-install python3.7)
2. Open CMD/terminal, install following python packages in Command :
python3.7 -m pip install --no-cache-dir tensorflow==1.15.0
python3.7 -m pip install --no-cache-dir scipy==1.1.0
python3.7 -m pip install --no-cache-dir scikit-learn
python3.7 -m pip install --no-cache-dir opencv-python==4.5.5
python3.7 -m pip install --no-cache-dir h5py
python3.7 -m pip install --no-cache-dir matplotlib
python3.7 -m pip install --no-cache-dir Pillow
python3.7 -m pip install --no-cache-dir requests
python3.7 -m pip install --no-cache-dir psutil
3. cd to this folder
4. run file train.py:
python3.7 train.py
5. Run file detection.py (change the input to camera/mp4 file at the end of the code)

Source:
https://github.com/nkloi/simple_facenet

--------------------------------------------------------------------------------------------

Installations for mongoDB:
MongoDB Atlas:
1. Login at https://cloud.mongodb.com/
2. Find project 0 and find cluster SmartPodium (https://i.imgur.com/cypbpE3.png)
3. Click to "Browse Collection", you can see SinhVien collection (table) and its data.

Your computer:
1. Download and Install python 3.7 (sudo apt-install python3.7)
2. Open CMD/terminal ad Admin, install following python packages in Command :
python3.7 -m pip install --no-cache-dir pymongo
python3.7 -m pip install --no-cache-dir dnspython
python3.7 -m pip install --no-cache-dir python-dateutil
python3.7 -m pip install --no-cache-dir pandas
python3.7 -m pip install --no-cache-dir certifi
3. cd to this folder
4. run file pymongo_test_query:
python3.7 pymongo_test_query
5. sinh vien datas will be printed in the command line

Source:
https://www.mongodb.com/languages/python
https://www.mongodb.com/docs/manual/reference/sql-comparison/

--------------------------------------------------------------------------------------------

Installations for HiveMQ and MongoDB:
1. Go to http://www.hivemq.com/demos/websocket-client/
2. Fill Username and Password as "nhom1", then click Connect
3. On Publish area, replace topic with "LED/Status"
4. On Subcribe area, add topic "LED/Status" as QOS0
5. Go to folder "python_mongo_hivemq", install paho-mqtt: python3.7 -m pip install paho-mqtt
6. After that, on terminal, run: python3.7 pymongo_test_query_hivemq.py
7. On terminal, there will show json documents of Trang's database
8. At same time, go to hivemq site and check Message area, there should be json documents message

Source:
https://www.hivemq.com/blog/mqtt-client-library-paho-python/

--------------------------------------------------------------------------------------------

Installation for HiveMQ and ESP32:
1. Setup Arduino IDE ESP32: https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/
2. Open file esp32_hivemq_led.ino, setup your WIFI ssid and password. Then save the file.
3. Choose board and port, then upload code (you may need to press Boot button on ESP32 during upload)
4. Go to HiveMQ websocket, then fill username and password as "nhom1", then click connect
5. On Publish area, replace topic with "LED/Status"
6. On Subcribe area, add topic "LED/Status" as QOS0 
7. Try type "ON" on text box of Publish area, then click Publish
8. LED will flash and Message area will show the "ON" message

Source:
https://khuenguyencreator.com/lap-trinh-esp32-mqtt-bat-tat-den-voi-hivemq-broker/
