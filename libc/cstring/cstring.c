/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <string.h>
#include <ctype.h>
#include "cstring.h"

/*
 INIT & DESTROY
 */
CString* cstring_init()
{
    CString * cstr;
    
    cstr = (CString*)malloc(sizeof(CString));
    if (cstr == NULL) {
        return NULL;
    }
    cstr->sz = 0;
    cstr->capacity = 0;
    cstr->str = NULL;
    
    return cstr;
}

cstring_err cstring_destroy(CString * src)
{
    if (src == NULL) {
        return CSTR_NULLPARAM_ERR;
    }
    
    if (src->str != NULL) {
        free(src->str);
    }
    free(src);
    
    return CSTR_NO_ERR;
}

cstring_err cstring_reserve(CString * dest, size_t n)
{
    if (dest == NULL) {
        return CSTR_NULLPARAM_ERR;
    }
    if (n == 0) {
        return CSTR_NULLSTR_ERR;
    }
    
    char *tmp;
    if (dest->str == NULL) {
        tmp = (char*)calloc(1, (n + 1));
    } else {
        tmp = (char*)realloc(dest->str, (n + 1));
    }
    
    if (tmp == NULL) {
        return CSTR_ALLOC_ERR;
    }
    
    dest->str = tmp;
    dest->sz = 0;
    dest->capacity = n;
    
    return CSTR_NO_ERR;
}
/*
    PUBLIC FUNCTIONS
 */
cstring_err cstring_cat(CString * dest, CString * src)
{
    if (dest == NULL || src == NULL) {
        return CSTR_NULLPARAM_ERR;
    }
    if (src->str == NULL) {
        return CSTR_NULLSTR_ERR;
    }
    
    size_t dest_sz = dest->sz;
    size_t src_sz = src->sz;
    size_t n = dest_sz + src_sz;
    size_t capacity = dest->capacity;
    
    char* tmp;
    if (dest->str == NULL) {
        tmp = (char*)calloc(1, (n + 1));
    } else {
        tmp = dest->str;
        if (n > capacity) {
            tmp = (char*)realloc(dest->str, (n + 1));
        }
    }
    if (tmp == NULL) {
        return CSTR_ALLOC_ERR;
    }
    
    dest->str = tmp;
    dest->sz = n;
    if (n > capacity) {
        dest->capacity = n;
    }
    //dest->capacity = n;
    //Copy
    strncpy(&dest->str[dest_sz], src->str, src_sz);
    dest->str[n] = 0;
    
    return CSTR_NO_ERR;    
}

cstring_err cstring_strcat(CString * dest, char * src)
{
    if (dest == NULL) {
        return CSTR_NULLPARAM_ERR;
    }
    if (src == NULL) {
        return CSTR_NULLSTR_ERR;
    }
    
    size_t dest_sz = dest->sz;
    size_t src_sz = strlen(src);
    size_t n = dest_sz + src_sz;
    size_t capacity = dest->capacity;
    
    char* tmp;
    if (dest->str == NULL) {
        tmp = (char*)calloc(1, (n + 1));
    } else {
        tmp = dest->str;
        if (n > capacity) {
            tmp = (char*)realloc(dest->str, (n + 1));
        }
    }
    
    if (tmp == NULL) {
        return CSTR_ALLOC_ERR;
    }
    
    dest->str = tmp;
    dest->sz = n;
    if (n > capacity) {
        dest->capacity = n;
    }
    //Copy
    strncpy(&dest->str[dest_sz], src, src_sz);
    dest->str[n] = 0;
    
    return CSTR_NO_ERR;
}

cstring_err cstring_chcat(CString * dest, const char src)
{
    if (dest == NULL) {
        return CSTR_NULLPARAM_ERR;
    }
    
    size_t n = dest->sz + 1;
    size_t capacity = dest->capacity;
    
    char *tmp;
    if (dest->str == NULL) {
        tmp = (char*)calloc(1, (n + 1));
    } else {
        tmp = dest->str;
        if (n > capacity) {
            tmp = (char*)realloc(dest->str, (n + 1));
        }
    }
    if (tmp == NULL) {
        return CSTR_ALLOC_ERR;
    }
    
    dest->str = tmp;
    dest->sz = n;
    if (n > capacity) {
        dest->capacity = n;
    }
    //Copy
    dest->str[n - 1] = src;
    dest->str[n] = 0;
    
    return CSTR_NO_ERR;
}

cstring_err cstring_update(CString * dest, CString * src)
{
    if (dest == NULL) {
        return CSTR_NULLPARAM_ERR;
    }
    if (src == NULL) {
        return CSTR_NULLSTR_ERR;
    }
    
    size_t n = src->sz;
    size_t capacity = dest->capacity;
    char *tmp;
    
    if (dest->str == NULL) {
        tmp = (char*)calloc(1, (n + 1));
    } else {
        tmp = dest->str;
        if (n > capacity) {
            tmp = (char*)realloc(dest->str, (n + 1));
        }
    }
    
    dest->str = tmp;
    dest->sz = n;
    if (n > capacity) {
        dest->capacity = n;
    }
    //Copy
    strncpy(&dest->str[0], src->str, n);
    dest->str[n] = 0;
    
    return CSTR_NO_ERR;
    
}

cstring_err cstring_strupdate(CString * dest, char * src)
{
    if (dest == NULL || src == NULL) {
        return CSTR_NULLPARAM_ERR;
    }
    if (src == NULL) {
        return CSTR_NULLSTR_ERR;
    }
    
    size_t n = strlen(src);
    size_t capacity = dest->capacity;
    char *tmp;
    
    if (dest->str == NULL) {
        tmp = (char*)calloc(1, (n + 1));
    } else {
        tmp = dest->str;
        if (n > capacity) {
            tmp = (char*)realloc(dest->str, (n + 1));
        }
    }
    
    dest->str = tmp;
    dest->sz = n;
    if (n > capacity) {
        dest->capacity = n;
    }
    //Copy
    strncpy(&dest->str[0], src, n);
    dest->str[n] = 0;
    
    return CSTR_NO_ERR;
}

cstring_err cstring_get_size(CString * dest, size_t n)
{
    if (dest == NULL) {
        return CSTR_NULLPARAM_ERR;
    }
    return dest->sz;
}
