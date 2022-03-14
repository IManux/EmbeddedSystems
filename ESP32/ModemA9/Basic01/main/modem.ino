/*
    COMMANDS
    Turn off echo --> AT&FZE0 - OK
    AT+RST=1, OK
    AT+CMER=3,0,0,2 - OK
    Get network status --> AT+CREG? - 1 or 5
    Get RSSI --> AT+CSQ - +CSQ: XX
    AT+CLTS=1
    AT+CLTS=0
    AT+CCLK?
    Gprs state --> AT+CGATT? - +CGATT: XX
    
    2. GPRS ON
    AT+CGATT=1 - OK, Attached gprs
    AT+CSTT="apnname","username","password"
    AT+CGDCONT=1,"IP","apnname"
    Bring up Wireless connection --> AT+CIICR - OK
    AT+CIFSR - OK
    
    3. GPRS OFF
    AT+CIPSHUT - SHUT OK or OK
    AT+CGATT=0 - OK, Detached gprs

    4. TCP Connect

    5. TCP Close

    6. TCP commands
    AT+CIPSTATUS
    AT+CIPRXGET=4
    AT+CIPRXGET=2

    Reference:
    - https://ai-thinker-open.github.io/GPRS-AT/
    - https://wiki.dfrobot.com/A9G_Module_SKU_TEL0134
    - https://docs.ai-thinker.com/en/gprs%E6%A8%A1%E7%BB%84%E4%B8%93%E9%A2%982
    - https://www.smssolutions.net/tutorials/gsm/gsmerrorcodes/
    
*/
bool modem_exec_init(struct modem_t *m)
{
    //states of operation
    if (m->operation_sub_state == 0 || m->operation_sub_state == 1 || m->operation_sub_state == 2) {
        DBG.println("AT");
        UART2.println("AT");
        strcpy(modem_expect_response, ok_reply);
        m->response_tout = 10;
        m->xwait_resp = true;
        m->xoperation_tout = true;
        m->minit_cnt = 0;
    }
    else if (m->operation_sub_state == 3) {
        DBG.println("AT&FZE0");
        UART2.println("AT&FZE0");
        strcpy(modem_expect_response, ok_reply);
        m->response_tout = 10;
        m->xwait_resp = true;
    }
    else if (m->operation_sub_state == 4) {
        DBG.println("AT+CMER=3,0,0,2");
        UART2.println("AT+CMER=3,0,0,2");
        strcpy(modem_expect_response, ok_reply);
        m->response_tout = 10;
        m->xwait_resp = true;
    }
    else if(m->operation_sub_state == 5) {
        m->xinit = true;
        m->xwait_resp = false;
        m->xoperation_done = true;
        m->xoperation_tout = false;
        m->operation_sub_state++;
    }

    return true;
}

void modem_exec_network_init(struct modem_t *m)
{
  
}

void modem_exec_network_check(struct modem_t *m)
{
    //states of operation    
    if (m->operation_sub_state == 0) {   //AT+CIPSTART="TCP","SERVER","PORT"
        DBG.println("AT+CREG?");
        UART2.println("AT+CREG?");
        strcpy(modem_expect_response, "CONNECT OK");
        m->response_tout = 100;
        m->xwait_resp = true;  
        m->xoperation_tout = true;
        m->mnetwork_cnt = 0;
    }
    else if(m->operation_sub_state == 1) {
        m->xnetwork = true;
        m->xwait_resp = false;
        m->xoperation_done = true;
        m->xoperation_tout = false;
        m->operation_sub_state++;   
    }     
}
