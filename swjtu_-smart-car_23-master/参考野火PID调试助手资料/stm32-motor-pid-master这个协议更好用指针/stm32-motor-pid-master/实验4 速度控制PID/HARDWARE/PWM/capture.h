#ifndef _CAPTURE_H
#define _CAPTURE_H

void TIMx_encoder_init(void);
void calc_motor_rotate_speed(void);
int read_encoder(void);
#endif
