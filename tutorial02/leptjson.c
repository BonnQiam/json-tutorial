#include "leptjson.h"
#include <assert.h>  /* assert() */
#include <stdlib.h>  /* NULL, strtod() */

#include <errno.h>   /* errno, ERANGE */
#include <math.h>    /* HUGE_VAL */

#define EXPECT(c, ch)       do { assert(*c->json == (ch)); c->json++; } while(0)

#define ISDIGIT(ch)         ((ch) >= '0' && (ch) <= '9')
#define ISDIGIT1TO9(ch)     ((ch) >= '1' && (ch) <= '9')

typedef struct {
    const char* json;
}lept_context;

static void lept_parse_whitespace(lept_context* c) {
    const char *p = c->json;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
        p++;
    c->json = p;
}

static int lept_parse_true(lept_context* c, lept_value* v) {
    EXPECT(c, 't');
    if (c->json[0] != 'r' || c->json[1] != 'u' || c->json[2] != 'e')
        return LEPT_PARSE_INVALID_VALUE;
    c->json += 3;
    v->type = LEPT_TRUE;
    return LEPT_PARSE_OK;
}

static int lept_parse_false(lept_context* c, lept_value* v) {
    EXPECT(c, 'f');
    if (c->json[0] != 'a' || c->json[1] != 'l' || c->json[2] != 's' || c->json[3] != 'e')
        return LEPT_PARSE_INVALID_VALUE;
    c->json += 4;
    v->type = LEPT_FALSE;
    return LEPT_PARSE_OK;
}

static int lept_parse_null(lept_context* c, lept_value* v) {
    EXPECT(c, 'n');
    if (c->json[0] != 'u' || c->json[1] != 'l' || c->json[2] != 'l')
        return LEPT_PARSE_INVALID_VALUE;
    c->json += 3;
    v->type = LEPT_NULL;
    return LEPT_PARSE_OK;
}

static int lept_parse_literal(lept_context* c, lept_value* v){
    
}

static int lept_parse_number(lept_context* c, lept_value* v) {
    char* end;

    if(
        (*c->json != '-') && (*c->json != '0') && (*c->json != '1') && (*c->json != '2') && (*c->json != '3') &&
        (*c->json != '4') && (*c->json != '5') && (*c->json != '6') && (*c->json != '7') && (*c->json != '8') && 
        (*c->json != '9')
    )
        return LEPT_PARSE_INVALID_VALUE;
    if(
        ( *c->json == '0')          && 
        ( *(c->json +1) != '\0')    &&
        ( *(c->json +1) != '.')     &&
        ( *(c->json +1) != 'E')     &&
        ( *(c->json +1) != 'e')
    ) 
        return LEPT_PARSE_ROOT_NOT_SINGULAR;

    v->n = strtod(c->json, &end);
    if (
        (c->json == end) || 
        (*(end-1) == '.')
    )
        return LEPT_PARSE_INVALID_VALUE;
    c->json = end;
    v->type = LEPT_NUMBER;
    return LEPT_PARSE_OK;
}

/*
static int lept_parse_number(lept_context* c, lept_value* v) {
    const char* p = c->json;
    if (*p == '-') 
        p++;//若当前字符为负数，则移动至下一个字符

    if (*p == '0') 
        p++;//若当前字符为 0，则移动至下一个字符
    else {
        if (!ISDIGIT1TO9(*p)) 
            return LEPT_PARSE_INVALID_VALUE;//若当前字符不为零，则检查当前字符是否 '1' - '9' 的合法数字
        for (p++; ISDIGIT(*p); p++);// 持续移动字符至非法字符处，合理的话应该是小数点或 'e' 或 'E'
    }

    if (*p == '.') {//若当前字符为小数点
        p++;//移动至下一个字符
        if (!ISDIGIT(*p)) return LEPT_PARSE_INVALID_VALUE;//检查当前字符是否 '1' - '9' 的合法数字
        for (p++; ISDIGIT(*p); p++);//持续移动字符至非法字符处
    }
    if (*p == 'e' || *p == 'E') {//若当前字符为 'e'、'E'
        p++;
        if (*p == '+' || *p == '-') 
            p++;
        if (!ISDIGIT(*p)) return LEPT_PARSE_INVALID_VALUE;
        for (p++; ISDIGIT(*p); p++);
    }

    errno = 0;
    v->n = strtod(c->json, NULL);
    if (errno == ERANGE && (v->n == HUGE_VAL || v->n == -HUGE_VAL))
        return LEPT_PARSE_NUMBER_TOO_BIG;
    v->type = LEPT_NUMBER;
    c->json = p;
    return LEPT_PARSE_OK;
}
*/


static int lept_parse_value(lept_context* c, lept_value* v) {
    switch (*c->json) {
        case 't':  return lept_parse_true(c, v);
        case 'f':  return lept_parse_false(c, v);
        case 'n':  return lept_parse_null(c, v);
        default:   return lept_parse_number(c, v);
        case '\0': return LEPT_PARSE_EXPECT_VALUE;
    }
}

int lept_parse(lept_value* v, const char* json) {
    lept_context c;
    int ret;
    assert(v != NULL);
    c.json = json;
    v->type = LEPT_NULL;
    lept_parse_whitespace(&c);
    if ((ret = lept_parse_value(&c, v)) == LEPT_PARSE_OK) {
        lept_parse_whitespace(&c);
        if (*c.json != '\0') {
            v->type = LEPT_NULL;
            ret = LEPT_PARSE_ROOT_NOT_SINGULAR;
        }
    }
    return ret;
}

// 获取 json 数字
lept_type lept_get_type(const lept_value* v) {
    assert(v != NULL);
    return v->type;
}

double lept_get_number(const lept_value* v) {
    assert(v != NULL && v->type == LEPT_NUMBER);
    return v->n;
}
