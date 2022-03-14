struct qdata_t* queue_init(uint8_t n)
{
    struct qdata_t *mq = NULL;
    
    if (n) {
        mq = (struct qdata_t*)malloc(sizeof(struct qdata_t));
        if (mq != NULL) {
            mq->qbuff = (struct mdata_t*)malloc((sizeof(struct mdata_t) * n));
            if (mq->qbuff != NULL) {
                mq->qhead = 0;
                mq->qtail = 0;
                mq->qmax = n + 1;
                mq->dsize = sizeof(struct mdata_t);
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

void queue_disable_ring(struct qdata_t *mq)
{
    if (mq != NULL) {
        mq->xring = 0;  
    }  
}

void queue_enable_ring(struct qdata_t *mq)
{
    if (mq != NULL) {
        mq->xring = 1;  
    }  
}

uint8_t queue_add(struct qdata_t *mq, struct mdata_t *md)
{
    int8_t idx;
    struct mdata_t *pmq;
    
    idx = (mq->qhead + 1) % mq->qmax;
    
    if (idx == mq->qtail) {  //Queue Full?
        if (mq->xring) {  //ring buffer enable?
            mq->qtail = (mq->qtail + 1) % mq->qmax;   //yes, overwrite buffer
        } else {
            return 0;
        }
    }
    pmq = &mq->qbuff[mq->qhead];
    memcpy(pmq, md, mq->dsize);
    mq->qhead = idx;
    
    return 1;  
}

uint8_t queue_pop(struct qdata_t *mq, struct mdata_t *md)
{
    if (mq->qhead == mq->qtail) {
        return 0;  
    }
    
    struct mdata_t *pmq;
    pmq = &mq->qbuff[mq->qtail];
    //copy
    memcpy(md, pmq, mq->dsize);
    mq->qtail = (mq->qtail + 1) % mq->qmax;
    
    return 1;  
}


uint8_t queue_npop(struct qdata_t *mq, struct mdata_t *md)
{
    if (mq->qhead == mq->qtail) {
        return 0;  
    }
    
    struct mdata_t *pmq;
    pmq = &mq->qbuff[mq->qtail];
    //copy
    memcpy(md, pmq, mq->dsize);
    
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
