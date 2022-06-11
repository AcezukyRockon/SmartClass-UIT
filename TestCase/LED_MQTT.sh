# thay so 20 thanh so khac de tang/giam so lan
for i in $(seq 1 20);
do
    echo "ONN $i time(s)"
    mosquitto_pub -m "ONN" -t LED/Status
    sleep 1
    echo "OFF $i time(s)"
    mosquitto_pub -m "OFF" -t LED/Status
    sleep 1
done
