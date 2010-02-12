#ifndef UDP_ALIVE_H
#define UDP_ALIVE_H

#include "ofxNetwork.h"
#include "ofEvents.h"

class UDP_Alive {

public:
	UDP_Alive();
	UDP_Alive(string _host, int port_in,int port_out,string host_out);
	~UDP_Alive();
	//alive Connection (to send a alive message to test if the application is still alive)
	ofxUDPManager		udpConnection_IN;
	ofxUDPManager		udpConnection_OUT;
	int					udp_in_port;
	string				udp_out_host;  
	int					udp_out_port;
	string				host;
	bool				isINConnected;
	bool				isOUTConnected;

	char udpMessage[256];
	string testMessage;


	//OF POCO EVENTS TO FORWARD TUIO EVENTS TO testApp or any other Class
	ofEvent<char> onUDP_makeAndSaveBackground;

	//Methods:
	void init_Connections();
	void init_OUT_connection();
	void init_OUT_connection(string _host,int _port);
	void init_IN_connection();
	void init_IN_connection(int port);
	void readMessage();
	void sendAliveMessage();

private:

};

#endif
