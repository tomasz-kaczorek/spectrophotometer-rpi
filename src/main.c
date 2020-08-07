#include "spectrophotometer.h"
#include "tcp.h"

int main(int argc, char ** argv)
{
	(void) argc; (void) argv;
	if(spectrophotometer_init()) return -1;
	tcp_server_init("0.0.0.0", 31415);
	tcp_server_start(&spectrophotometer_message_handler);
	return 0;
}
