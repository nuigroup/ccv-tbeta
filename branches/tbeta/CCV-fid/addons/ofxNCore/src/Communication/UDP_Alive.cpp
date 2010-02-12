#include "UDP_Alive.h"

UDP_Alive::UDP_Alive(){
	isINConnected = false;
	isOUTConnected = false;
};


UDP_Alive::UDP_Alive(string _host, int port_in,int port_out,string host_out) {
	isINConnected = false;
	isOUTConnected = false;
	host = _host;
	udp_in_port = port_in;
	udp_out_host= host_out;
	udp_out_port = port_out;
};


UDP_Alive:: ~UDP_Alive(){
	udpConnection_OUT.Close();
	udpConnection_IN.Close();
};

void UDP_Alive::init_Connections(){
	init_IN_connection();
	init_OUT_connection();
}
//IN
void UDP_Alive::init_IN_connection(){
	init_IN_connection(udp_in_port);
};
void UDP_Alive::init_IN_connection(int port){
	//udp_in_port = port;
	udpConnection_IN.Create();
	isINConnected = udpConnection_IN.Bind(udp_in_port);
	udpConnection_IN.SetNonBlocking(true);
	printf("UDP.Alive.isINConnected: %s\n",isINConnected ? "true":"false");
	printf("udp_in_port: %i\n",udp_in_port);
};
//OUT
void UDP_Alive::init_OUT_connection(){
	printf("initOUT:udp_out_host: %s\n",udp_out_host.c_str() );
	init_OUT_connection(udp_out_host,udp_out_port);
};

void UDP_Alive::init_OUT_connection(string _host,int _port){

	//udp_out_host = _host;
	//udp_out_port = _port;

	udpConnection_OUT.Create();
	isOUTConnected = udpConnection_OUT.Connect(udp_out_host.c_str(),udp_out_port);
	udpConnection_OUT.SetNonBlocking(true);
	printf("UDP.Alive.isOUTConnected: %s\n",isOUTConnected ? "true":"false");
	printf("udp_out_host: %s\n",udp_out_host.c_str());
	printf("udp_out_port: %i\n",udp_out_port);
};

void UDP_Alive::readMessage(){

	udpConnection_IN.Receive(udpMessage,256);
	string message=udpMessage;
	if(message.compare("")==0)return;

	testMessage = "<command id=\"getstatus\" ></command>";
	if(message.compare(testMessage) == 0){
		printf("UDP_alive_in: %s\n",message.c_str());				
		sendAliveMessage();
		return;
	}


	testMessage = "<command id=\"setBackground\"></command>";
	if(message.compare(testMessage) == 0){	
		printf("UDP_onUDP_makeAndSaveBackground:%s\n",message.c_str());
		char chr ;
		ofNotifyEvent(onUDP_makeAndSaveBackground, chr, this);
		return;
		}

}

void UDP_Alive::sendAliveMessage(){
		char udpOutMessage[256];
		sprintf(udpOutMessage,"<command id=\"status\" wert=\"alive\" ip=\"%s\" ></command>",host.c_str());// Strings to const char* convert ! 
		int sent = udpConnection_OUT.Send(udpOutMessage,sizeof(udpOutMessage));

}