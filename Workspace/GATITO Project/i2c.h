#define I2CSEL     P3SEL
#define I2CDIR     P3DIR
#define I2COUT     P3OUT
#define SDA        BIT1
#define SCL        BIT3

void i2c_init(void);
void i2c_start_w(void);
void i2c_write(unsigned char data);
void i2c_read(unsigned char n);
void i2c_start_r(unsigned char addr);