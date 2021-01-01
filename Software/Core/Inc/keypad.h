
#ifndef INC_KEYPAD_H_
#define INC_KEYPAD_H_


// Now defines for the Keyboard.
// The keyboard uses a TCA8418 keypad scann IC.
// It is on I2C bus 1 with an address of 0x68
// The INT output of the '8418 connects to MCU GPIO PA10
// The /RST input connects to MCU GPIO PA9
#define	TCA8418_ADDR		0x68	// 0110 100r

#define TCA_REG_CFG			0x01		// Configuration register
#define TCA_REG_INT_STAT	0x02		// Key Interrupt status
#define TCA_REG_LCK_EC		0x03		// Key event counter
#define TCA_REG_EVENT_A		0x04		// Key event register A
#define TCA_REG_KP_GPIO1	0x1D		// keypad or gpio (o=gpio, 1=keypad)
#define TCA_REG_KP_GPIO2	0x1E		// keypad or gpio (o=gpio, 1=keypad)
#define TCA_REG_KP_GPIO3	0x1F		// keypad or gpio (o=gpio, 1=keypad)


// Key codes  - these are decimal values.
// Read datasheet page 16, 8,3,1,3 for how to read.
// Bit 7 is set for a KEYDOWN event and cleared for a KEYUP event.
enum {
    KEY_BASE    = 41,
    KEY_DSP     = 42,
    KEY_C       = 43,
    KEY_D       = 44,
    KEY_E       = 45,
    KEY_F       = 46,
    KEY_DIV     = 47,
    KEY_SEND    = 31,
    KEY_CHS     = 32,
    KEY_B       = 33,
    KEY_7       = 34,
    KEY_8       = 35,
    KEY_9       = 36,
    KEY_X       = 37,
    KEY_COPY    = 21,
    KEY_ROL     = 22,
    KEY_A       = 23,
    KEY_4       = 24,
    KEY_5       = 25,
    KEY_6       = 26,
    KEY_MINUS   = 27,
    KEY_PASTE   = 11,
    KEY_X_Y     = 12,
    KEY_MOD     = 13,
    KEY_1       = 14,
    KEY_2       = 15,
    KEY_3       = 16,
    KEY_PLUS    = 17,
    KEY_FUNC    = 1,
    KEY_CLX     = 2,
    KEY_DEL     = 3,
    KEY_0       = 4,
    KEY_DP      = 5,
    KEY_ENTER   = 7,
	KEY_PWR_ON_OFF = 88		// this one is  on gpio B pin 12
};

#define		KEYDOWN_EVENT	0x80

// Exported prototypes for keyboard
void	KeyboardInitialize(void);
uint8_t	ReadKeypad(void);

#endif /* INC_KEYPAD_H_ */
