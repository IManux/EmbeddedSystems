//Libraries here

//Defines here
#define DBG Serial
#define UART2 Serial2
#define UART_RX2  16
#define UART_TX2  17  
#define TMR1_SOFT_PERIOD    100
#define SENSOR_PERIOD       20
#define MODEM_PERIOD        50
#define QDATA_BUFF_SIZE    20
#define MODEM_OPERATION_PERIOD_DEFAULT  600

//Global
uint32_t tmr1_soft_cur = 0;
uint32_t tmr1_soft_pre = 0;
uint32_t tmr1_soft_diff = 0;
bool tmr1_soft_flag = false;
uint8_t tmr1_soft_secs = 10;

uint8_t tmr1_soft_sensor = SENSOR_PERIOD;
uint8_t tmr1_soft_modem = MODEM_PERIOD;   //send data
uint16_t tmr1_soft_g1 = 1;
uint16_t tmr1_soft_g2 = 2;

bool xtmr1_soft_modem = false;
bool xtmr1_soft_sensor = false;
bool xtmr1_soft_g1 = false;
bool xtmr1_soft_g2 = false;

struct mdata_t
{
    uint8_t var1;
    uint8_t var2;
    uint8_t var3;
    uint8_t var4;
    uint32_t tm;    //time total in seconds
};

//Queue
struct qdata_t
{
  uint8_t qhead;
  uint8_t qtail;
  uint8_t qmax;
  uint8_t dsize;
  uint8_t xring;
  struct mdata_t *qbuff;
};
struct qdata_t myqdata;

//modem state
enum modem_operation_t
{
    MODEM_INIT,
    MODEM_NETWORK_INIT,
    MODEM_NETWORK_CHECK,
    MODEM_GPRS_INIT,
    MODEM_GPRS_CHECK,
    MODEM_TCP_CONNECT,
    MODEM_TCP_DISCONNECT,
    MODEM_TCP_CHECK,
    MODEM_TCP_SEND,
    MODEM_TCP_READ,
    MODEM_STANDBY,
    MODEM_BLOCKED  
};

uint8_t modem_state_sub = 0;
uint16_t modem_send_data_tout = 0;
uint8_t modem_send_data_state = 0;
char modem_expect_response[34];
char modem_rx_buff[128];


//flags
struct modem_t {
    bool xinit;
    bool xnetwork;
    bool xgprs;
    bool xtcp_connected;
    bool xtcp_sent;
    bool xoperation_running;
    bool xoperation_done;
    bool xsend_data;
    bool xwait_resp;  //wait response
    bool xoperation_tout;
    uint8_t mgprs_cnt;
    uint8_t mnetwork_cnt;
    uint8_t minit_cnt;
    uint8_t mtcp_cnt;
    uint8_t evt;
    uint8_t operation_max_state;
    uint8_t operation_sub_state;
    uint16_t response_tout;
    uint16_t operation_tout;
    enum modem_operation_t operation_cur;
    enum modem_operation_t operation_prev;
};

struct modem_t mymodem = {  
                            false, false, false, false, false,
                            false, false, false, false, false,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            MODEM_STANDBY, MODEM_STANDBY
                         };

//commands at response
char *ok_reply = "OK";

//Prototype  Functions
void sys_soft_timers(void);
uint32_t diff_time(uint32_t t1, uint32_t t2);

//queue functions
struct qdata_t* queue_init(uint8_t n);
uint8_t queue_destroy(struct qdata_t *mq);
uint8_t queue_add(struct qdata_t *mq, struct mdata_t *md);
uint8_t queue_pop(struct qdata_t *mq, struct mdata_t *md);
uint8_t queue_npop(struct qdata_t *mq, struct mdata_t *md);
uint8_t queue_full(struct qdata_t *mq);
uint8_t queue_empty(struct qdata_t *mq);
uint8_t queue_items(struct qdata_t *mq);
