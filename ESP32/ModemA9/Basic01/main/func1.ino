void modem_exec_operation()
{
    bool drecv;
    bool operation_update = false;
    
    drecv = modem_rx_read(modem_rx_buff, 127);
    
    if (drecv) {
        DBG.println(modem_rx_buff);  
    }

    /*
        Handle CME or CMS Error
    */

    
    if (mymodem.xoperation_running) {
        if (mymodem.xwait_resp && drecv) {

            if (strstr(modem_rx_buff, modem_expect_response)) {
                operation_update = true;  
            } else {
                if (mymodem.operation_cur == MODEM_INIT) {
                    if (strstr(modem_rx_buff, "+CREG: 1,5")) {
                        operation_update = true;  
                    }  
                } 
            }        
            
            if (operation_update) {
              
                mymodem.operation_sub_state++;
                //PROCESS OPERATIONS
                if (mymodem.operation_cur == MODEM_INIT) {
                    modem_exec_init(&mymodem);  
                }
                else if (mymodem.operation_cur == MODEM_NETWORK_INIT) {
                    modem_exec_network_init(&mymodem);
                }
                else if (mymodem.operation_cur == MODEM_NETWORK_CHECK) {
                    modem_exec_network_check(&mymodem);
                }
                else if (mymodem.operation_cur == MODEM_GPRS_INIT) {
                    modem_exec_gprs_init(&mymodem);
                }
                else if (mymodem.operation_cur == MODEM_GPRS_CHECK) {
                    modem_exec_gprs_check(&mymodem);
                }
                else if (mymodem.operation_cur == MODEM_TCP_CONNECT) {
                    modem_exec_tcp_connect(&mymodem);
                }
                else if (mymodem.operation_cur == MODEM_TCP_CHECK) {
                    modem_exec_tcp_check(&mymodem);
                }              
                else if (mymodem.operation_cur == MODEM_TCP_DISCONNECT) {
                    modem_exec_tcp_disconnect(&mymodem);  
                }
            }     
        }

        //timeout running
        if (mymodem.xoperation_tout) {
            mymodem.operation_tout--;
            if (!mymodem.operation_tout) {
                mymodem.xoperation_tout = false;
                mymodem.xoperation_done = true;  
            }  
        }

        if (mymodem.xoperation_done) {  //operation done?
            if (mymodem.operation_cur == MODEM_INIT) {  //  MODEM INIT
                if (mymodem.xinit) {
                    DBG.println("MODEM INIT OK");
                    mymodem.operation_sub_state = 0;
                    mymodem.operation_cur = MODEM_STANDBY;
                    modem_exec_gprs_init(&mymodem);      
                } else {  // repeat
                    mymodem.minit_cnt++;
                    if (mymodem.minit_cnt > 2) {
                        DBG.println("MODEM INIT 3 FAIL");
                        DBG.println("MODEM BLOCKED");
                        mymodem.operation_cur = MODEM_BLOCKED; 
                    } else {
                        DBG.println("MODEM INIT REPEAT");
                        mymodem.operation_sub_state = 0;
                        modem_exec_init(&mymodem);  
                    }
                }
            }
            else if (mymodem.operation_cur == MODEM_GPRS_INIT) {  //  MODEM GPRS  INIT
                if (mymodem.xgprs) {
                    mymodem.operation_cur = MODEM_STANDBY;  
                } else {
                    mymodem.mgprs_cnt++;
                    if (mymodem.mgprs_cnt > 2) {
                        DBG.println("MODEM GPRS INIT 3 FAIL");
                        DBG.println("MODEM INIT START");  
                        mymodem.xinit = false;
                        mymodem.operation_sub_state = 0;
                        modem_exec_init(&mymodem);
                    } else {
                        DBG.println("MODEM GPRS INIT REPEAT");
                        mymodem.operation_sub_state = 0;
                        modem_exec_init(&mymodem);  
                    }  
                }
            }
        }
        
    }
}

void sys_soft_timers()
{
    uint32_t ms;
    
    ms = millis();
    tmr1_soft_diff = diff_time(ms, tmr1_soft_cur);
    if (tmr1_soft_diff > TMR1_SOFT_PERIOD) {
        tmr1_soft_cur = ms;
        ms = tmr1_soft_diff - TMR1_SOFT_PERIOD;
        tmr1_soft_cur = tmr1_soft_cur - ms;
        --tmr1_soft_secs;
        --tmr1_soft_g1;
        --tmr1_soft_sensor;
        --tmr1_soft_modem;
    }

    if (!tmr1_soft_g1) {
        xtmr1_soft_g1 = true;
        tmr1_soft_g1 = 1;   //reload
    }
    
    if (!tmr1_soft_sensor) {
      DBG.println("sampling sensor");
      tmr1_soft_sensor = SENSOR_PERIOD;  
    }

    if (!tmr1_soft_modem) {
      DBG.println("checking modem");
      tmr1_soft_modem = MODEM_PERIOD;  
    }
     
}

void sys_soft_irq()
{
    if (xtmr1_soft_g1) {
        modem_exec_operation();
        xtmr1_soft_g1 = false;  
    }  
}
