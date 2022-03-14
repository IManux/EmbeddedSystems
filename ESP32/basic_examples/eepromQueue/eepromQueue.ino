/*
    struct queue ex1
*/
#include <EEPROM.h>

#define DBG Serial
#define MDATA_TEMPLATE mdata_t

//EEPROM & QUEUE
#define EEPROM_MEM_OFFSET       10
#define EEPROM_MEM_SIZE        256

struct mdata_t
{
    uint8_t var1;
    uint8_t var2;
    uint8_t var3;
    uint8_t var4;
    uint32_t tm;    //time total in seconds
};

//Queue
struct qeeprom_t
{
  uint8_t qhead;
  uint8_t qtail;
  uint8_t qmax;
  uint8_t dsize;
  uint8_t xring;
  uint8_t offset;
};

struct qeeprom_t *mqdata;

//queue functions
struct qeeprom_t* qee_init(uint8_t n);
uint8_t qee_destroy(struct qeeprom_t *mq);
void qee_disable_ring(struct qeeprom_t *mq);
void qee_enable_ring(struct qeeprom_t *mq);
uint8_t qee_add(struct qeeprom_t *mq, MDATA_TEMPLATE *md);
uint8_t qee_pop(struct qeeprom_t *mq, MDATA_TEMPLATE *md);
uint8_t qee_npop(struct qeeprom_t *mq, MDATA_TEMPLATE *md);
uint8_t qee_full(struct qeeprom_t *mq);
uint8_t qee_empty(struct qeeprom_t *mq);
uint8_t qee_items(struct qeeprom_t *mq);

void setup()
{
    DBG.begin(9600);
    DBG.println("eepromQueue");
    
    mqdata = qee_init(20);
    qee_update_internal(mqdata);
    
    if (mqdata == NULL) {
        DBG.println("mqdata init fail");
        while (true) {
            delay(1000);  
        }  
    }
    
}

void loop()
{
    int n;
    n = DBG.available();

    if (n) {
        char c;
        c = DBG.read();

        if (c == 'a') { // add
                
        }

        if (c == 'n') { // get qty
          
        }

        if (c == 'g') { // get one 
          
        }

        if (c == 'c') { // check queue
            DBG.print("qhead: ");
            DBG.println(mqdata->qhead);
            DBG.print("qtail: ");
            DBG.println(mqdata->qtail);
            DBG.print("qmax: ");
            DBG.println(mqdata->qmax);
            DBG.print("dsize: ");
            DBG.println(mqdata->dsize);
        }
    }
    
}

void qee_update_internal(struct qeeprom_t *q)
{
    uint8_t btmp, btail, bhead, bcrc, r;

    r = EEPROM.read(1);
    //get
    btail = EEPROM.read(2);
    bhead = EEPROM.read(3);
    bcrc = EEPROM.read(4);

    if (r == 200) {
        btmp = btail + bhead;
        if (btmp == bcrc) {
            q->qtail = btail;
            q->qhead = bhead;  
        }     
    }
}

void qee_store_internal(struct qeeprom_t *q)
{
    uint8_t bcrc;
    //store head, tail, crc
    bcrc = q->qtail + q->qhead;
    EEPROM.write(2, q->qtail);
    EEPROM.write(3, q->qhead);
    EEPROM.write(4, bcrc);
    EEPROM.write(1, 200);
}

struct qeeprom_t* qee_init(uint8_t n)
{
    struct qeeprom_t *mq = NULL;
    
    if (n) {
        mq = (struct qeeprom_t*) malloc(sizeof(struct qeeprom_t));
        if (mq != NULL) {
            mq->dsize = sizeof(MDATA_TEMPLATE);
            mq->qmax = (EEPROM_MEM_SIZE - EEPROM_MEM_OFFSET) / mq->dsize;
            mq->qtail = 0;
            mq->qhead = 0;
            mq->xring = 0;
            if (n <= mq->qmax) {
                mq->qmax = n;  
            }
        }
    }

    return mq;
}

uint8_t qee_destroy(struct qeeprom_t *mq)
{
    if (mq == NULL) {
        return 0;  
    }
    
    free(mq);
    
    return 1;
}

uint8_t qee_add(struct qeeprom_t *mq, MDATA_TEMPLATE *md)
{
    int8_t idx;
    MDATA_TEMPLATE *pmq;
    
    idx = (mq->qhead + 1) % mq->qmax;
    
    if (idx == mq->qtail) {  //Queue Full?
        if (mq->xring) {  //ring buffer enable?
            mq->qtail = (mq->qtail + 1) % mq->qmax;   //yes, overwrite buffer
        } else {
            return 0;
        }
    }

    //store in eeprom
    uint8_t *p;
    uint8_t bsz;
    uint16_t bidx;
    
    p = (uint8_t*) md;
    bidx = EEPROM_MEM_OFFSET + (mq->dsize * mq->qhead);
    bsz = mq->dsize;
    
    for (uint8_t i = 0; i < bsz; i++) {
        EEPROM.write(bidx, p[i]);  
        ++bidx;
    }
    qee_store_internal(mq);
    mq->qhead = idx;
    qee_store_internal(mq);
    
    return 1;  
}

void qee_disable_ring(struct qeeprom_t *mq)
{
    if (mq != NULL) {
        mq->xring = 0;  
    }  
}

void qee_enable_ring(struct qeeprom_t *mq)
{
    if (mq != NULL) {
        mq->xring = 1;  
    }  
}

uint8_t qee_pop(struct qeeprom_t *mq, MDATA_TEMPLATE *md)
{
    if (mq->qhead == mq->qtail) {
        return 0;  
    }
    
    MDATA_TEMPLATE *pmq;
    uint8_t *p;
    uint8_t bsz;
    uint16_t bidx;
    
    p = (uint8_t*)pmq;
    bsz = mq->dsize;
    bidx = EEPROM_MEM_OFFSET + (bsz * mq->qtail);
    for (uint8_t i = 0; i < bsz; i++) {
        p[i] = EEPROM.read(bidx);
        ++bidx;  
    }
    memcpy(md, pmq, bsz);
    mq->qtail = (mq->qtail + 1) % mq->qmax;
    qee_store_internal(mq);
    
    return 1;  
}

uint8_t qee_peak(struct qeeprom_t *mq, MDATA_TEMPLATE *md)
{
    if (mq->qhead == mq->qtail) {
        return 0;  
    }
    
    MDATA_TEMPLATE *pmq;
    uint8_t *p;
    uint8_t bsz;
    uint16_t bidx;
    
    p = (uint8_t*)pmq;
    bsz = mq->dsize;
    bidx = EEPROM_MEM_OFFSET + (bsz * mq->qtail);
    for (uint8_t i = 0; i < bsz; i++) {
        p[i] = EEPROM.read(bidx);
        ++bidx;  
    }
    memcpy(md, pmq, bsz);
    
    return 1;  
}

uint8_t qee_full(struct qeeprom_t *mq)
{
    uint8_t idx;
    idx = (mq->qhead + 1) % mq->qmax;
    if (idx == mq->qtail) {
        return 1;  
    }
    return 0;  
}

uint8_t qee_empty(struct qeeprom_t *mq)
{
    if (mq->qhead == mq->qtail) {
        return 1;  
    }
    return 0;  
}
 
uint8_t qee_items(struct qeeprom_t *mq)
{
    uint8_t n;
    n = (mq->qmax  + mq->qhead - mq->qtail) % mq->qmax;
    return n;
}
