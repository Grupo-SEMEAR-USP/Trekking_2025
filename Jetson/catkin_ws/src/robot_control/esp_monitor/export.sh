source /home/${USER}/esp-idf/export.sh
cd /home/${USER}/ESP32_Task_Monitor
idf.py -p /dev/ttyACM0 monitor
