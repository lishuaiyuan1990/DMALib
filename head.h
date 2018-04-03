#define TEST_MAGIC 	'x'//定义幻数

#define CONFIG_PARA_DMA			_IO(TEST_MAGIC, 0)	
#define REG_SET_PARA				_IO(TEST_MAGIC, 1)
#define SYS_START_MAGIC      _IO(TEST_MAGIC, 2)

#define INIT_CMD	 0x03
#define OPEN_CMD 	 0x04
#define TRIGGER_MODE 0x05
#define PRF_SET 	 0x06
#define CHAL_ENABLED 0x07
#define E_VOLTAGE	 0x08
#define PULSE_WIDTH  0x09
#define GAIN_SET 	 0x0A
#define SONIC_DIS	 0x0B
#define SONIC_VEL 	 0x0C
#define INIT_OFFSET  0x0D
#define SAMPLE_NUM   0x0E
#define GAIN_SWITCH  0x0F
#define REC_CHANNEL  0x10
#define READ_DATA	 0x21
#define MAX_PKT_LEN (50000)
