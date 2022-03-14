void modem_set_response(char *res, uint8_t n)
{
    if (n == 0) {   //response OK
        strcpy(res, "OK");      
    }
}

uint8_t modem_rx_read(char *buff, uint8_t sz)
{
    uint8_t tout;
    uint8_t idx;
    uint8_t n;
    uint8_t err;

    tout = 5;
    err = 0;
    n = UART2.available();
    if (n > 0) {
        while (tout > 0) {
            n = UART2.available();
            while (n > 0) {
                buff[idx] = UART2.read();
                idx++;
                sz--;
                if (!sz) {
                    n = 0;
                    tout = 1;  
                }  
            }
            tout--;
            delay(10);  
        }
        buff[idx] = '\0';
        err = 1;     
    }

    return err;
}

uint32_t diff_time(uint32_t t1, uint32_t t2)
{
    if (t1 > t2) {
      return (t1 - t2);
    }

    return (t2 - t1);
}
