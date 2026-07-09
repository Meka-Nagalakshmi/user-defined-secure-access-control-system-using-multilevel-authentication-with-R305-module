#include "types.h"
void i2c_eeprom_write_byte(u8 SlaveAddr, u16 BuffAddr,u8 data);
u8 i2c_eeprom_read_byte(u8 SlaveAddr,u8 BufferAddr);
void i2c_eeprom_write_page(u8 SlavAddr,u16 BuffAddr,s8 *P,u8 nbytes);
void i2c_eeprom_seq_read(u8 SlaveAddr,u16 BuffAdrr,s8 *p,u8 nbytes);
