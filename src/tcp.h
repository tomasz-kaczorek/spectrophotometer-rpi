#ifndef TCP_H
#define TCP_H

struct tcp
{
	int server;
	int client;
	struct
	{
		char buffer[100000];
		char * buffer_end;
		char * data;
		char * data_end;
	} input;
	struct
	{
		char buffer[100000];
		char * buffer_end;
		char * data;
		char * data_end;
	} output;
};

extern struct tcp tcp;

int tcp_server_init(char const * address, int port);

int tcp_server_deinit();

void tcp_server_start(void (* message_handler)(char * message, int length));

int tcp_server_printf(char const * format, ...);

#endif //TCP_SOCKET_H

