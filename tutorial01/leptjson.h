#ifndef LEPTJSON_H__
#define LEPTJSON_H__

typedef enum { LEPT_NULL, LEPT_FALSE, LEPT_TRUE, LEPT_NUMBER, LEPT_STRING, LEPT_ARRAY, LEPT_OBJECT } lept_type;

typedef struct {
    lept_type type;
}lept_value;

// 解析 JSON 的返回值
enum {
    LEPT_PARSE_OK = 0,           // 无错误
    LEPT_PARSE_EXPECT_VALUE,     // JSON 只含有空白
    LEPT_PARSE_INVALID_VALUE,    // JSON 含有子集中没有定义的字面值
    LEPT_PARSE_ROOT_NOT_SINGULAR // JSON 在一个值之后，在空白之后还有其他字
};

int lept_parse(lept_value* v, const char* json);// 解析 JSON 

lept_type lept_get_type(const lept_value* v); // 访问结果，此处功能仅仅为获取类型

#endif /* LEPTJSON_H__ */
