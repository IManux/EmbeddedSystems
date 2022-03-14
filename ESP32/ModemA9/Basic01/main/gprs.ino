void modem_exec_gprs_init(struct modem_t *m)
{
  
    if (m->operation_sub_state == 0) {   //AT+CGATT=1
        DBG.println("AT+CGATT=1");
        UART2.println("AT+CGATT=1");
        strcpy(modem_expect_response, ok_reply);
        m->response_tout = 100;
        m->xwait_resp = true;
        m->xoperation_tout = true;
        m->mgprs_cnt = 0;
    }
    else if (m->operation_sub_state == 1) {  //AT+CSTT="apn","user","pass"
        DBG.println("AT+CSTT=\"mavoco\",\"\",\"\"");
        UART2.println("AT+CSTT=\"mavoco\",\"\",\"\"");
        strcpy(modem_expect_response, ok_reply);
        m->response_tout = 100;
        m->xwait_resp = true;
    }
    else if (m->operation_sub_state == 2) {  //AT+CGDCONT
        DBG.println("AT+CGDCONT=1,\"IP\",\"mavoco\"");
        UART2.println("AT+CGDCONT=1,\"IP\",\"mavoco\"");
        strcpy(modem_expect_response, ok_reply);
        m->response_tout = 100;
        m->xwait_resp = true;
    }
    else if (m->operation_sub_state == 3) {  //AT_CIICR
        DBG.println("AT+CIICR");
        UART2.println("AT+CIICR");
        strcpy(modem_expect_response, ok_reply);
        m->response_tout = 100;
        m->xwait_resp = true;         
    }
    else if (m->operation_sub_state == 4) {  //AT+CIFSR
        DBG.println("AT+CIFSR");
        UART2.println("AT+CIFSR");
        strcpy(modem_expect_response, ok_reply);
        m->response_tout = 100;
        m->xwait_resp = true;   
    }
    else if(m->operation_sub_state == 5) {
        m->xgprs = true;
        m->xwait_resp = false;
        m->xoperation_done = true;
        m->xoperation_tout = false;
        m->operation_sub_state++; 
    }

}

void modem_exec_gprs_check(struct modem_t *m)
{
    
}
