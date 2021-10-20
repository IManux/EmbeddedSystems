/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   cstring.h
 * Author: manux
 *
 * Created on October 10, 2021, 1:11 PM
 */
#ifndef CSTRING_H
#define CSTRING_H

#include <stdlib.h>

/* Possible errors */
#define CSTR_NO_ERR                   0
#define CSTR_ALLOC_ERR               -1
#define CSTR_NULLPARAM_ERR           -2
#define CSTR_NULLSTR_ERR             -3
#define CSTR_INIT_ERR                -4 //STRUCTINIT
#define CSTR_NMSTR_ERR               -5 // NM = NO MATCH

#ifdef __cplusplus
extern "C" {
#endif
    
    typedef struct CString CString;
    typedef int cstring_err;
    
    struct CString
    {
        size_t sz;
        size_t capacity;
        //int err;
        char *str;
    };
    
    
    /*
     Init & Destroy
     */
    CString* cstring_init();
    cstring_err cstring_destroy(CString * src);
    /*
        Public Functions
     */
    cstring_err cstring_reserve(CString * dest, size_t n);
    cstring_err cstring_cat(CString * dest, CString * src);
    cstring_err cstring_strcat(CString * dest, char * src);
    cstring_err cstring_update(CString * dest, CString * src);
    cstring_err cstring_strupdate(CString * dest, char * src);
    cstring_err cstring_chcat(CString * dest, const char src);
    cstring_err cstring_get_size(CString * dest, size_t n);
    
#ifdef __cplusplus
}
#endif

#endif /* CSTRING_H */