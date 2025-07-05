#ifndef MSPM0_CLOCK_H
#define MSPM0_CLOCK_H

#ifdef __cplusplus
extern "C" {
#endif

int mspm0_delay_ms(unsigned long num_ms);
int mspm0_get_clock_ms(unsigned long *count);

#ifdef __cplusplus
}
#endif

#endif /* MSPM0_CLOCK_H */