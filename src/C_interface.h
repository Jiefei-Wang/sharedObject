#pragma once
#include <Rcpp.h>
#include "SharedObject_types.h"

DID C_getDataID(DID did);
PID C_getProcessID(DID did);
int C_getTypeID(DID did);
ULLong C_getLength(DID did);
ULLong C_getTotalSize(DID did);
bool C_getCopyOnWrite(DID did);
bool C_getSharedSubset(DID did);
bool C_getSharedCopy(DID did);

void C_setCopyOnWrite(DID did, bool value);
void C_setSharedSub(DID did, bool value);
void C_setSharedCopy(DID did, bool value);


