#ifndef __DRV_QMC5883P_H__
#define __DRV_QMC5883P_H__

void qmc5883p_init(void);

int16_t qmc5883p_read_x(void);
int16_t qmc5883p_read_y(void);
int16_t qmc5883p_read_z(void);

#endif

