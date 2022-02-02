/*
    basic queue ex1
*/
#define DBG Serial 
#define QDATA_BUFF_SIZE 5

//Queue
struct qdata_t
{
  uint8_t qhead;
  uint8_t qtail;
  uint8_t qmax;
  uint8_t xring;
  uint8_t *qbuff;
};

struct qdata_t *mqdata;

//Global
uint8_t dtmp;

//queue functions
struct qdata_t* queue_init(uint8_t n);
uint8_t queue_destroy(struct qdata_t *mq);
void queue_enable_ring(struct qdata_t *mq);
void queue_disable_ring(struct qdata_t *mq);
uint8_t queue_add(struct qdata_t *mq, uint8_t md);
uint8_t queue_pop(struct qdata_t *mq, uint8_t *md);
uint8_t queue_npop(struct qdata_t *mq, uint8_t *md);
uint8_t queue_full(struct qdata_t *mq);
uint8_t queue_empty(struct qdata_t *mq);
uint8_t queue_items(struct qdata_t *mq);

void setup()
{
    DBG.begin(9600);
    DBG.println("basicQueue");
    
    mqdata = queue_init(QDATA_BUFF_SIZE);
    if (mqdata == NULL) {
        DBG.println("mqdata init fail");
        while (true) {
            delay(1000);  
        }  
    }
    dtmp = 0;
}

void loop()
{
    int n;
    n = DBG.available();

    if (n) {
        char c;
        c = DBG.read();

        if (c == 'a') {   //add data
            dtmp = dtmp + 3;
            if (!queue_add(mqdata, dtmp)) {
                DBG.println("queue is full");  
            }
        }

        if (c == 'b') {   //get data
          if (!queue_empty(mqdata)) {
              uint8_t dtmp;
              queue_pop(mqdata, &dtmp);
              DBG.print("data: ");
              DBG.println(dtmp);
          } else {
              DBG.println("queue is empty");  
          }      
        }

        if (c == 'c') {   //get num items in queue
            int j;
            j = queue_items(mqdata);
            DBG.print("num items: ");
            DBG.println(j);  
        }

        if (c == 'd') {   //disable ring buffer
            queue_disable_ring(mqdata);  
        }
        if (c == 'e') {   //enable ring buffer
            queue_enable_ring(mqdata);  
        }

    }
    
}

struct qdata_t* queue_init(uint8_t n)
{
    struct qdata_t *mq = NULL;
    
    if (n) {
        mq = (struct qdata_t*)malloc(sizeof(struct qdata_t));
        if (mq != NULL) {
            mq->qbuff = (uint8_t*)malloc((sizeof(uint8_t) * n));
            if (mq->qbuff != NULL) {
                mq->qhead = 0;
                mq->qtail = 0;
                mq->qmax = n + 1;
                mq->xring = 0;
            } else {
                free(mq);
                mq = NULL;  
            }
        }
    }

    return mq;
}

uint8_t queue_destroy(struct qdata_t *mq)
{
    if (mq == NULL) {
        return 0;  
    }
    
    if (mq->qbuff != NULL) {
        free(mq->qbuff);  
    }
    free(mq);
    
    return 1;
}

void queue_enable_ring(struct qdata_t *mq)
{
    if (mq != NULL) {
        mq->xring = 1;  
    }  
}

void queue_disable_ring(struct qdata_t *mq)
{
    if (mq != NULL) {
        mq->xring = 0;  
    }  
}

uint8_t queue_add(struct qdata_t *mq, uint8_t md)
{
    int8_t idx;
    
    idx = (mq->qhead + 1) % mq->qmax;
    
    if (idx == mq->qtail) {  //Queue Full?
        if (mq->xring) {  //ring buffer enable?
            mq->qtail = (mq->qtail + 1) % mq->qmax;   //yes, overwrite buffer
        } else {
            return 0;
        }
    }

    mq->qbuff[mq->qhead] = md;
    mq->qhead = idx;
    
    return 1;  
}

uint8_t queue_pop(struct qdata_t *mq, uint8_t *md)
{
    if (mq->qhead == mq->qtail) {
        return 0;  
    }
    
    *md = mq->qbuff[mq->qtail];
    mq->qtail = (mq->qtail + 1) % mq->qmax;
    
    return 1;  
}


uint8_t queue_npop(struct qdata_t *mq, uint8_t *md)
{
    if (mq->qhead == mq->qtail) {
        return 0;  
    }
    
    *md = mq->qbuff[mq->qtail];
    
    return 1;  
}

uint8_t queue_full(struct qdata_t *mq)
{
    uint8_t idx;
    idx = (mq->qhead + 1) % mq->qmax;
    if (idx == mq->qtail) {
        return 1;  
    }
    return 0;  
}
  
uint8_t queue_empty(struct qdata_t *mq)
{
    if (mq->qhead == mq->qtail) {
        return 1;  
    }
    return 0;  
}
 
uint8_t queue_items(struct qdata_t *mq)
{
    uint8_t n;
    n = (mq->qmax  + mq->qhead - mq->qtail) % mq->qmax;
    return n;
}
