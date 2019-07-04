/* General purpose Reed-Solomon decoder for 8-bit symbols (255/223)
 * Copyright 2003 Phil Karn, KA9Q
 * May be used under the terms of the GNU Lesser General Public License (LGPL)
 * 
 * Refactoring for PIC microcontrollers by Alfredo Ortega
 */

typedef unsigned char data_t;

#define MM 8
#define NN 255
#define ALPHA_TO CCSDS_alpha_to
#define INDEX_OF CCSDS_index_of
#define GENPOLY CCSDS_poly
#define NROOTS 32
#define FCR 112
#define PRIM 11
#define IPRIM 116
#define PAD pad

int decode_rs_8(data_t *data, int *eras_pos, int no_eras, int pad);
void encode_rs_8_c(data_t *data, data_t *parity,int pad);
