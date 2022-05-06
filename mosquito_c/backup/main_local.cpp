#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <mosquitto.h>

int main(){
	int rc;
	struct mosquitto * mosq;

	mosquitto_lib_init();
	mosq = mosquitto_new(NULL, true, NULL);

	rc = mosquitto_connect(mosq, "broker.mqttdashboard.com", 1883, 60);
	if(rc != 0){
		printf("Client could not connect to broker! Error Code: %d\n", rc);
		mosquitto_destroy(mosq);
		return -1;
	}
	printf("We are now connected to the broker!\n");
	mosquitto_loop_forever(mosq);
	int result = mosquitto_publish(mosq, NULL, "LED/Status", 2, "ON", 0, false);
	printf("%d",result);
	
	mosquitto_disconnect(mosq);
	//mosquitto_loop_stop(mosq, false);
	mosquitto_destroy(mosq);

	mosquitto_lib_cleanup();
	return 0;
}

