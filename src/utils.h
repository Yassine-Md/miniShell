#ifndef __UTILS_H__
#define __UTILS_H__

void createPipes(int fd[][2], int n);
void closeUnusedPipes(int fd[][2], int n, int currentPipe);


#endif
