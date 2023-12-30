/*
python -c "import sys;a=sys.argv;open(a[2],'wb').write(('const unsigned char
'+a[3]+'[] = {'+','.join([hex(b) for b in
open(a[1],'rb').read()])+'};').encode('utf-8'))" <binary file> <header file>
<array name>

xxd -i file.raw > file.h
xxd -i Macabre.raw > ../Software/Pumpkin/src/macabre.h

*/

#include <stdint.h>

#include "hiss.h"
#include "howl.h"
#include "macabre.h"
#include "magniet.h"
#include "scream.h"
#include "thunder.h"

struct sample_dsc_t {
  const uint32_t size;
  const uint16_t rate;
  const uint8_t *data;
};

const sample_dsc_t howl_dsc = {sizeof(howl), 11025, howl};
const sample_dsc_t scream_dsc = {sizeof(scream), 11025, scream};
const sample_dsc_t hiss_dsc = {sizeof(hiss), 11025, hiss};
const sample_dsc_t thunder_dsc = {sizeof(thunder), 11025, thunder};
const sample_dsc_t magniet_dsc = {sizeof(magniet), 11025, magniet};
const sample_dsc_t macabre_dsc = {sizeof(macabre), 11025, macabre};