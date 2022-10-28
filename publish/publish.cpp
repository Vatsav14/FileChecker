#include <stdio.h>
#include <mosquitto.h>
#include <cstring>


int publish(const char *id, const char *message){
	int rc;
	struct mosquitto * mosq;

	mosquitto_lib_init();

	mosq = mosquitto_new(id, true, NULL);

	rc = mosquitto_connect(mosq, "localhost", 1883, 60);

    if(rc != 0){
		printf("Client could not connect to broker! Error Code: %d\n", rc);
		mosquitto_destroy(mosq);
		return -1;
	}
	printf("We are now connected to the broker!\n");

	mosquitto_publish(mosq, NULL, "agent/face_analyzer", strlen(message) + 1, message, 0, false);

	mosquitto_disconnect(mosq);
	mosquitto_destroy(mosq);

	mosquitto_lib_cleanup();
	return 0;
}
