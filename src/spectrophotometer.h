#ifndef SPECTROPHOTOMETER_H
#define SPECTROPHOTOMETER_H

int spectrophotometer_init();
int spectrophotometer_deinit();

void spectrophotometer_message_handler(char * message, int length);

#endif //SPECTROPHOTOMETER_H
