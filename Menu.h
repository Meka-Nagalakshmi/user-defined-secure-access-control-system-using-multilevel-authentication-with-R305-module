#ifndef _EXINT_MENU_H_
#define _EXINT_MEMU_H_

#define EINT2_INPUT_PIN 0x0000C000
#define EINT2_VIC_CHNO 16

void init_eint2(void);
void eint2_isr(void) __irq;
unsigned short is_id_in_db(char id);
void enroll_id(void);
void menu(void);
s8 check_admin(void);
void login(void);
void forget_password(void);
void finger_edit(void);
void password_edit(void);
void init_ids(void);
void disable(void);
void enable(void);
void edit(void);
void delete_id(void);
#endif
