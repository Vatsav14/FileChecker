#include <mosquitto.h>

class myMosq
{
  private:
   const char * host;
   const char * id;
   const char * topic;
   int port;
   int keepalive;
  
   void on_connect(int rc);
   void on_disconnect(int rc);
   void on_publish(int mid);
  public:
    myMosq(const char *id, const char * _topic, const char *host, int port);
   ~myMosq();
   bool send_message(const char * _message);
};

myMosq::myMosq(const char * _id,const char * _topic, const char * _host, int _port)
{
    mosquitto_lib_init();        // Mandatory initialization for mosquitto library
    this->keepalive = 60;    // Basic configuration setup for myMosq class
    this->id = _id;
    this->port = _port;
    this->host = _host;
    this->topic = _topic;
    mosquitto_connect_async(host,     // non blocking connection to broker request
    port,
    keepalive);
    mosquitto_loop_start();            // Start thread managing connection / publish / subscribe
};
myMosq::~myMosq() {
    mosquitto_loop_stop();            // Kill the thread
    mosquitto_lib_cleanup();    // Mosquitto library cleanup
}
bool myMosq::send_message(const  char * _message)
{
// Send message - depending on QoS, mosquitto lib managed re-submission this the thread
//
// * NULL : Message Id (int *) this allow to latter get status of each message
// * topic : topic to be used
// * lenght of the message
// * message
// * qos (0,1,2)
// * retain (boolean) - indicates if message is retained on broker or not
// Should return MOSQ_ERR_SUCCESS
    int ret = mosquitto_publish(NULL,this->topic,strlen(_message),_message,1,false);
    return ( ret == MOSQ_ERR_SUCCESS );
}
void myMosq::on_disconnect(int rc) {
    std::cout << ">> myMosq - disconnection(" << rc << ")" << std::endl;
}
void myMosq::on_connect(int rc)
{
    if ( rc == 0 ) {
        std::cout << ">> myMosq - connected with server" << std::endl;
    }
    else {
        std::cout << ">> myMosq - Impossible to connect with server(" << rc << ")" << std::endl;
    }
}
void myMosq::on_publish(int mid)
{
    std::cout << ">> myMosq - Message (" << mid << ") succeed to be published " << std::endl;
}

