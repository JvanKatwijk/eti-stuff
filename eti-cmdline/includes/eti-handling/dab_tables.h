#ifndef _DAB_TABLES_H
#define _DAB_TABLES_H

#include <stdint.h>

struct eepprof {
  int sizemul;
  int ratemul;
  struct {
    int mul;
    int offset;
  } l[2];
  int pi[2];
};


/*
** Tables 8, 9, 39, 41: EEP Sub-channel sizes/coding rates etc.
** ETSI EN 300 401 V1.3.3 (2001-05), 6.2, P.46 & 11.3.2, P.159
*/
const struct eepprof eeptable[] = {
	{12, 8, {{6, -3}, {0, 3}}, {23, 22}},
	{8, 8, {{2, -3}, {4, 3}}, {13, 12}},
	{6, 8, {{6, -3}, {0, 3}}, {7, 6}},
	{4, 8, {{4, -3}, {2, 3}}, {2, 1}},
	{27, 32, {{24, -3}, {0, 3}}, {9, 8}},
	{21, 32, {{24, -3}, {0, 3}}, {5, 4}},
	{18, 32, {{24, -3}, {0, 3}}, {3, 2}},
	{15, 32, {{24, -3}, {0, 3}}, {1, 0}}
};
/* Special case for EEP 2-A when bitate == 8 */
const struct eepprof eep2a8kbps = {
  8, 8, {{0, 5}, {0, 1}}, {3, 12}
};


#endif


