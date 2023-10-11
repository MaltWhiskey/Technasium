/*
python -c "import sys;a=sys.argv;open(a[2],'wb').write(('const unsigned char '+a[3]+'[] = {'+','.join([hex(b) for b in open(a[1],'rb').read()])+'};').encode('utf-8'))" <binary file> <header file> <array name>
*/

#include <stdint.h>
#include "hiss.h"
//#include "howl.h"
#include "scream.h"
#include "thunder1.h"
//#include "magniet.h"

struct sample_dsc_t {
  const uint32_t size;
  const uint16_t rate;
  const uint8_t *data;
};

//const sample_dsc_t howl_dsc = {sizeof(howl), 44100, howl};
const sample_dsc_t scream_dsc = {sizeof(scream), 44100, scream};
const sample_dsc_t hiss_dsc = {sizeof(hiss), 44100, hiss};
const sample_dsc_t thunder1_dsc = {sizeof(thunder1), 44100, thunder1};
//const sample_dsc_t magniet_dsc = {sizeof(magniet), 44100, magniet};                               