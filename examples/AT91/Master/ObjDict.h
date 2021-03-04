
/* File generated by gen_cfile.py. Should not be modified. */

#ifndef OBJDICT_H
#define OBJDICT_H

#include "data.h"

/* Prototypes of function provided by object dictionnary */
UNS32 ObjDict_valueRangeTest (UNS8 typeValue, void * value);
const indextable * ObjDict_scanIndexOD (UNS16 wIndex, UNS32 * errorCode, ODCallback_t **callbacks);

/* Master node data struct */
extern CO_Data ObjDict_Data;
extern UNS8 DI1;		/* Mapped at index 0x2000, subindex 0x00*/
extern UNS8 DI2;		/* Mapped at index 0x2001, subindex 0x00*/
extern UNS8 DO1;		/* Mapped at index 0x2100, subindex 0x00*/
extern UNS8 DO2;		/* Mapped at index 0x2101, subindex 0x00*/

#endif // OBJDICT_H
