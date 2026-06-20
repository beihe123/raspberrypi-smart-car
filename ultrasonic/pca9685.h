#ifndef PCA9685_H
#define PCA9685_H
#ifdef __cplusplus
extern "C" {
#endif

extern int pca9685Setup(const int pinBase, const int i2cAddress, float freq);
extern void pca9685PWMFreq(int fd, float freq);
extern void pca9685PWMReset(int fd);
extern void pca9685PWMWrite(int fd, int pin, int on, int off);
extern void pca9685PWMRead(int fd, int pin, int *on, int *off);
extern void pca9685FullOn(int fd, int pin, int tf);
extern void pca9685FullOff(int fd, int pin, int tf);

#ifdef __cplusplus
}
#endif
#endif