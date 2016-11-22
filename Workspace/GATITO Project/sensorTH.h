#define CLK_PIN 		BIT6
#define DATA_PIN		BIT5
#define PSENOUT			P5OUT
#define PSENIN			P5IN
#define PSENSEL			P5SEL
#define PSENDIR			P5DIR

void sensor_init(void);
void transmition_start(void);
float sensor_get_t(void);
float sensor_get_h(void);
int sensor_send_cmd(unsigned char cmd);
void s_connectionreset(void);
char s_softreset(void);