void modem_exec_tcp_connect(struct modem_t *m)
{
    //states of operation    
    if (m->operation_sub_state == 0) {   //AT+CIPSTART="TCP","SERVER","PORT"
        DBG.println("AT+CIPSTART=\"TCP\",\"SERVER\",\"PORT\"");
        UART2.println("AT+CIPSTART=\"TCP\",\"SERVER\",\"PORT\"");
        strcpy(modem_expect_response, "CONNECT OK");
        m->response_tout = 100;
        m->xwait_resp = true;  
        m->xoperation_tout = true;
        m->mtcp_cnt = 0;
    }
    else if(m->operation_sub_state == 1) {
        m->xtcp_connected = true;
        m->xwait_resp = false;
        m->xoperation_done = true;
        m->xoperation_tout = false;
        m->operation_sub_state++;   
    }
      
}

void modem_exec_tcp_disconnect(struct modem_t *m)
{
    //states of operation
    if (m->operation_sub_state == 0) {   //AT+CGATT=1
        DBG.println("AT+CIPCLOSE");
        UART2.println("AT+CIPCLOSE");
        strcpy(modem_expect_response, ok_reply);
        m->response_tout = 100;
        m->xwait_resp = true;
        m->xoperation_tout = true;
        m->mtcp_cnt = 0;
    }
    else if(m->operation_sub_state == 1) {
        m->xtcp_connected = false;
        m->xwait_resp = false;
        m->xoperation_done = true;
        m->xoperation_tout = false;
        m->operation_sub_state++;  
    }


}

void modem_exec_tcp_check(struct modem_t *m)
{
    

}
