#pragma once
void InitializeNetwork();
void ShutdownNetwork();
void getpos(int id, int* x, int * y, unsigned int* numofconnection);

void getnpcPos(int id, int *x, int* y, unsigned int* numofnpc);

void LoginOut(int c1);