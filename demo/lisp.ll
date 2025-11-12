int printf(char * fmg, ...);int scanf(char * format, ...);int getchar();int putchar(int  ch);int getint(){int val;scanf(%d,&val);return val;}int getch(){return getchar();}int getarray([-1]int * val){int len;for (int i = 0; i < len; i++) {scanf(%d,val[i]);};return len;}void putint(int  val){printf(%d,val);}void putch(int  val){putchar(val);}void putarray(int  len,[-1]int * arr){printf(%d:,len);for (int i = 0; i < len; i++) {printf( %d,arr[i]);};}int ERR_INVALID_DATA_TYPE = 0int ERR_BUFFER_OVERFLOW = 1int ERR_PARSE_ERROR = 2int ERR_SYMBOL_NOT_FOUND = 3int ERR_INVALID_LIST = 4int ERR_INVALID_FUNC = 5int ERR_INVALID_ARG_NUM = 6int ERR_TYPE_MISMATCH = 7int ERR_INVALID_SYMBOL = 8int panic(int  code){putch(112);putch(97);putch(110);putch(105);putch(99);putch(32);putint(code);putch(10);for (; 1; ) {};return code;}int DATA_TYPE = 0int DATA_VALUE = 1int DATA_NEXT = 2int DATA_REF_COUNT = 3int DATA_FIELD_COUNT = 4int MAX_DATA_LEN = 2048[2048][2048]int dataint free_data_ptr = 0int DATA_TYPE_SYMBOL = 0int DATA_TYPE_NUMBER = 1int DATA_TYPE_LIST = 2int DATA_TYPE_ENV = 3int DATA_TYPE_FUNC = 4void init_data(){data[0][DATA_NEXT] = 0;int i = 1;for (; i < MAX_DATA_LEN; ) {data[i][DATA_NEXT] = i - 1;i = i + 1;};free_data_ptr = MAX_DATA_LEN - 1;}int alloc_data(){if (!free_data_ptr)
panic(ERR_BUFFER_OVERFLOW);int i = free_data_ptr;free_data_ptr = data[i][DATA_NEXT];data[i][DATA_NEXT] = 0;data[i][DATA_REF_COUNT] = 1;return i;}void free_data(int  data_ptr){if (!data_ptr)
return ;int ref_count = data[data_ptr][DATA_REF_COUNT] - 1;data[data_ptr][DATA_REF_COUNT] = ref_count;if (ref_count > 0)
return ;if (data[data_ptr][DATA_TYPE] != DATA_TYPE_SYMBOL && data[data_ptr][DATA_TYPE] != DATA_TYPE_NUMBER)
{free_data(data[data_ptr][DATA_VALUE]);};free_data(data[data_ptr][DATA_NEXT]);data[data_ptr][DATA_NEXT] = free_data_ptr;free_data_ptr = data_ptr;}int copy_ptr(int  data_ptr){if (!data_ptr)
return 0;data[data_ptr][DATA_REF_COUNT] = data[data_ptr][DATA_REF_COUNT] + 1;return data_ptr;}int copy_data(int  data_ptr){if (!data_ptr)
return 0;int new_data_ptr = alloc_data();data[new_data_ptr][DATA_TYPE] = data[data_ptr][DATA_TYPE];int value_ptr = data[data_ptr][DATA_VALUE];if (data[data_ptr][DATA_TYPE] != DATA_TYPE_SYMBOL && data[data_ptr][DATA_TYPE] != DATA_TYPE_NUMBER)
{value_ptr = copy_ptr(value_ptr);};data[new_data_ptr][DATA_VALUE] = value_ptr;data[new_data_ptr][DATA_NEXT] = 0;return new_data_ptr;}int make_symbol(int  sym_ptr){int data_ptr = alloc_data();data[data_ptr][DATA_TYPE] = DATA_TYPE_SYMBOL;data[data_ptr][DATA_VALUE] = sym_ptr;return data_ptr;}int make_number(int  num){int data_ptr = alloc_data();data[data_ptr][DATA_TYPE] = DATA_TYPE_NUMBER;data[data_ptr][DATA_VALUE] = num;return data_ptr;}int make_list(int  head_ptr){int data_ptr = alloc_data();data[data_ptr][DATA_TYPE] = DATA_TYPE_LIST;data[data_ptr][DATA_VALUE] = head_ptr;return data_ptr;}int MAX_SYM_BUF_LEN = 4096[4096]int sym_bufint next_sym = 0int SYM_QUOTE = -1int SYM_ATOM = -2int SYM_NUMBER = -3int SYM_EQ = -4int SYM_CAR = -5int SYM_CDR = -6int SYM_CONS = -7int SYM_COND = -8int SYM_LAMBDA = -9int SYM_DEFINE = -10int SYM_T = -11int SYM_F = -12int SYM_LIST = -13int SYM_ADD = -14int SYM_SUB = -15int SYM_MUL = -16int SYM_DIV = -17int SYM_GT = -18int SYM_LT = -19int SYM_GE = -20int SYM_LE = -21int SYM_EQ_NUM = -22int PRE_SYM_COUNT = 22[22][8]int PREDEF_SYMS = 113,117,111,116,101,97,116,111,109,63,110,117,109,98,101,114,63,101,113,63,99,97,114,99,100,114,99,111,110,115,99,111,110,100,108,97,109,98,100,97,100,101,102,105,110,101,116,102,108,105,115,116,43,45,42,47,62,60,62,61,60,61,61int is_predef_sym(int  sym_ptr){int i = 0;for (; i < PRE_SYM_COUNT; ) {int j = 0;int failed = 0;for (; sym_buf[sym_ptr + j]; ) {if (sym_buf[sym_ptr + j] != PREDEF_SYMS[i][j])
{failed = 1;break;};j = j + 1;};if (!failed && !PREDEF_SYMS[i][j])
{next_sym = sym_ptr;return -i - 1;};i = i + 1;};return sym_ptr;}int is_eq_sym(int  sym1,int  sym2){if (sym1 == sym2)
return 1;if (sym1 < 0 || sym2 < 0)
return 0;int i = 0;for (; sym_buf[sym1 + i]; ) {if (sym_buf[sym1 + i] != sym_buf[sym2 + i])
return 0;i = i + 1;};return sym_buf[sym1 + i] == sym_buf[sym2 + i];}void print_sym(int  sym_ptr){if (sym_ptr < 0)
{int i = 0;for (; PREDEF_SYMS[-sym_ptr - 1][i]; ) {putch(PREDEF_SYMS[-sym_ptr - 1][i]);i = i + 1;};}
else
{int i = 0;for (; sym_buf[sym_ptr + i]; ) {putch(sym_buf[sym_ptr + i]);i = i + 1;};};}int make_env(int  outer_ptr){int data_ptr = alloc_data();data[data_ptr][DATA_TYPE] = DATA_TYPE_ENV;data[data_ptr][DATA_VALUE] = 0;data[data_ptr][DATA_NEXT] = outer_ptr;return data_ptr;}void env_add(int  env_ptr,int  sym_ptr,int  val_data_ptr){int sym = make_symbol(sym_ptr);data[sym][DATA_NEXT] = val_data_ptr;int pair = make_list(sym);data[pair][DATA_NEXT] = data[env_ptr][DATA_VALUE];data[env_ptr][DATA_VALUE] = pair;}int env_get(int  env_ptr,int  sym_ptr){int pair = data[env_ptr][DATA_VALUE];for (; pair; ) {int sym = data[pair][DATA_VALUE];if (is_eq_sym(sym_ptr,data[sym][DATA_VALUE]))
return pair;pair = data[pair][DATA_NEXT];};return 0;}void env_set(int  env_ptr,int  sym_ptr,int  val_data_ptr){int pair = env_get(env_ptr,sym_ptr);if (pair)
{int sym = data[pair][DATA_VALUE];free_data(data[sym][DATA_NEXT]);data[sym][DATA_NEXT] = val_data_ptr;}
else
{env_add(env_ptr,sym_ptr,val_data_ptr);};}int env_find(int  env_ptr,int  sym_ptr){if (!env_ptr)
return 0;int pair = env_get(env_ptr,sym_ptr);if (pair)
return data[data[pair][DATA_VALUE]][DATA_NEXT];return env_find(data[env_ptr][DATA_NEXT],sym_ptr);}int make_func(int  param_list_ptr,int  body_ptr,int  env_ptr){int data_ptr = alloc_data();data[data_ptr][DATA_TYPE] = DATA_TYPE_FUNC;data[data_ptr][DATA_VALUE] = param_list_ptr;data[param_list_ptr][DATA_NEXT] = body_ptr;data[body_ptr][DATA_NEXT] = env_ptr;return data_ptr;}int last_char = 32int TOKEN_EOF = 0int TOKEN_SYMBOL = 1int TOKEN_NUMBER = 2int TOKEN_QUOTE = 3int TOKEN_LPAREN = 4int TOKEN_RPAREN = 5int last_token = 0int is_space(int  c){return c == 32 || c == 9 || c == 10 || c == 13;}int is_digit(int  c){return c >= 48 && c <= 57;}int next_token(){for (; is_space(last_char); ) last_char = getch();if (last_char == -1)
return TOKEN_EOF;if (last_char >= 39 && last_char <= 41)
{int c = last_char;last_char = getch();return TOKEN_QUOTE + c - 39;};if (is_digit(last_char))
{int num = 0;for (; is_digit(last_char); ) {num = num * 10 + last_char - 48;last_char = getch();};last_token = num;return TOKEN_NUMBER;};int sym_ptr = next_sym;for (; last_char != -1 && !last_char >= 39 && last_char <= 41 && !is_space(last_char); ) {sym_buf[next_sym] = last_char;next_sym = next_sym + 1;last_char = getch();};sym_buf[next_sym] = 0;next_sym = next_sym + 1;if (next_sym >= MAX_SYM_BUF_LEN)
panic(ERR_BUFFER_OVERFLOW);last_token = is_predef_sym(sym_ptr);return TOKEN_SYMBOL;}int last_token_type = 0int parse(){if (last_token_type == TOKEN_EOF)
return 0;if (last_token_type == TOKEN_SYMBOL)
{int data_ptr = make_symbol(last_token);last_token_type = next_token();return data_ptr;};if (last_token_type == TOKEN_NUMBER)
{int data_ptr = make_number(last_token);last_token_type = next_token();return data_ptr;};if (last_token_type == TOKEN_QUOTE)
{last_token_type = next_token();int elem_ptr = parse();int quote_ptr = make_symbol(SYM_QUOTE);data[quote_ptr][DATA_NEXT] = elem_ptr;int list_ptr = make_list(quote_ptr);return list_ptr;};if (last_token_type == TOKEN_LPAREN)
{last_token_type = next_token();int list_ptr = make_list(0);int cur_elem = 0;for (; last_token_type != TOKEN_RPAREN; ) {int elem_ptr = parse();if (cur_elem)
{data[cur_elem][DATA_NEXT] = elem_ptr;}
else
{data[list_ptr][DATA_VALUE] = elem_ptr;};cur_elem = elem_ptr;};last_token_type = next_token();return list_ptr;};return panic(ERR_PARSE_ERROR);}int make_bool(int  value){int data_ptr = alloc_data();data[data_ptr][DATA_TYPE] = DATA_TYPE_SYMBOL;if (value)
{data[data_ptr][DATA_VALUE] = SYM_T;}
else
{data[data_ptr][DATA_VALUE] = SYM_F;};return data_ptr;}int is_true(int  bool_ptr){if (!bool_ptr)
return 1;if (data[bool_ptr][DATA_TYPE] != DATA_TYPE_SYMBOL)
panic(ERR_TYPE_MISMATCH);if (data[bool_ptr][DATA_VALUE] == SYM_T)
return 1;if (data[bool_ptr][DATA_VALUE] == SYM_F)
return 0;return panic(ERR_TYPE_MISMATCH);}int is_equal(int  val1,int  val2){if (val1 == val2)
return 1;if (!val1 || !val2)
return 0;if (data[val1][DATA_TYPE] != data[val2][DATA_TYPE])
return 0;if (data[val1][DATA_TYPE] == DATA_TYPE_SYMBOL)
{return is_eq_sym(data[val1][DATA_VALUE],data[val2][DATA_VALUE]);};if (data[val1][DATA_TYPE] == DATA_TYPE_NUMBER)
{return data[val1][DATA_VALUE] == data[val2][DATA_VALUE];};if (data[val1][DATA_TYPE] == DATA_TYPE_LIST)
{int list1 = data[val1][DATA_VALUE];int list2 = data[val2][DATA_VALUE];for (; list1 && list2; ) {if (!is_equal(list1,list2))
return 0;list1 = data[list1][DATA_NEXT];list2 = data[list2][DATA_NEXT];};if (list1 || list2)
return 0;return 1;};return panic(ERR_TYPE_MISMATCH);}int unwrap_list(int  list_ptr){if (data[list_ptr][DATA_TYPE] != DATA_TYPE_LIST)
panic(ERR_TYPE_MISMATCH);return data[list_ptr][DATA_VALUE];}int unwrap_symbol(int  sym_ptr){if (data[sym_ptr][DATA_TYPE] != DATA_TYPE_SYMBOL)
panic(ERR_TYPE_MISMATCH);return data[sym_ptr][DATA_VALUE];}int unwrap_number(int  num_ptr){if (data[num_ptr][DATA_TYPE] != DATA_TYPE_NUMBER)
panic(ERR_TYPE_MISMATCH);return data[num_ptr][DATA_VALUE];}int eval(int  data_ptr,int  env_ptr){if (data[data_ptr][DATA_TYPE] == DATA_TYPE_SYMBOL)
{int val_ptr = env_find(env_ptr,data[data_ptr][DATA_VALUE]);if (!val_ptr)
panic(ERR_SYMBOL_NOT_FOUND);return copy_ptr(val_ptr);};if (data[data_ptr][DATA_TYPE] == DATA_TYPE_NUMBER)
return copy_ptr(data_ptr);if (data[data_ptr][DATA_TYPE] == DATA_TYPE_LIST)
{int head_ptr = data[data_ptr][DATA_VALUE];if (!head_ptr)
panic(ERR_INVALID_LIST);int arg_ptr = data[head_ptr][DATA_NEXT];if (data[head_ptr][DATA_TYPE] != DATA_TYPE_SYMBOL || env_find(env_ptr,data[head_ptr][DATA_VALUE]))
{int func_ptr = eval(head_ptr,env_ptr);if (!func_ptr || data[func_ptr][DATA_TYPE] != DATA_TYPE_FUNC)
{panic(ERR_INVALID_FUNC);};int param_list_ptr = data[func_ptr][DATA_VALUE];int body_ptr = data[param_list_ptr][DATA_NEXT];int outer_ptr = data[body_ptr][DATA_NEXT];int func_env_ptr = make_env(copy_ptr(outer_ptr));int cur_param_ptr = data[param_list_ptr][DATA_VALUE];int cur_arg_ptr = arg_ptr;for (; cur_param_ptr && cur_arg_ptr; ) {int param_sym_ptr = data[cur_param_ptr][DATA_VALUE];int arg_ptr = eval(cur_arg_ptr,env_ptr);env_add(func_env_ptr,param_sym_ptr,arg_ptr);cur_param_ptr = data[cur_param_ptr][DATA_NEXT];cur_arg_ptr = data[cur_arg_ptr][DATA_NEXT];};if (cur_param_ptr || cur_arg_ptr)
panic(ERR_INVALID_ARG_NUM);int result_ptr = eval(body_ptr,func_env_ptr);free_data(func_env_ptr);free_data(func_ptr);return result_ptr;};int sym_ptr = data[head_ptr][DATA_VALUE];if (sym_ptr == SYM_QUOTE)
{if (!arg_ptr || data[arg_ptr][DATA_NEXT])
panic(ERR_INVALID_ARG_NUM);return copy_ptr(arg_ptr);};if (sym_ptr == SYM_ATOM)
{if (!arg_ptr || data[arg_ptr][DATA_NEXT])
panic(ERR_INVALID_ARG_NUM);int value_ptr = eval(arg_ptr,env_ptr);int result_ptr = make_bool(data[value_ptr][DATA_TYPE] != DATA_TYPE_LIST);free_data(value_ptr);return result_ptr;};if (sym_ptr == SYM_NUMBER)
{if (!arg_ptr || data[arg_ptr][DATA_NEXT])
panic(ERR_INVALID_ARG_NUM);int value_ptr = eval(arg_ptr,env_ptr);int result_ptr = make_bool(data[value_ptr][DATA_TYPE] == DATA_TYPE_NUMBER);free_data(value_ptr);return result_ptr;};if (sym_ptr == SYM_EQ)
{int arg2_ptr = data[arg_ptr][DATA_NEXT];if (!arg_ptr || !arg2_ptr || data[arg2_ptr][DATA_NEXT])
{panic(ERR_INVALID_ARG_NUM);};int value1_ptr = eval(arg_ptr,env_ptr);int value2_ptr = eval(arg2_ptr,env_ptr);int result_ptr = make_bool(is_equal(value1_ptr,value2_ptr));free_data(value1_ptr);free_data(value2_ptr);return result_ptr;};if (sym_ptr == SYM_CAR)
{if (!arg_ptr || data[arg_ptr][DATA_NEXT])
panic(ERR_INVALID_ARG_NUM);int value_ptr = eval(arg_ptr,env_ptr);int result_ptr = copy_ptr(unwrap_list(value_ptr));if (!result_ptr)
panic(ERR_INVALID_LIST);free_data(value_ptr);return result_ptr;};if (sym_ptr == SYM_CDR)
{if (!arg_ptr || data[arg_ptr][DATA_NEXT])
panic(ERR_INVALID_ARG_NUM);int value_ptr = eval(arg_ptr,env_ptr);int head_ptr = unwrap_list(value_ptr);if (!head_ptr)
panic(ERR_INVALID_LIST);int result_ptr = make_list(copy_ptr(data[head_ptr][DATA_NEXT]));free_data(value_ptr);return result_ptr;};if (sym_ptr == SYM_CONS)
{int arg2_ptr = data[arg_ptr][DATA_NEXT];if (!arg_ptr || !arg2_ptr || data[arg2_ptr][DATA_NEXT])
{panic(ERR_INVALID_ARG_NUM);};int value1_ptr = eval(arg_ptr,env_ptr);int value2_ptr = eval(arg2_ptr,env_ptr);int head_ptr = copy_data(value1_ptr);data[head_ptr][DATA_NEXT] = copy_ptr(unwrap_list(value2_ptr));int result_ptr = make_list(head_ptr);free_data(value1_ptr);free_data(value2_ptr);return result_ptr;};if (sym_ptr == SYM_COND)
{int cur_ptr = arg_ptr;for (; cur_ptr; ) {int test_ptr = unwrap_list(cur_ptr);int result_ptr = eval(test_ptr,env_ptr);if (is_true(result_ptr))
{int body_ptr = data[test_ptr][DATA_NEXT];int body_result_ptr = eval(body_ptr,env_ptr);free_data(result_ptr);return body_result_ptr;};free_data(result_ptr);cur_ptr = data[cur_ptr][DATA_NEXT];};return 0;};if (sym_ptr == SYM_LAMBDA)
{int arg2_ptr = data[arg_ptr][DATA_NEXT];if (!arg_ptr || !arg2_ptr || data[arg2_ptr][DATA_NEXT])
{panic(ERR_INVALID_ARG_NUM);};int result_ptr = make_func(copy_data(arg_ptr),copy_data(arg2_ptr),copy_ptr(env_ptr));return result_ptr;};if (sym_ptr == SYM_DEFINE)
{int arg2_ptr = data[arg_ptr][DATA_NEXT];if (!arg_ptr || !arg2_ptr || data[arg2_ptr][DATA_NEXT])
{panic(ERR_INVALID_ARG_NUM);};int sym_ptr = unwrap_symbol(arg_ptr);int value_ptr = eval(arg2_ptr,env_ptr);env_set(env_ptr,sym_ptr,value_ptr);return 0;};if (sym_ptr == SYM_LIST)
{int result_ptr = make_list(0);int cur_ptr = 0;for (; arg_ptr; ) {int value_ptr = eval(arg_ptr,env_ptr);int elem_ptr = copy_data(value_ptr);free_data(value_ptr);if (cur_ptr)
{data[cur_ptr][DATA_NEXT] = elem_ptr;}
else
{data[result_ptr][DATA_VALUE] = elem_ptr;};cur_ptr = elem_ptr;arg_ptr = data[arg_ptr][DATA_NEXT];};return result_ptr;};int arg2_ptr = data[arg_ptr][DATA_NEXT];if (!arg_ptr || !arg2_ptr || data[arg2_ptr][DATA_NEXT])
{panic(ERR_INVALID_ARG_NUM);};int value1_ptr = eval(arg_ptr,env_ptr);int value2_ptr = eval(arg2_ptr,env_ptr);int lhs = unwrap_number(value1_ptr);int rhs = unwrap_number(value2_ptr);int result_ptr = 0;if (sym_ptr == SYM_ADD)
{result_ptr = make_number(lhs + rhs);}
else
if (sym_ptr == SYM_SUB)
{result_ptr = make_number(lhs - rhs);}
else
if (sym_ptr == SYM_MUL)
{result_ptr = make_number(lhs * rhs);}
else
if (sym_ptr == SYM_DIV)
{result_ptr = make_number(lhs / rhs);}
else
if (sym_ptr == SYM_LT)
{result_ptr = make_bool(lhs < rhs);}
else
if (sym_ptr == SYM_LE)
{result_ptr = make_bool(lhs <= rhs);}
else
if (sym_ptr == SYM_GT)
{result_ptr = make_bool(lhs > rhs);}
else
if (sym_ptr == SYM_GE)
{result_ptr = make_bool(lhs >= rhs);}
else
if (sym_ptr == SYM_EQ_NUM)
{result_ptr = make_bool(lhs == rhs);}
else
{panic(ERR_INVALID_SYMBOL);};free_data(value1_ptr);free_data(value2_ptr);return result_ptr;};return panic(ERR_INVALID_DATA_TYPE);}void print(int  data_ptr){if (data[data_ptr][DATA_TYPE] == DATA_TYPE_SYMBOL)
{print_sym(data[data_ptr][DATA_VALUE]);return ;};if (data[data_ptr][DATA_TYPE] == DATA_TYPE_NUMBER)
{putint(data[data_ptr][DATA_VALUE]);return ;};if (data[data_ptr][DATA_TYPE] == DATA_TYPE_LIST)
{putch(40);int list_ptr = data[data_ptr][DATA_VALUE];for (; list_ptr; ) {print(list_ptr);list_ptr = data[list_ptr][DATA_NEXT];if (list_ptr)
putch(32);};putch(41);return ;};if (data[data_ptr][DATA_TYPE] == DATA_TYPE_FUNC)
{putch(35);putch(60);putch(102);putch(117);putch(110);putch(99);putch(32);putint(data_ptr);putch(62);return ;};panic(ERR_INVALID_DATA_TYPE);}int main(){init_data();int global_env_ptr = make_env(0);last_token_type = next_token();for (; 1; ) {int data_ptr = parse();if (data_ptr == 0)
break;int result_ptr = eval(data_ptr,global_env_ptr);if (result_ptr)
{print(result_ptr);putch(10);};free_data(data_ptr);free_data(result_ptr);};return 0;}; ModuleID = 'demo/lisp.c'
source_filename = "demo/lisp.c"

@"%d" = private constant [3 x i8] c"%d\00"
@"%d.1" = private constant [3 x i8] c"%d\00"
@"%d.2" = private constant [3 x i8] c"%d\00"
@"%d:" = private constant [4 x i8] c"%d:\00"
@" %d" = private constant [4 x i8] c" %d\00"
@0 = global i32 0, align 4
@1 = global i32 1, align 4
@2 = global i32 2, align 4
@3 = global i32 3, align 4
@4 = global i32 4, align 4
@5 = global i32 5, align 4
@6 = global i32 6, align 4
@7 = global i32 7, align 4
@8 = global i32 8, align 4
@9 = global i32 0, align 4
@10 = global i32 1, align 4
@11 = global i32 2, align 4
@12 = global i32 3, align 4
@13 = global i32 4, align 4
@14 = global i32 2048, align 4
@15 = global [2048 x [2048 x i32]] zeroinitializer, align 4
@16 = global i32 0, align 4
@17 = global i32 0, align 4
@18 = global i32 1, align 4
@19 = global i32 2, align 4
@20 = global i32 3, align 4
@21 = global i32 4, align 4
@22 = global i32 4096, align 4
@23 = global [4096 x i32] zeroinitializer, align 4
@24 = global i32 0, align 4
@25 = global i32 -1, align 4
@26 = global i32 -2, align 4
@27 = global i32 -3, align 4
@28 = global i32 -4, align 4
@29 = global i32 -5, align 4
@30 = global i32 -6, align 4
@31 = global i32 -7, align 4
@32 = global i32 -8, align 4
@33 = global i32 -9, align 4
@34 = global i32 -10, align 4
@35 = global i32 -11, align 4
@36 = global i32 -12, align 4
@37 = global i32 -13, align 4
@38 = global i32 -14, align 4
@39 = global i32 -15, align 4
@40 = global i32 -16, align 4
@41 = global i32 -17, align 4
@42 = global i32 -18, align 4
@43 = global i32 -19, align 4
@44 = global i32 -20, align 4
@45 = global i32 -21, align 4
@46 = global i32 -22, align 4
@47 = global i32 22, align 4
@48 = global [22 x [8 x i32]] [[8 x i32] [i32 113, i32 117, i32 111, i32 116, i32 101, i32 0, i32 0, i32 0], [8 x i32] [i32 97, i32 116, i32 111, i32 109, i32 63, i32 0, i32 0, i32 0], [8 x i32] [i32 110, i32 117, i32 109, i32 98, i32 101, i32 114, i32 63, i32 0], [8 x i32] [i32 101, i32 113, i32 63, i32 0, i32 0, i32 0, i32 0, i32 0], [8 x i32] [i32 99, i32 97, i32 114, i32 0, i32 0, i32 0, i32 0, i32 0], [8 x i32] [i32 99, i32 100, i32 114, i32 0, i32 0, i32 0, i32 0, i32 0], [8 x i32] [i32 99, i32 111, i32 110, i32 115, i32 0, i32 0, i32 0, i32 0], [8 x i32] [i32 99, i32 111, i32 110, i32 100, i32 0, i32 0, i32 0, i32 0], [8 x i32] [i32 108, i32 97, i32 109, i32 98, i32 100, i32 97, i32 0, i32 0], [8 x i32] [i32 100, i32 101, i32 102, i32 105, i32 110, i32 101, i32 0, i32 0], [8 x i32] [i32 116, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0], [8 x i32] [i32 102, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0], [8 x i32] [i32 108, i32 105, i32 115, i32 116, i32 0, i32 0, i32 0, i32 0], [8 x i32] [i32 43, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0], [8 x i32] [i32 45, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0], [8 x i32] [i32 42, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0], [8 x i32] [i32 47, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0], [8 x i32] [i32 62, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0], [8 x i32] [i32 60, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0], [8 x i32] [i32 62, i32 61, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0], [8 x i32] [i32 60, i32 61, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0], [8 x i32] [i32 61, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0]], align 4
@49 = global i32 32, align 4
@50 = global i32 0, align 4
@51 = global i32 1, align 4
@52 = global i32 2, align 4
@53 = global i32 3, align 4
@54 = global i32 4, align 4
@55 = global i32 5, align 4
@56 = global i32 0, align 4
@57 = global i32 0, align 4

declare i32 @printf(ptr, ...)

declare i32 @scanf(ptr, ...)

declare i32 @getchar()

declare i32 @putchar(i32)

define i32 @getint() {
entry:
  %val = alloca i32, align 4
  %val1 = load i32, ptr %val, align 4
  %0 = call i32 (ptr, ...) @scanf(ptr @"%d", ptr %val)
  %val2 = load i32, ptr %val, align 4
  ret i32 %val2
}

define i32 @getch() {
entry:
  %0 = call i32 @getchar()
  ret i32 %0
}

define i32 @getarray(ptr %val) {
entry:
  %i = alloca i32, align 4
  %len = alloca i32, align 4
  %val1 = alloca ptr, align 8
  store ptr %val, ptr %val1, align 8
  store i32 0, ptr %i, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %entry
  %i2 = load i32, ptr %i, align 4
  %len3 = load i32, ptr %len, align 4
  %0 = icmp slt i32 %i2, %len3
  br i1 %0, label %for.body, label %for.last

for.body:                                         ; preds = %for.cond
  %val4 = load ptr, ptr %val1, align 8
  %i5 = load i32, ptr %i, align 4
  %1 = getelementptr inbounds i32, ptr %val4, i32 %i5
  %2 = load i32, ptr %1, align 4
  %3 = call i32 (ptr, ...) @scanf(ptr @"%d.1", i32 %2)
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %i6 = load i32, ptr %i, align 4
  %4 = add i32 %i6, 1
  store i32 %4, ptr %i, align 4
  br label %for.cond

for.last:                                         ; preds = %for.cond
  %len7 = load i32, ptr %len, align 4
  ret i32 %len7
}

define void @putint(i32 %val) {
entry:
  %val1 = alloca i32, align 4
  store i32 %val, ptr %val1, align 4
  %val2 = load i32, ptr %val1, align 4
  %0 = call i32 (ptr, ...) @printf(ptr @"%d.2", i32 %val2)
  ret void
}

define void @putch(i32 %val) {
entry:
  %val1 = alloca i32, align 4
  store i32 %val, ptr %val1, align 4
  %val2 = load i32, ptr %val1, align 4
  %0 = call i32 @putchar(i32 %val2)
  ret void
}

define void @putarray(i32 %len, ptr %arr) {
entry:
  %i = alloca i32, align 4
  %len1 = alloca i32, align 4
  store i32 %len, ptr %len1, align 4
  %arr2 = alloca ptr, align 8
  store ptr %arr, ptr %arr2, align 8
  %len3 = load i32, ptr %len1, align 4
  %0 = call i32 (ptr, ...) @printf(ptr @"%d:", i32 %len3)
  store i32 0, ptr %i, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %entry
  %i4 = load i32, ptr %i, align 4
  %len5 = load i32, ptr %len1, align 4
  %1 = icmp slt i32 %i4, %len5
  br i1 %1, label %for.body, label %for.last

for.body:                                         ; preds = %for.cond
  %arr6 = load ptr, ptr %arr2, align 8
  %i7 = load i32, ptr %i, align 4
  %2 = getelementptr inbounds i32, ptr %arr6, i32 %i7
  %3 = load i32, ptr %2, align 4
  %4 = call i32 (ptr, ...) @printf(ptr @" %d", i32 %3)
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %i8 = load i32, ptr %i, align 4
  %5 = add i32 %i8, 1
  store i32 %5, ptr %i, align 4
  br label %for.cond

for.last:                                         ; preds = %for.cond
  ret void
}

define i32 @panic(i32 %code) {
entry:
  %code1 = alloca i32, align 4
  store i32 %code, ptr %code1, align 4
  call void @putch(i32 112)
  call void @putch(i32 97)
  call void @putch(i32 110)
  call void @putch(i32 105)
  call void @putch(i32 99)
  call void @putch(i32 32)
  %code2 = load i32, ptr %code1, align 4
  call void @putint(i32 %code2)
  call void @putch(i32 10)
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %entry
  br i1 true, label %for.body, label %for.last

for.body:                                         ; preds = %for.cond
  br label %for.inc

for.inc:                                          ; preds = %for.body
  br label %for.cond

for.last:                                         ; preds = %for.cond
  %code3 = load i32, ptr %code1, align 4
  ret i32 %code3
}

define void @init_data() {
entry:
  %i = alloca i32, align 4
  %data = load [2048 x [2048 x i32]], ptr @15, align 4
  %0 = load [2048 x i32], ptr @15, align 4
  %DATA_NEXT = load i32, ptr @11, align 4
  %1 = getelementptr inbounds i32, ptr @15, i32 %DATA_NEXT
  %2 = load i32, ptr %1, align 4
  store i32 0, ptr %1, align 4
  store i32 1, ptr %i, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %entry
  %i1 = load i32, ptr %i, align 4
  %MAX_DATA_LEN = load i32, ptr @14, align 4
  %3 = icmp slt i32 %i1, %MAX_DATA_LEN
  br i1 %3, label %for.body, label %for.last

for.body:                                         ; preds = %for.cond
  %data2 = load [2048 x [2048 x i32]], ptr @15, align 4
  %i3 = load i32, ptr %i, align 4
  %4 = getelementptr inbounds [2048 x i32], ptr @15, i32 %i3
  %5 = load [2048 x i32], ptr %4, align 4
  %DATA_NEXT4 = load i32, ptr @11, align 4
  %6 = getelementptr inbounds i32, ptr %4, i32 %DATA_NEXT4
  %7 = load i32, ptr %6, align 4
  %i5 = load i32, ptr %i, align 4
  %8 = sub nsw i32 %i5, 1
  store i32 %8, ptr %6, align 4
  %i6 = load i32, ptr %i, align 4
  %i7 = load i32, ptr %i, align 4
  %9 = add nsw i32 %i7, 1
  store i32 %9, ptr %i, align 4
  br label %for.inc

for.inc:                                          ; preds = %for.body
  br label %for.cond

for.last:                                         ; preds = %for.cond
  %free_data_ptr = load i32, ptr @16, align 4
  %MAX_DATA_LEN8 = load i32, ptr @14, align 4
  %10 = sub nsw i32 %MAX_DATA_LEN8, 1
  store i32 %10, ptr @16, align 4
  ret void
}

define i32 @alloc_data() {
entry:
  %i = alloca i32, align 4
  %free_data_ptr = load i32, ptr @16, align 4
  %0 = icmp ne i32 %free_data_ptr, 0
  %1 = xor i1 %0, true
  %2 = zext i1 %1 to i32
  %3 = icmp ne i32 %2, 0
  br i1 %3, label %then, label %last

then:                                             ; preds = %entry
  %ERR_BUFFER_OVERFLOW = load i32, ptr @1, align 4
  %4 = call i32 @panic(i32 %ERR_BUFFER_OVERFLOW)
  br label %last

last:                                             ; preds = %then, %entry
  %free_data_ptr1 = load i32, ptr @16, align 4
  store i32 %free_data_ptr1, ptr %i, align 4
  %free_data_ptr2 = load i32, ptr @16, align 4
  %data = load [2048 x [2048 x i32]], ptr @15, align 4
  %i3 = load i32, ptr %i, align 4
  %5 = getelementptr inbounds [2048 x i32], ptr @15, i32 %i3
  %6 = load [2048 x i32], ptr %5, align 4
  %DATA_NEXT = load i32, ptr @11, align 4
  %7 = getelementptr inbounds i32, ptr %5, i32 %DATA_NEXT
  %8 = load i32, ptr %7, align 4
  store i32 %8, ptr @16, align 4
  %data4 = load [2048 x [2048 x i32]], ptr @15, align 4
  %i5 = load i32, ptr %i, align 4
  %9 = getelementptr inbounds [2048 x i32], ptr @15, i32 %i5
  %10 = load [2048 x i32], ptr %9, align 4
  %DATA_NEXT6 = load i32, ptr @11, align 4
  %11 = getelementptr inbounds i32, ptr %9, i32 %DATA_NEXT6
  %12 = load i32, ptr %11, align 4
  store i32 0, ptr %11, align 4
  %data7 = load [2048 x [2048 x i32]], ptr @15, align 4
  %i8 = load i32, ptr %i, align 4
  %13 = getelementptr inbounds [2048 x i32], ptr @15, i32 %i8
  %14 = load [2048 x i32], ptr %13, align 4
  %DATA_REF_COUNT = load i32, ptr @12, align 4
  %15 = getelementptr inbounds i32, ptr %13, i32 %DATA_REF_COUNT
  %16 = load i32, ptr %15, align 4
  store i32 1, ptr %15, align 4
  %i9 = load i32, ptr %i, align 4
  ret i32 %i9
}

define void @free_data(i32 %data_ptr) {
entry:
  %ref_count = alloca i32, align 4
  %data_ptr1 = alloca i32, align 4
  store i32 %data_ptr, ptr %data_ptr1, align 4
  %data_ptr2 = load i32, ptr %data_ptr1, align 4
  %0 = icmp ne i32 %data_ptr2, 0
  %1 = xor i1 %0, true
  %2 = zext i1 %1 to i32
  %3 = icmp ne i32 %2, 0
  br i1 %3, label %then, label %last

then:                                             ; preds = %entry
  ret void

last:                                             ; preds = %entry
  %data = load [2048 x [2048 x i32]], ptr @15, align 4
  %data_ptr3 = load i32, ptr %data_ptr1, align 4
  %4 = getelementptr inbounds [2048 x i32], ptr @15, i32 %data_ptr3
  %5 = load [2048 x i32], ptr %4, align 4
  %DATA_REF_COUNT = load i32, ptr @12, align 4
  %6 = getelementptr inbounds i32, ptr %4, i32 %DATA_REF_COUNT
  %7 = load i32, ptr %6, align 4
  %8 = sub nsw i32 %7, 1
  store i32 %8, ptr %ref_count, align 4
  %data4 = load [2048 x [2048 x i32]], ptr @15, align 4
  %data_ptr5 = load i32, ptr %data_ptr1, align 4
  %9 = getelementptr inbounds [2048 x i32], ptr @15, i32 %data_ptr5
  %10 = load [2048 x i32], ptr %9, align 4
  %DATA_REF_COUNT6 = load i32, ptr @12, align 4
  %11 = getelementptr inbounds i32, ptr %9, i32 %DATA_REF_COUNT6
  %12 = load i32, ptr %11, align 4
  %ref_count7 = load i32, ptr %ref_count, align 4
  store i32 %ref_count7, ptr %11, align 4
  %ref_count8 = load i32, ptr %ref_count, align 4
  %13 = icmp sgt i32 %ref_count8, 0
  br i1 %13, label %then9, label %last10

then9:                                            ; preds = %last
  ret void

last10:                                           ; preds = %last
  %data11 = load [2048 x [2048 x i32]], ptr @15, align 4
  %data_ptr12 = load i32, ptr %data_ptr1, align 4
  %14 = getelementptr inbounds [2048 x i32], ptr @15, i32 %data_ptr12
  %15 = load [2048 x i32], ptr %14, align 4
  %DATA_TYPE = load i32, ptr @9, align 4
  %16 = getelementptr inbounds i32, ptr %14, i32 %DATA_TYPE
  %17 = load i32, ptr %16, align 4
  %DATA_TYPE_SYMBOL = load i32, ptr @17, align 4
  %18 = icmp ne i32 %17, %DATA_TYPE_SYMBOL
  br i1 %18, label %nextBB, label %falseBB

nextBB:                                           ; preds = %last10
  %data13 = load [2048 x [2048 x i32]], ptr @15, align 4
  %data_ptr14 = load i32, ptr %data_ptr1, align 4
  %19 = getelementptr inbounds [2048 x i32], ptr @15, i32 %data_ptr14
  %20 = load [2048 x i32], ptr %19, align 4
  %DATA_TYPE15 = load i32, ptr @9, align 4
  %21 = getelementptr inbounds i32, ptr %19, i32 %DATA_TYPE15
  %22 = load i32, ptr %21, align 4
  %DATA_TYPE_NUMBER = load i32, ptr @18, align 4
  %23 = icmp ne i32 %22, %DATA_TYPE_NUMBER
  %24 = zext i1 %23 to i32
  br label %mergeBB

falseBB:                                          ; preds = %last10
  br label %mergeBB

mergeBB:                                          ; preds = %falseBB, %nextBB
  %25 = phi i32 [ %24, %nextBB ], [ 0, %falseBB ]
  %26 = icmp ne i32 %25, 0
  br i1 %26, label %then16, label %last19

then16:                                           ; preds = %mergeBB
  %data17 = load [2048 x [2048 x i32]], ptr @15, align 4
  %data_ptr18 = load i32, ptr %data_ptr1, align 4
  %27 = getelementptr inbounds [2048 x i32], ptr @15, i32 %data_ptr18
  %28 = load [2048 x i32], ptr %27, align 4
  %DATA_VALUE = load i32, ptr @10, align 4
  %29 = getelementptr inbounds i32, ptr %27, i32 %DATA_VALUE
  %30 = load i32, ptr %29, align 4
  call void @free_data(i32 %30)
  br label %last19

last19:                                           ; preds = %then16, %mergeBB
  %data20 = load [2048 x [2048 x i32]], ptr @15, align 4
  %data_ptr21 = load i32, ptr %data_ptr1, align 4
  %31 = getelementptr inbounds [2048 x i32], ptr @15, i32 %data_ptr21
  %32 = load [2048 x i32], ptr %31, align 4
  %DATA_NEXT = load i32, ptr @11, align 4
  %33 = getelementptr inbounds i32, ptr %31, i32 %DATA_NEXT
  %34 = load i32, ptr %33, align 4
  call void @free_data(i32 %34)
  %data22 = load [2048 x [2048 x i32]], ptr @15, align 4
  %data_ptr23 = load i32, ptr %data_ptr1, align 4
  %35 = getelementptr inbounds [2048 x i32], ptr @15, i32 %data_ptr23
  %36 = load [2048 x i32], ptr %35, align 4
  %DATA_NEXT24 = load i32, ptr @11, align 4
  %37 = getelementptr inbounds i32, ptr %35, i32 %DATA_NEXT24
  %38 = load i32, ptr %37, align 4
  %free_data_ptr = load i32, ptr @16, align 4
  store i32 %free_data_ptr, ptr %37, align 4
  %free_data_ptr25 = load i32, ptr @16, align 4
  %data_ptr26 = load i32, ptr %data_ptr1, align 4
  store i32 %data_ptr26, ptr @16, align 4
  ret void
}

define i32 @copy_ptr(i32 %data_ptr) {
entry:
  %data_ptr1 = alloca i32, align 4
  store i32 %data_ptr, ptr %data_ptr1, align 4
  %data_ptr2 = load i32, ptr %data_ptr1, align 4
  %0 = icmp ne i32 %data_ptr2, 0
  %1 = xor i1 %0, true
  %2 = zext i1 %1 to i32
  %3 = icmp ne i32 %2, 0
  br i1 %3, label %then, label %last

then:                                             ; preds = %entry
  ret i32 0

last:                                             ; preds = %entry
  %data = load [2048 x [2048 x i32]], ptr @15, align 4
  %data_ptr3 = load i32, ptr %data_ptr1, align 4
  %4 = getelementptr inbounds [2048 x i32], ptr @15, i32 %data_ptr3
  %5 = load [2048 x i32], ptr %4, align 4
  %DATA_REF_COUNT = load i32, ptr @12, align 4
  %6 = getelementptr inbounds i32, ptr %4, i32 %DATA_REF_COUNT
  %7 = load i32, ptr %6, align 4
  %data4 = load [2048 x [2048 x i32]], ptr @15, align 4
  %data_ptr5 = load i32, ptr %data_ptr1, align 4
  %8 = getelementptr inbounds [2048 x i32], ptr @15, i32 %data_ptr5
  %9 = load [2048 x i32], ptr %8, align 4
  %DATA_REF_COUNT6 = load i32, ptr @12, align 4
  %10 = getelementptr inbounds i32, ptr %8, i32 %DATA_REF_COUNT6
  %11 = load i32, ptr %10, align 4
  %12 = add nsw i32 %11, 1
  store i32 %12, ptr %6, align 4
  %data_ptr7 = load i32, ptr %data_ptr1, align 4
  ret i32 %data_ptr7
}

define i32 @copy_data(i32 %data_ptr) {
entry:
  %value_ptr = alloca i32, align 4
  %new_data_ptr = alloca i32, align 4
  %data_ptr1 = alloca i32, align 4
  store i32 %data_ptr, ptr %data_ptr1, align 4
  %data_ptr2 = load i32, ptr %data_ptr1, align 4
  %0 = icmp ne i32 %data_ptr2, 0
  %1 = xor i1 %0, true
  %2 = zext i1 %1 to i32
  %3 = icmp ne i32 %2, 0
  br i1 %3, label %then, label %last

then:                                             ; preds = %entry
  ret i32 0

last:                                             ; preds = %entry
  %4 = call i32 @alloc_data()
  store i32 %4, ptr %new_data_ptr, align 4
  %data = load [2048 x [2048 x i32]], ptr @15, align 4
  %new_data_ptr3 = load i32, ptr %new_data_ptr, align 4
  %5 = getelementptr inbounds [2048 x i32], ptr @15, i32 %new_data_ptr3
  %6 = load [2048 x i32], ptr %5, align 4
  %DATA_TYPE = load i32, ptr @9, align 4
  %7 = getelementptr inbounds i32, ptr %5, i32 %DATA_TYPE
  %8 = load i32, ptr %7, align 4
  %data4 = load [2048 x [2048 x i32]], ptr @15, align 4
  %data_ptr5 = load i32, ptr %data_ptr1, align 4
  %9 = getelementptr inbounds [2048 x i32], ptr @15, i32 %data_ptr5
  %10 = load [2048 x i32], ptr %9, align 4
  %DATA_TYPE6 = load i32, ptr @9, align 4
  %11 = getelementptr inbounds i32, ptr %9, i32 %DATA_TYPE6
  %12 = load i32, ptr %11, align 4
  store i32 %12, ptr %7, align 4
  %data7 = load [2048 x [2048 x i32]], ptr @15, align 4
  %data_ptr8 = load i32, ptr %data_ptr1, align 4
  %13 = getelementptr inbounds [2048 x i32], ptr @15, i32 %data_ptr8
  %14 = load [2048 x i32], ptr %13, align 4
  %DATA_VALUE = load i32, ptr @10, align 4
  %15 = getelementptr inbounds i32, ptr %13, i32 %DATA_VALUE
  %16 = load i32, ptr %15, align 4
  store i32 %16, ptr %value_ptr, align 4
  %data9 = load [2048 x [2048 x i32]], ptr @15, align 4
  %data_ptr10 = load i32, ptr %data_ptr1, align 4
  %17 = getelementptr inbounds [2048 x i32], ptr @15, i32 %data_ptr10
  %18 = load [2048 x i32], ptr %17, align 4
  %DATA_TYPE11 = load i32, ptr @9, align 4
  %19 = getelementptr inbounds i32, ptr %17, i32 %DATA_TYPE11
  %20 = load i32, ptr %19, align 4
  %DATA_TYPE_SYMBOL = load i32, ptr @17, align 4
  %21 = icmp ne i32 %20, %DATA_TYPE_SYMBOL
  br i1 %21, label %nextBB, label %falseBB

nextBB:                                           ; preds = %last
  %data12 = load [2048 x [2048 x i32]], ptr @15, align 4
  %data_ptr13 = load i32, ptr %data_ptr1, align 4
  %22 = getelementptr inbounds [2048 x i32], ptr @15, i32 %data_ptr13
  %23 = load [2048 x i32], ptr %22, align 4
  %DATA_TYPE14 = load i32, ptr @9, align 4
  %24 = getelementptr inbounds i32, ptr %22, i32 %DATA_TYPE14
  %25 = load i32, ptr %24, align 4
  %DATA_TYPE_NUMBER = load i32, ptr @18, align 4
  %26 = icmp ne i32 %25, %DATA_TYPE_NUMBER
  %27 = zext i1 %26 to i32
  br label %mergeBB

falseBB:                                          ; preds = %last
  br label %mergeBB

mergeBB:                                          ; preds = %falseBB, %nextBB
  %28 = phi i32 [ %27, %nextBB ], [ 0, %falseBB ]
  %29 = icmp ne i32 %28, 0
  br i1 %29, label %then15, label %last18

then15:                                           ; preds = %mergeBB
  %value_ptr16 = load i32, ptr %value_ptr, align 4
  %value_ptr17 = load i32, ptr %value_ptr, align 4
  %30 = call i32 @copy_ptr(i32 %value_ptr17)
  store i32 %30, ptr %value_ptr, align 4
  br label %last18

last18:                                           ; preds = %then15, %mergeBB
  %data19 = load [2048 x [2048 x i32]], ptr @15, align 4
  %new_data_ptr20 = load i32, ptr %new_data_ptr, align 4
  %31 = getelementptr inbounds [2048 x i32], ptr @15, i32 %new_data_ptr20
  %32 = load [2048 x i32], ptr %31, align 4
  %DATA_VALUE21 = load i32, ptr @10, align 4
  %33 = getelementptr inbounds i32, ptr %31, i32 %DATA_VALUE21
  %34 = load i32, ptr %33, align 4
  %value_ptr22 = load i32, ptr %value_ptr, align 4
  store i32 %value_ptr22, ptr %33, align 4
  %data23 = load [2048 x [2048 x i32]], ptr @15, align 4
  %new_data_ptr24 = load i32, ptr %new_data_ptr, align 4
  %35 = getelementptr inbounds [2048 x i32], ptr @15, i32 %new_data_ptr24
  %36 = load [2048 x i32], ptr %35, align 4
  %DATA_NEXT = load i32, ptr @11, align 4
  %37 = getelementptr inbounds i32, ptr %35, i32 %DATA_NEXT
  %38 = load i32, ptr %37, align 4
  store i32 0, ptr %37, align 4
  %new_data_ptr25 = load i32, ptr %new_data_ptr, align 4
  ret i32 %new_data_ptr25
}

define i32 @make_symbol(i32 %sym_ptr) {
entry:
  %data_ptr = alloca i32, align 4
  %sym_ptr1 = alloca i32, align 4
  store i32 %sym_ptr, ptr %sym_ptr1, align 4
  %0 = call i32 @alloc_data()
  store i32 %0, ptr %data_ptr, align 4
  %data = load [2048 x [2048 x i32]], ptr @15, align 4
  %data_ptr2 = load i32, ptr %data_ptr, align 4
  %1 = getelementptr inbounds [2048 x i32], ptr @15, i32 %data_ptr2
  %2 = load [2048 x i32], ptr %1, align 4
  %DATA_TYPE = load i32, ptr @9, align 4
  %3 = getelementptr inbounds i32, ptr %1, i32 %DATA_TYPE
  %4 = load i32, ptr %3, align 4
  %DATA_TYPE_SYMBOL = load i32, ptr @17, align 4
  store i32 %DATA_TYPE_SYMBOL, ptr %3, align 4
  %data3 = load [2048 x [2048 x i32]], ptr @15, align 4
  %data_ptr4 = load i32, ptr %data_ptr, align 4
  %5 = getelementptr inbounds [2048 x i32], ptr @15, i32 %data_ptr4
  %6 = load [2048 x i32], ptr %5, align 4
  %DATA_VALUE = load i32, ptr @10, align 4
  %7 = getelementptr inbounds i32, ptr %5, i32 %DATA_VALUE
  %8 = load i32, ptr %7, align 4
  %sym_ptr5 = load i32, ptr %sym_ptr1, align 4
  store i32 %sym_ptr5, ptr %7, align 4
  %data_ptr6 = load i32, ptr %data_ptr, align 4
  ret i32 %data_ptr6
}

define i32 @make_number(i32 %num) {
entry:
  %data_ptr = alloca i32, align 4
  %num1 = alloca i32, align 4
  store i32 %num, ptr %num1, align 4
  %0 = call i32 @alloc_data()
  store i32 %0, ptr %data_ptr, align 4
  %data = load [2048 x [2048 x i32]], ptr @15, align 4
  %data_ptr2 = load i32, ptr %data_ptr, align 4
  %1 = getelementptr inbounds [2048 x i32], ptr @15, i32 %data_ptr2
  %2 = load [2048 x i32], ptr %1, align 4
  %DATA_TYPE = load i32, ptr @9, align 4
  %3 = getelementptr inbounds i32, ptr %1, i32 %DATA_TYPE
  %4 = load i32, ptr %3, align 4
  %DATA_TYPE_NUMBER = load i32, ptr @18, align 4
  store i32 %DATA_TYPE_NUMBER, ptr %3, align 4
  %data3 = load [2048 x [2048 x i32]], ptr @15, align 4
  %data_ptr4 = load i32, ptr %data_ptr, align 4
  %5 = getelementptr inbounds [2048 x i32], ptr @15, i32 %data_ptr4
  %6 = load [2048 x i32], ptr %5, align 4
  %DATA_VALUE = load i32, ptr @10, align 4
  %7 = getelementptr inbounds i32, ptr %5, i32 %DATA_VALUE
  %8 = load i32, ptr %7, align 4
  %num5 = load i32, ptr %num1, align 4
  store i32 %num5, ptr %7, align 4
  %data_ptr6 = load i32, ptr %data_ptr, align 4
  ret i32 %data_ptr6
}

define i32 @make_list(i32 %head_ptr) {
entry:
  %data_ptr = alloca i32, align 4
  %head_ptr1 = alloca i32, align 4
  store i32 %head_ptr, ptr %head_ptr1, align 4
  %0 = call i32 @alloc_data()
  store i32 %0, ptr %data_ptr, align 4
  %data = load [2048 x [2048 x i32]], ptr @15, align 4
  %data_ptr2 = load i32, ptr %data_ptr, align 4
  %1 = getelementptr inbounds [2048 x i32], ptr @15, i32 %data_ptr2
  %2 = load [2048 x i32], ptr %1, align 4
  %DATA_TYPE = load i32, ptr @9, align 4
  %3 = getelementptr inbounds i32, ptr %1, i32 %DATA_TYPE
  %4 = load i32, ptr %3, align 4
  %DATA_TYPE_LIST = load i32, ptr @19, align 4
  store i32 %DATA_TYPE_LIST, ptr %3, align 4
  %data3 = load [2048 x [2048 x i32]], ptr @15, align 4
  %data_ptr4 = load i32, ptr %data_ptr, align 4
  %5 = getelementptr inbounds [2048 x i32], ptr @15, i32 %data_ptr4
  %6 = load [2048 x i32], ptr %5, align 4
  %DATA_VALUE = load i32, ptr @10, align 4
  %7 = getelementptr inbounds i32, ptr %5, i32 %DATA_VALUE
  %8 = load i32, ptr %7, align 4
  %head_ptr5 = load i32, ptr %head_ptr1, align 4
  store i32 %head_ptr5, ptr %7, align 4
  %data_ptr6 = load i32, ptr %data_ptr, align 4
  ret i32 %data_ptr6
}

define i32 @is_predef_sym(i32 %sym_ptr) {
entry:
  %failed = alloca i32, align 4
  %j = alloca i32, align 4
  %i = alloca i32, align 4
  %sym_ptr1 = alloca i32, align 4
  store i32 %sym_ptr, ptr %sym_ptr1, align 4
  store i32 0, ptr %i, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc25, %entry
  %i2 = load i32, ptr %i, align 4
  %PRE_SYM_COUNT = load i32, ptr @47, align 4
  %0 = icmp slt i32 %i2, %PRE_SYM_COUNT
  br i1 %0, label %for.body, label %for.last26

for.body:                                         ; preds = %for.cond
  store i32 0, ptr %j, align 4
  store i32 0, ptr %failed, align 4
  br label %for.cond3

for.cond3:                                        ; preds = %for.inc, %for.body
  %sym_buf = load [4096 x i32], ptr @23, align 4
  %sym_ptr4 = load i32, ptr %sym_ptr1, align 4
  %j5 = load i32, ptr %j, align 4
  %1 = add nsw i32 %sym_ptr4, %j5
  %2 = getelementptr inbounds i32, ptr @23, i32 %1
  %3 = load i32, ptr %2, align 4
  %4 = icmp ne i32 %3, 0
  br i1 %4, label %for.body6, label %for.last

for.body6:                                        ; preds = %for.cond3
  %sym_buf7 = load [4096 x i32], ptr @23, align 4
  %sym_ptr8 = load i32, ptr %sym_ptr1, align 4
  %j9 = load i32, ptr %j, align 4
  %5 = add nsw i32 %sym_ptr8, %j9
  %6 = getelementptr inbounds i32, ptr @23, i32 %5
  %7 = load i32, ptr %6, align 4
  %PREDEF_SYMS = load [22 x [8 x i32]], ptr @48, align 4
  %i10 = load i32, ptr %i, align 4
  %8 = getelementptr inbounds [8 x i32], ptr @48, i32 %i10
  %9 = load [8 x i32], ptr %8, align 4
  %j11 = load i32, ptr %j, align 4
  %10 = getelementptr inbounds i32, ptr %8, i32 %j11
  %11 = load i32, ptr %10, align 4
  %12 = icmp ne i32 %7, %11
  br i1 %12, label %then, label %last

then:                                             ; preds = %for.body6
  %failed12 = load i32, ptr %failed, align 4
  store i32 1, ptr %failed, align 4
  br label %for.last

last:                                             ; preds = %for.body6
  %j13 = load i32, ptr %j, align 4
  %j14 = load i32, ptr %j, align 4
  %13 = add nsw i32 %j14, 1
  store i32 %13, ptr %j, align 4
  br label %for.inc

for.inc:                                          ; preds = %last
  br label %for.cond3

for.last:                                         ; preds = %then, %for.cond3
  %failed15 = load i32, ptr %failed, align 4
  %14 = icmp ne i32 %failed15, 0
  %15 = xor i1 %14, true
  %16 = zext i1 %15 to i32
  %17 = icmp ne i32 %16, 0
  br i1 %17, label %nextBB, label %falseBB

nextBB:                                           ; preds = %for.last
  %PREDEF_SYMS16 = load [22 x [8 x i32]], ptr @48, align 4
  %i17 = load i32, ptr %i, align 4
  %18 = getelementptr inbounds [8 x i32], ptr @48, i32 %i17
  %19 = load [8 x i32], ptr %18, align 4
  %j18 = load i32, ptr %j, align 4
  %20 = getelementptr inbounds i32, ptr %18, i32 %j18
  %21 = load i32, ptr %20, align 4
  %22 = icmp ne i32 %21, 0
  %23 = xor i1 %22, true
  %24 = zext i1 %23 to i32
  %25 = icmp ne i32 %24, 0
  %26 = zext i1 %25 to i32
  br label %mergeBB

falseBB:                                          ; preds = %for.last
  br label %mergeBB

mergeBB:                                          ; preds = %falseBB, %nextBB
  %27 = phi i32 [ %26, %nextBB ], [ 0, %falseBB ]
  %28 = icmp ne i32 %27, 0
  br i1 %28, label %then19, label %last22

then19:                                           ; preds = %mergeBB
  %next_sym = load i32, ptr @24, align 4
  %sym_ptr20 = load i32, ptr %sym_ptr1, align 4
  store i32 %sym_ptr20, ptr @24, align 4
  %i21 = load i32, ptr %i, align 4
  %29 = sub i32 0, %i21
  %30 = sub nsw i32 %29, 1
  ret i32 %30

last22:                                           ; preds = %mergeBB
  %i23 = load i32, ptr %i, align 4
  %i24 = load i32, ptr %i, align 4
  %31 = add nsw i32 %i24, 1
  store i32 %31, ptr %i, align 4
  br label %for.inc25

for.inc25:                                        ; preds = %last22
  br label %for.cond

for.last26:                                       ; preds = %for.cond
  %sym_ptr27 = load i32, ptr %sym_ptr1, align 4
  ret i32 %sym_ptr27
}

define i32 @is_eq_sym(i32 %sym1, i32 %sym2) {
entry:
  %i = alloca i32, align 4
  %sym11 = alloca i32, align 4
  store i32 %sym1, ptr %sym11, align 4
  %sym22 = alloca i32, align 4
  store i32 %sym2, ptr %sym22, align 4
  %sym13 = load i32, ptr %sym11, align 4
  %sym24 = load i32, ptr %sym22, align 4
  %0 = icmp eq i32 %sym13, %sym24
  br i1 %0, label %then, label %last

then:                                             ; preds = %entry
  ret i32 1

last:                                             ; preds = %entry
  %sym15 = load i32, ptr %sym11, align 4
  %1 = icmp slt i32 %sym15, 0
  br i1 %1, label %trueBB, label %nextBB

nextBB:                                           ; preds = %last
  %sym26 = load i32, ptr %sym22, align 4
  %2 = icmp slt i32 %sym26, 0
  %3 = zext i1 %2 to i32
  br label %mergeBB

trueBB:                                           ; preds = %last
  br label %mergeBB

mergeBB:                                          ; preds = %trueBB, %nextBB
  %4 = phi i32 [ %3, %nextBB ], [ 1, %trueBB ]
  %5 = icmp ne i32 %4, 0
  br i1 %5, label %then7, label %last8

then7:                                            ; preds = %mergeBB
  ret i32 0

last8:                                            ; preds = %mergeBB
  store i32 0, ptr %i, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %last8
  %sym_buf = load [4096 x i32], ptr @23, align 4
  %sym19 = load i32, ptr %sym11, align 4
  %i10 = load i32, ptr %i, align 4
  %6 = add nsw i32 %sym19, %i10
  %7 = getelementptr inbounds i32, ptr @23, i32 %6
  %8 = load i32, ptr %7, align 4
  %9 = icmp ne i32 %8, 0
  br i1 %9, label %for.body, label %for.last

for.body:                                         ; preds = %for.cond
  %sym_buf11 = load [4096 x i32], ptr @23, align 4
  %sym112 = load i32, ptr %sym11, align 4
  %i13 = load i32, ptr %i, align 4
  %10 = add nsw i32 %sym112, %i13
  %11 = getelementptr inbounds i32, ptr @23, i32 %10
  %12 = load i32, ptr %11, align 4
  %sym_buf14 = load [4096 x i32], ptr @23, align 4
  %sym215 = load i32, ptr %sym22, align 4
  %i16 = load i32, ptr %i, align 4
  %13 = add nsw i32 %sym215, %i16
  %14 = getelementptr inbounds i32, ptr @23, i32 %13
  %15 = load i32, ptr %14, align 4
  %16 = icmp ne i32 %12, %15
  br i1 %16, label %then17, label %last18

then17:                                           ; preds = %for.body
  ret i32 0

last18:                                           ; preds = %for.body
  %i19 = load i32, ptr %i, align 4
  %i20 = load i32, ptr %i, align 4
  %17 = add nsw i32 %i20, 1
  store i32 %17, ptr %i, align 4
  br label %for.inc

for.inc:                                          ; preds = %last18
  br label %for.cond

for.last:                                         ; preds = %for.cond
  %sym_buf21 = load [4096 x i32], ptr @23, align 4
  %sym122 = load i32, ptr %sym11, align 4
  %i23 = load i32, ptr %i, align 4
  %18 = add nsw i32 %sym122, %i23
  %19 = getelementptr inbounds i32, ptr @23, i32 %18
  %20 = load i32, ptr %19, align 4
  %sym_buf24 = load [4096 x i32], ptr @23, align 4
  %sym225 = load i32, ptr %sym22, align 4
  %i26 = load i32, ptr %i, align 4
  %21 = add nsw i32 %sym225, %i26
  %22 = getelementptr inbounds i32, ptr @23, i32 %21
  %23 = load i32, ptr %22, align 4
  %24 = icmp eq i32 %20, %23
  %25 = sext i1 %24 to i32
  ret i32 %25
}

define void @print_sym(i32 %sym_ptr) {
entry:
  %i10 = alloca i32, align 4
  %i = alloca i32, align 4
  %sym_ptr1 = alloca i32, align 4
  store i32 %sym_ptr, ptr %sym_ptr1, align 4
  %sym_ptr2 = load i32, ptr %sym_ptr1, align 4
  %0 = icmp slt i32 %sym_ptr2, 0
  br i1 %0, label %then, label %else

then:                                             ; preds = %entry
  store i32 0, ptr %i, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %then
  %PREDEF_SYMS = load [22 x [8 x i32]], ptr @48, align 4
  %sym_ptr3 = load i32, ptr %sym_ptr1, align 4
  %1 = sub i32 0, %sym_ptr3
  %2 = sub nsw i32 %1, 1
  %3 = getelementptr inbounds [8 x i32], ptr @48, i32 %2
  %4 = load [8 x i32], ptr %3, align 4
  %i4 = load i32, ptr %i, align 4
  %5 = getelementptr inbounds i32, ptr %3, i32 %i4
  %6 = load i32, ptr %5, align 4
  %7 = icmp ne i32 %6, 0
  br i1 %7, label %for.body, label %for.last

for.body:                                         ; preds = %for.cond
  %PREDEF_SYMS5 = load [22 x [8 x i32]], ptr @48, align 4
  %sym_ptr6 = load i32, ptr %sym_ptr1, align 4
  %8 = sub i32 0, %sym_ptr6
  %9 = sub nsw i32 %8, 1
  %10 = getelementptr inbounds [8 x i32], ptr @48, i32 %9
  %11 = load [8 x i32], ptr %10, align 4
  %i7 = load i32, ptr %i, align 4
  %12 = getelementptr inbounds i32, ptr %10, i32 %i7
  %13 = load i32, ptr %12, align 4
  call void @putch(i32 %13)
  %i8 = load i32, ptr %i, align 4
  %i9 = load i32, ptr %i, align 4
  %14 = add nsw i32 %i9, 1
  store i32 %14, ptr %i, align 4
  br label %for.inc

for.inc:                                          ; preds = %for.body
  br label %for.cond

for.last:                                         ; preds = %for.cond
  br label %last

else:                                             ; preds = %entry
  store i32 0, ptr %i10, align 4
  br label %for.cond11

for.cond11:                                       ; preds = %for.inc20, %else
  %sym_buf = load [4096 x i32], ptr @23, align 4
  %sym_ptr12 = load i32, ptr %sym_ptr1, align 4
  %i13 = load i32, ptr %i10, align 4
  %15 = add nsw i32 %sym_ptr12, %i13
  %16 = getelementptr inbounds i32, ptr @23, i32 %15
  %17 = load i32, ptr %16, align 4
  %18 = icmp ne i32 %17, 0
  br i1 %18, label %for.body14, label %for.last21

for.body14:                                       ; preds = %for.cond11
  %sym_buf15 = load [4096 x i32], ptr @23, align 4
  %sym_ptr16 = load i32, ptr %sym_ptr1, align 4
  %i17 = load i32, ptr %i10, align 4
  %19 = add nsw i32 %sym_ptr16, %i17
  %20 = getelementptr inbounds i32, ptr @23, i32 %19
  %21 = load i32, ptr %20, align 4
  call void @putch(i32 %21)
  %i18 = load i32, ptr %i10, align 4
  %i19 = load i32, ptr %i10, align 4
  %22 = add nsw i32 %i19, 1
  store i32 %22, ptr %i10, align 4
  br label %for.inc20

for.inc20:                                        ; preds = %for.body14
  br label %for.cond11

for.last21:                                       ; preds = %for.cond11
  br label %last

last:                                             ; preds = %for.last21, %for.last
  ret void
}

define i32 @make_env(i32 %outer_ptr) {
entry:
  %data_ptr = alloca i32, align 4
  %outer_ptr1 = alloca i32, align 4
  store i32 %outer_ptr, ptr %outer_ptr1, align 4
  %0 = call i32 @alloc_data()
  store i32 %0, ptr %data_ptr, align 4
  %data = load [2048 x [2048 x i32]], ptr @15, align 4
  %data_ptr2 = load i32, ptr %data_ptr, align 4
  %1 = getelementptr inbounds [2048 x i32], ptr @15, i32 %data_ptr2
  %2 = load [2048 x i32], ptr %1, align 4
  %DATA_TYPE = load i32, ptr @9, align 4
  %3 = getelementptr inbounds i32, ptr %1, i32 %DATA_TYPE
  %4 = load i32, ptr %3, align 4
  %DATA_TYPE_ENV = load i32, ptr @20, align 4
  store i32 %DATA_TYPE_ENV, ptr %3, align 4
  %data3 = load [2048 x [2048 x i32]], ptr @15, align 4
  %data_ptr4 = load i32, ptr %data_ptr, align 4
  %5 = getelementptr inbounds [2048 x i32], ptr @15, i32 %data_ptr4
  %6 = load [2048 x i32], ptr %5, align 4
  %DATA_VALUE = load i32, ptr @10, align 4
  %7 = getelementptr inbounds i32, ptr %5, i32 %DATA_VALUE
  %8 = load i32, ptr %7, align 4
  store i32 0, ptr %7, align 4
  %data5 = load [2048 x [2048 x i32]], ptr @15, align 4
  %data_ptr6 = load i32, ptr %data_ptr, align 4
  %9 = getelementptr inbounds [2048 x i32], ptr @15, i32 %data_ptr6
  %10 = load [2048 x i32], ptr %9, align 4
  %DATA_NEXT = load i32, ptr @11, align 4
  %11 = getelementptr inbounds i32, ptr %9, i32 %DATA_NEXT
  %12 = load i32, ptr %11, align 4
  %outer_ptr7 = load i32, ptr %outer_ptr1, align 4
  store i32 %outer_ptr7, ptr %11, align 4
  %data_ptr8 = load i32, ptr %data_ptr, align 4
  ret i32 %data_ptr8
}

define void @env_add(i32 %env_ptr, i32 %sym_ptr, i32 %val_data_ptr) {
entry:
  %pair = alloca i32, align 4
  %sym = alloca i32, align 4
  %env_ptr1 = alloca i32, align 4
  store i32 %env_ptr, ptr %env_ptr1, align 4
  %sym_ptr2 = alloca i32, align 4
  store i32 %sym_ptr, ptr %sym_ptr2, align 4
  %val_data_ptr3 = alloca i32, align 4
  store i32 %val_data_ptr, ptr %val_data_ptr3, align 4
  %sym_ptr4 = load i32, ptr %sym_ptr2, align 4
  %0 = call i32 @make_symbol(i32 %sym_ptr4)
  store i32 %0, ptr %sym, align 4
  %data = load [2048 x [2048 x i32]], ptr @15, align 4
  %sym5 = load i32, ptr %sym, align 4
  %1 = getelementptr inbounds [2048 x i32], ptr @15, i32 %sym5
  %2 = load [2048 x i32], ptr %1, align 4
  %DATA_NEXT = load i32, ptr @11, align 4
  %3 = getelementptr inbounds i32, ptr %1, i32 %DATA_NEXT
  %4 = load i32, ptr %3, align 4
  %val_data_ptr6 = load i32, ptr %val_data_ptr3, align 4
  store i32 %val_data_ptr6, ptr %3, align 4
  %sym7 = load i32, ptr %sym, align 4
  %5 = call i32 @make_list(i32 %sym7)
  store i32 %5, ptr %pair, align 4
  %data8 = load [2048 x [2048 x i32]], ptr @15, align 4
  %pair9 = load i32, ptr %pair, align 4
  %6 = getelementptr inbounds [2048 x i32], ptr @15, i32 %pair9
  %7 = load [2048 x i32], ptr %6, align 4
  %DATA_NEXT10 = load i32, ptr @11, align 4
  %8 = getelementptr inbounds i32, ptr %6, i32 %DATA_NEXT10
  %9 = load i32, ptr %8, align 4
  %data11 = load [2048 x [2048 x i32]], ptr @15, align 4
  %env_ptr12 = load i32, ptr %env_ptr1, align 4
  %10 = getelementptr inbounds [2048 x i32], ptr @15, i32 %env_ptr12
  %11 = load [2048 x i32], ptr %10, align 4
  %DATA_VALUE = load i32, ptr @10, align 4
  %12 = getelementptr inbounds i32, ptr %10, i32 %DATA_VALUE
  %13 = load i32, ptr %12, align 4
  store i32 %13, ptr %8, align 4
  %data13 = load [2048 x [2048 x i32]], ptr @15, align 4
  %env_ptr14 = load i32, ptr %env_ptr1, align 4
  %14 = getelementptr inbounds [2048 x i32], ptr @15, i32 %env_ptr14
  %15 = load [2048 x i32], ptr %14, align 4
  %DATA_VALUE15 = load i32, ptr @10, align 4
  %16 = getelementptr inbounds i32, ptr %14, i32 %DATA_VALUE15
  %17 = load i32, ptr %16, align 4
  %pair16 = load i32, ptr %pair, align 4
  store i32 %pair16, ptr %16, align 4
  ret void
}

define i32 @env_get(i32 %env_ptr, i32 %sym_ptr) {
entry:
  %sym = alloca i32, align 4
  %pair = alloca i32, align 4
  %env_ptr1 = alloca i32, align 4
  store i32 %env_ptr, ptr %env_ptr1, align 4
  %sym_ptr2 = alloca i32, align 4
  store i32 %sym_ptr, ptr %sym_ptr2, align 4
  %data = load [2048 x [2048 x i32]], ptr @15, align 4
  %env_ptr3 = load i32, ptr %env_ptr1, align 4
  %0 = getelementptr inbounds [2048 x i32], ptr @15, i32 %env_ptr3
  %1 = load [2048 x i32], ptr %0, align 4
  %DATA_VALUE = load i32, ptr @10, align 4
  %2 = getelementptr inbounds i32, ptr %0, i32 %DATA_VALUE
  %3 = load i32, ptr %2, align 4
  store i32 %3, ptr %pair, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %entry
  %pair4 = load i32, ptr %pair, align 4
  %4 = icmp ne i32 %pair4, 0
  br i1 %4, label %for.body, label %for.last

for.body:                                         ; preds = %for.cond
  %data5 = load [2048 x [2048 x i32]], ptr @15, align 4
  %pair6 = load i32, ptr %pair, align 4
  %5 = getelementptr inbounds [2048 x i32], ptr @15, i32 %pair6
  %6 = load [2048 x i32], ptr %5, align 4
  %DATA_VALUE7 = load i32, ptr @10, align 4
  %7 = getelementptr inbounds i32, ptr %5, i32 %DATA_VALUE7
  %8 = load i32, ptr %7, align 4
  store i32 %8, ptr %sym, align 4
  %sym_ptr8 = load i32, ptr %sym_ptr2, align 4
  %data9 = load [2048 x [2048 x i32]], ptr @15, align 4
  %sym10 = load i32, ptr %sym, align 4
  %9 = getelementptr inbounds [2048 x i32], ptr @15, i32 %sym10
  %10 = load [2048 x i32], ptr %9, align 4
  %DATA_VALUE11 = load i32, ptr @10, align 4
  %11 = getelementptr inbounds i32, ptr %9, i32 %DATA_VALUE11
  %12 = load i32, ptr %11, align 4
  %13 = call i32 @is_eq_sym(i32 %sym_ptr8, i32 %12)
  %14 = icmp ne i32 %13, 0
  br i1 %14, label %then, label %last

then:                                             ; preds = %for.body
  %pair12 = load i32, ptr %pair, align 4
  ret i32 %pair12

last:                                             ; preds = %for.body
  %pair13 = load i32, ptr %pair, align 4
  %data14 = load [2048 x [2048 x i32]], ptr @15, align 4
  %pair15 = load i32, ptr %pair, align 4
  %15 = getelementptr inbounds [2048 x i32], ptr @15, i32 %pair15
  %16 = load [2048 x i32], ptr %15, align 4
  %DATA_NEXT = load i32, ptr @11, align 4
  %17 = getelementptr inbounds i32, ptr %15, i32 %DATA_NEXT
  %18 = load i32, ptr %17, align 4
  store i32 %18, ptr %pair, align 4
  br label %for.inc

for.inc:                                          ; preds = %last
  br label %for.cond

for.last:                                         ; preds = %for.cond
  ret i32 0
}

define void @env_set(i32 %env_ptr, i32 %sym_ptr, i32 %val_data_ptr) {
entry:
  %sym = alloca i32, align 4
  %pair = alloca i32, align 4
  %env_ptr1 = alloca i32, align 4
  store i32 %env_ptr, ptr %env_ptr1, align 4
  %sym_ptr2 = alloca i32, align 4
  store i32 %sym_ptr, ptr %sym_ptr2, align 4
  %val_data_ptr3 = alloca i32, align 4
  store i32 %val_data_ptr, ptr %val_data_ptr3, align 4
  %env_ptr4 = load i32, ptr %env_ptr1, align 4
  %sym_ptr5 = load i32, ptr %sym_ptr2, align 4
  %0 = call i32 @env_get(i32 %env_ptr4, i32 %sym_ptr5)
  store i32 %0, ptr %pair, align 4
  %pair6 = load i32, ptr %pair, align 4
  %1 = icmp ne i32 %pair6, 0
  br i1 %1, label %then, label %else

then:                                             ; preds = %entry
  %data = load [2048 x [2048 x i32]], ptr @15, align 4
  %pair7 = load i32, ptr %pair, align 4
  %2 = getelementptr inbounds [2048 x i32], ptr @15, i32 %pair7
  %3 = load [2048 x i32], ptr %2, align 4
  %DATA_VALUE = load i32, ptr @10, align 4
  %4 = getelementptr inbounds i32, ptr %2, i32 %DATA_VALUE
  %5 = load i32, ptr %4, align 4
  store i32 %5, ptr %sym, align 4
  %data8 = load [2048 x [2048 x i32]], ptr @15, align 4
  %sym9 = load i32, ptr %sym, align 4
  %6 = getelementptr inbounds [2048 x i32], ptr @15, i32 %sym9
  %7 = load [2048 x i32], ptr %6, align 4
  %DATA_NEXT = load i32, ptr @11, align 4
  %8 = getelementptr inbounds i32, ptr %6, i32 %DATA_NEXT
  %9 = load i32, ptr %8, align 4
  call void @free_data(i32 %9)
  %data10 = load [2048 x [2048 x i32]], ptr @15, align 4
  %sym11 = load i32, ptr %sym, align 4
  %10 = getelementptr inbounds [2048 x i32], ptr @15, i32 %sym11
  %11 = load [2048 x i32], ptr %10, align 4
  %DATA_NEXT12 = load i32, ptr @11, align 4
  %12 = getelementptr inbounds i32, ptr %10, i32 %DATA_NEXT12
  %13 = load i32, ptr %12, align 4
  %val_data_ptr13 = load i32, ptr %val_data_ptr3, align 4
  store i32 %val_data_ptr13, ptr %12, align 4
  br label %last

else:                                             ; preds = %entry
  %env_ptr14 = load i32, ptr %env_ptr1, align 4
  %sym_ptr15 = load i32, ptr %sym_ptr2, align 4
  %val_data_ptr16 = load i32, ptr %val_data_ptr3, align 4
  call void @env_add(i32 %env_ptr14, i32 %sym_ptr15, i32 %val_data_ptr16)
  br label %last

last:                                             ; preds = %else, %then
  ret void
}

define i32 @env_find(i32 %env_ptr, i32 %sym_ptr) {
entry:
  %pair = alloca i32, align 4
  %env_ptr1 = alloca i32, align 4
  store i32 %env_ptr, ptr %env_ptr1, align 4
  %sym_ptr2 = alloca i32, align 4
  store i32 %sym_ptr, ptr %sym_ptr2, align 4
  %env_ptr3 = load i32, ptr %env_ptr1, align 4
  %0 = icmp ne i32 %env_ptr3, 0
  %1 = xor i1 %0, true
  %2 = zext i1 %1 to i32
  %3 = icmp ne i32 %2, 0
  br i1 %3, label %then, label %last

then:                                             ; preds = %entry
  ret i32 0

last:                                             ; preds = %entry
  %env_ptr4 = load i32, ptr %env_ptr1, align 4
  %sym_ptr5 = load i32, ptr %sym_ptr2, align 4
  %4 = call i32 @env_get(i32 %env_ptr4, i32 %sym_ptr5)
  store i32 %4, ptr %pair, align 4
  %pair6 = load i32, ptr %pair, align 4
  %5 = icmp ne i32 %pair6, 0
  br i1 %5, label %then7, label %last10

then7:                                            ; preds = %last
  %data = load [2048 x [2048 x i32]], ptr @15, align 4
  %data8 = load [2048 x [2048 x i32]], ptr @15, align 4
  %pair9 = load i32, ptr %pair, align 4
  %6 = getelementptr inbounds [2048 x i32], ptr @15, i32 %pair9
  %7 = load [2048 x i32], ptr %6, align 4
  %DATA_VALUE = load i32, ptr @10, align 4
  %8 = getelementptr inbounds i32, ptr %6, i32 %DATA_VALUE
  %9 = load i32, ptr %8, align 4
  %10 = getelementptr inbounds [2048 x i32], ptr @15, i32 %9
  %11 = load [2048 x i32], ptr %10, align 4
  %DATA_NEXT = load i32, ptr @11, align 4
  %12 = getelementptr inbounds i32, ptr %10, i32 %DATA_NEXT
  %13 = load i32, ptr %12, align 4
  ret i32 %13

last10:                                           ; preds = %last
  %data11 = load [2048 x [2048 x i32]], ptr @15, align 4
  %env_ptr12 = load i32, ptr %env_ptr1, align 4
  %14 = getelementptr inbounds [2048 x i32], ptr @15, i32 %env_ptr12
  %15 = load [2048 x i32], ptr %14, align 4
  %DATA_NEXT13 = load i32, ptr @11, align 4
  %16 = getelementptr inbounds i32, ptr %14, i32 %DATA_NEXT13
  %17 = load i32, ptr %16, align 4
  %sym_ptr14 = load i32, ptr %sym_ptr2, align 4
  %18 = call i32 @env_find(i32 %17, i32 %sym_ptr14)
  ret i32 %18
}

define i32 @make_func(i32 %param_list_ptr, i32 %body_ptr, i32 %env_ptr) {
entry:
  %data_ptr = alloca i32, align 4
  %param_list_ptr1 = alloca i32, align 4
  store i32 %param_list_ptr, ptr %param_list_ptr1, align 4
  %body_ptr2 = alloca i32, align 4
  store i32 %body_ptr, ptr %body_ptr2, align 4
  %env_ptr3 = alloca i32, align 4
  store i32 %env_ptr, ptr %env_ptr3, align 4
  %0 = call i32 @alloc_data()
  store i32 %0, ptr %data_ptr, align 4
  %data = load [2048 x [2048 x i32]], ptr @15, align 4
  %data_ptr4 = load i32, ptr %data_ptr, align 4
  %1 = getelementptr inbounds [2048 x i32], ptr @15, i32 %data_ptr4
  %2 = load [2048 x i32], ptr %1, align 4
  %DATA_TYPE = load i32, ptr @9, align 4
  %3 = getelementptr inbounds i32, ptr %1, i32 %DATA_TYPE
  %4 = load i32, ptr %3, align 4
  %DATA_TYPE_FUNC = load i32, ptr @21, align 4
  store i32 %DATA_TYPE_FUNC, ptr %3, align 4
  %data5 = load [2048 x [2048 x i32]], ptr @15, align 4
  %data_ptr6 = load i32, ptr %data_ptr, align 4
  %5 = getelementptr inbounds [2048 x i32], ptr @15, i32 %data_ptr6
  %6 = load [2048 x i32], ptr %5, align 4
  %DATA_VALUE = load i32, ptr @10, align 4
  %7 = getelementptr inbounds i32, ptr %5, i32 %DATA_VALUE
  %8 = load i32, ptr %7, align 4
  %param_list_ptr7 = load i32, ptr %param_list_ptr1, align 4
  store i32 %param_list_ptr7, ptr %7, align 4
  %data8 = load [2048 x [2048 x i32]], ptr @15, align 4
  %param_list_ptr9 = load i32, ptr %param_list_ptr1, align 4
  %9 = getelementptr inbounds [2048 x i32], ptr @15, i32 %param_list_ptr9
  %10 = load [2048 x i32], ptr %9, align 4
  %DATA_NEXT = load i32, ptr @11, align 4
  %11 = getelementptr inbounds i32, ptr %9, i32 %DATA_NEXT
  %12 = load i32, ptr %11, align 4
  %body_ptr10 = load i32, ptr %body_ptr2, align 4
  store i32 %body_ptr10, ptr %11, align 4
  %data11 = load [2048 x [2048 x i32]], ptr @15, align 4
  %body_ptr12 = load i32, ptr %body_ptr2, align 4
  %13 = getelementptr inbounds [2048 x i32], ptr @15, i32 %body_ptr12
  %14 = load [2048 x i32], ptr %13, align 4
  %DATA_NEXT13 = load i32, ptr @11, align 4
  %15 = getelementptr inbounds i32, ptr %13, i32 %DATA_NEXT13
  %16 = load i32, ptr %15, align 4
  %env_ptr14 = load i32, ptr %env_ptr3, align 4
  store i32 %env_ptr14, ptr %15, align 4
  %data_ptr15 = load i32, ptr %data_ptr, align 4
  ret i32 %data_ptr15
}

define i32 @is_space(i32 %c) {
entry:
  %c1 = alloca i32, align 4
  store i32 %c, ptr %c1, align 4
  %c2 = load i32, ptr %c1, align 4
  %0 = icmp eq i32 %c2, 32
  br i1 %0, label %trueBB, label %nextBB

nextBB:                                           ; preds = %entry
  %c3 = load i32, ptr %c1, align 4
  %1 = icmp eq i32 %c3, 9
  %2 = zext i1 %1 to i32
  br label %mergeBB

trueBB:                                           ; preds = %entry
  br label %mergeBB

mergeBB:                                          ; preds = %trueBB, %nextBB
  %3 = phi i32 [ %2, %nextBB ], [ 1, %trueBB ]
  %4 = icmp ne i32 %3, 0
  br i1 %4, label %trueBB6, label %nextBB4

nextBB4:                                          ; preds = %mergeBB
  %c5 = load i32, ptr %c1, align 4
  %5 = icmp eq i32 %c5, 10
  %6 = zext i1 %5 to i32
  br label %mergeBB7

trueBB6:                                          ; preds = %mergeBB
  br label %mergeBB7

mergeBB7:                                         ; preds = %trueBB6, %nextBB4
  %7 = phi i32 [ %6, %nextBB4 ], [ 1, %trueBB6 ]
  %8 = icmp ne i32 %7, 0
  br i1 %8, label %trueBB10, label %nextBB8

nextBB8:                                          ; preds = %mergeBB7
  %c9 = load i32, ptr %c1, align 4
  %9 = icmp eq i32 %c9, 13
  %10 = zext i1 %9 to i32
  br label %mergeBB11

trueBB10:                                         ; preds = %mergeBB7
  br label %mergeBB11

mergeBB11:                                        ; preds = %trueBB10, %nextBB8
  %11 = phi i32 [ %10, %nextBB8 ], [ 1, %trueBB10 ]
  ret i32 %11
}

define i32 @is_digit(i32 %c) {
entry:
  %c1 = alloca i32, align 4
  store i32 %c, ptr %c1, align 4
  %c2 = load i32, ptr %c1, align 4
  %0 = icmp sge i32 %c2, 48
  br i1 %0, label %nextBB, label %falseBB

nextBB:                                           ; preds = %entry
  %c3 = load i32, ptr %c1, align 4
  %1 = icmp sle i32 %c3, 57
  %2 = zext i1 %1 to i32
  br label %mergeBB

falseBB:                                          ; preds = %entry
  br label %mergeBB

mergeBB:                                          ; preds = %falseBB, %nextBB
  %3 = phi i32 [ %2, %nextBB ], [ 0, %falseBB ]
  ret i32 %3
}

define i32 @next_token() {
entry:
  %sym_ptr = alloca i32, align 4
  %num = alloca i32, align 4
  %c = alloca i32, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %entry
  %last_char = load i32, ptr @49, align 4
  %0 = call i32 @is_space(i32 %last_char)
  %1 = icmp ne i32 %0, 0
  br i1 %1, label %for.body, label %for.last

for.body:                                         ; preds = %for.cond
  %last_char1 = load i32, ptr @49, align 4
  %2 = call i32 @getch()
  store i32 %2, ptr @49, align 4
  br label %for.inc

for.inc:                                          ; preds = %for.body
  br label %for.cond

for.last:                                         ; preds = %for.cond
  %last_char2 = load i32, ptr @49, align 4
  %3 = icmp eq i32 %last_char2, -1
  br i1 %3, label %then, label %last

then:                                             ; preds = %for.last
  %TOKEN_EOF = load i32, ptr @50, align 4
  ret i32 %TOKEN_EOF

last:                                             ; preds = %for.last
  %last_char3 = load i32, ptr @49, align 4
  %4 = icmp sge i32 %last_char3, 39
  br i1 %4, label %nextBB, label %falseBB

nextBB:                                           ; preds = %last
  %last_char4 = load i32, ptr @49, align 4
  %5 = icmp sle i32 %last_char4, 41
  %6 = zext i1 %5 to i32
  br label %mergeBB

falseBB:                                          ; preds = %last
  br label %mergeBB

mergeBB:                                          ; preds = %falseBB, %nextBB
  %7 = phi i32 [ %6, %nextBB ], [ 0, %falseBB ]
  %8 = icmp ne i32 %7, 0
  br i1 %8, label %then5, label %last9

then5:                                            ; preds = %mergeBB
  %last_char6 = load i32, ptr @49, align 4
  store i32 %last_char6, ptr %c, align 4
  %last_char7 = load i32, ptr @49, align 4
  %9 = call i32 @getch()
  store i32 %9, ptr @49, align 4
  %TOKEN_QUOTE = load i32, ptr @53, align 4
  %c8 = load i32, ptr %c, align 4
  %10 = add nsw i32 %TOKEN_QUOTE, %c8
  %11 = sub nsw i32 %10, 39
  ret i32 %11

last9:                                            ; preds = %mergeBB
  %last_char10 = load i32, ptr @49, align 4
  %12 = call i32 @is_digit(i32 %last_char10)
  %13 = icmp ne i32 %12, 0
  br i1 %13, label %then11, label %last22

then11:                                           ; preds = %last9
  store i32 0, ptr %num, align 4
  br label %for.cond12

for.cond12:                                       ; preds = %for.inc19, %then11
  %last_char13 = load i32, ptr @49, align 4
  %14 = call i32 @is_digit(i32 %last_char13)
  %15 = icmp ne i32 %14, 0
  br i1 %15, label %for.body14, label %for.last20

for.body14:                                       ; preds = %for.cond12
  %num15 = load i32, ptr %num, align 4
  %num16 = load i32, ptr %num, align 4
  %16 = mul nsw i32 %num16, 10
  %last_char17 = load i32, ptr @49, align 4
  %17 = add nsw i32 %16, %last_char17
  %18 = sub nsw i32 %17, 48
  store i32 %18, ptr %num, align 4
  %last_char18 = load i32, ptr @49, align 4
  %19 = call i32 @getch()
  store i32 %19, ptr @49, align 4
  br label %for.inc19

for.inc19:                                        ; preds = %for.body14
  br label %for.cond12

for.last20:                                       ; preds = %for.cond12
  %last_token = load i32, ptr @56, align 4
  %num21 = load i32, ptr %num, align 4
  store i32 %num21, ptr @56, align 4
  %TOKEN_NUMBER = load i32, ptr @52, align 4
  ret i32 %TOKEN_NUMBER

last22:                                           ; preds = %last9
  %next_sym = load i32, ptr @24, align 4
  store i32 %next_sym, ptr %sym_ptr, align 4
  br label %for.cond23

for.cond23:                                       ; preds = %for.inc43, %last22
  %last_char24 = load i32, ptr @49, align 4
  %20 = icmp ne i32 %last_char24, -1
  br i1 %20, label %nextBB25, label %falseBB31

nextBB25:                                         ; preds = %for.cond23
  %last_char26 = load i32, ptr @49, align 4
  %21 = icmp sge i32 %last_char26, 39
  br i1 %21, label %nextBB27, label %falseBB29

nextBB27:                                         ; preds = %nextBB25
  %last_char28 = load i32, ptr @49, align 4
  %22 = icmp sle i32 %last_char28, 41
  %23 = zext i1 %22 to i32
  br label %mergeBB30

falseBB29:                                        ; preds = %nextBB25
  br label %mergeBB30

mergeBB30:                                        ; preds = %falseBB29, %nextBB27
  %24 = phi i32 [ %23, %nextBB27 ], [ 0, %falseBB29 ]
  %25 = icmp ne i32 %24, 0
  %26 = xor i1 %25, true
  %27 = zext i1 %26 to i32
  %28 = icmp ne i32 %27, 0
  %29 = zext i1 %28 to i32
  br label %mergeBB32

falseBB31:                                        ; preds = %for.cond23
  br label %mergeBB32

mergeBB32:                                        ; preds = %falseBB31, %mergeBB30
  %30 = phi i32 [ %29, %mergeBB30 ], [ 0, %falseBB31 ]
  %31 = icmp ne i32 %30, 0
  br i1 %31, label %nextBB33, label %falseBB35

nextBB33:                                         ; preds = %mergeBB32
  %last_char34 = load i32, ptr @49, align 4
  %32 = call i32 @is_space(i32 %last_char34)
  %33 = icmp ne i32 %32, 0
  %34 = xor i1 %33, true
  %35 = zext i1 %34 to i32
  %36 = icmp ne i32 %35, 0
  %37 = zext i1 %36 to i32
  br label %mergeBB36

falseBB35:                                        ; preds = %mergeBB32
  br label %mergeBB36

mergeBB36:                                        ; preds = %falseBB35, %nextBB33
  %38 = phi i32 [ %37, %nextBB33 ], [ 0, %falseBB35 ]
  %39 = icmp ne i32 %38, 0
  br i1 %39, label %for.body37, label %for.last44

for.body37:                                       ; preds = %mergeBB36
  %sym_buf = load [4096 x i32], ptr @23, align 4
  %next_sym38 = load i32, ptr @24, align 4
  %40 = getelementptr inbounds i32, ptr @23, i32 %next_sym38
  %41 = load i32, ptr %40, align 4
  %last_char39 = load i32, ptr @49, align 4
  store i32 %last_char39, ptr %40, align 4
  %next_sym40 = load i32, ptr @24, align 4
  %next_sym41 = load i32, ptr @24, align 4
  %42 = add nsw i32 %next_sym41, 1
  store i32 %42, ptr @24, align 4
  %last_char42 = load i32, ptr @49, align 4
  %43 = call i32 @getch()
  store i32 %43, ptr @49, align 4
  br label %for.inc43

for.inc43:                                        ; preds = %for.body37
  br label %for.cond23

for.last44:                                       ; preds = %mergeBB36
  %sym_buf45 = load [4096 x i32], ptr @23, align 4
  %next_sym46 = load i32, ptr @24, align 4
  %44 = getelementptr inbounds i32, ptr @23, i32 %next_sym46
  %45 = load i32, ptr %44, align 4
  store i32 0, ptr %44, align 4
  %next_sym47 = load i32, ptr @24, align 4
  %next_sym48 = load i32, ptr @24, align 4
  %46 = add nsw i32 %next_sym48, 1
  store i32 %46, ptr @24, align 4
  %next_sym49 = load i32, ptr @24, align 4
  %MAX_SYM_BUF_LEN = load i32, ptr @22, align 4
  %47 = icmp sge i32 %next_sym49, %MAX_SYM_BUF_LEN
  br i1 %47, label %then50, label %last51

then50:                                           ; preds = %for.last44
  %ERR_BUFFER_OVERFLOW = load i32, ptr @1, align 4
  %48 = call i32 @panic(i32 %ERR_BUFFER_OVERFLOW)
  br label %last51

last51:                                           ; preds = %then50, %for.last44
  %last_token52 = load i32, ptr @56, align 4
  %sym_ptr53 = load i32, ptr %sym_ptr, align 4
  %49 = call i32 @is_predef_sym(i32 %sym_ptr53)
  store i32 %49, ptr @56, align 4
  %TOKEN_SYMBOL = load i32, ptr @51, align 4
  ret i32 %TOKEN_SYMBOL
}

define i32 @parse() {
entry:
  %elem_ptr26 = alloca i32, align 4
  %cur_elem = alloca i32, align 4
  %list_ptr24 = alloca i32, align 4
  %list_ptr = alloca i32, align 4
  %quote_ptr = alloca i32, align 4
  %elem_ptr = alloca i32, align 4
  %data_ptr8 = alloca i32, align 4
  %data_ptr = alloca i32, align 4
  %last_token_type = load i32, ptr @57, align 4
  %TOKEN_EOF = load i32, ptr @50, align 4
  %0 = icmp eq i32 %last_token_type, %TOKEN_EOF
  br i1 %0, label %then, label %last

then:                                             ; preds = %entry
  ret i32 0

last:                                             ; preds = %entry
  %last_token_type1 = load i32, ptr @57, align 4
  %TOKEN_SYMBOL = load i32, ptr @51, align 4
  %1 = icmp eq i32 %last_token_type1, %TOKEN_SYMBOL
  br i1 %1, label %then2, label %last5

then2:                                            ; preds = %last
  %last_token = load i32, ptr @56, align 4
  %2 = call i32 @make_symbol(i32 %last_token)
  store i32 %2, ptr %data_ptr, align 4
  %last_token_type3 = load i32, ptr @57, align 4
  %3 = call i32 @next_token()
  store i32 %3, ptr @57, align 4
  %data_ptr4 = load i32, ptr %data_ptr, align 4
  ret i32 %data_ptr4

last5:                                            ; preds = %last
  %last_token_type6 = load i32, ptr @57, align 4
  %TOKEN_NUMBER = load i32, ptr @52, align 4
  %4 = icmp eq i32 %last_token_type6, %TOKEN_NUMBER
  br i1 %4, label %then7, label %last12

then7:                                            ; preds = %last5
  %last_token9 = load i32, ptr @56, align 4
  %5 = call i32 @make_number(i32 %last_token9)
  store i32 %5, ptr %data_ptr8, align 4
  %last_token_type10 = load i32, ptr @57, align 4
  %6 = call i32 @next_token()
  store i32 %6, ptr @57, align 4
  %data_ptr11 = load i32, ptr %data_ptr8, align 4
  ret i32 %data_ptr11

last12:                                           ; preds = %last5
  %last_token_type13 = load i32, ptr @57, align 4
  %TOKEN_QUOTE = load i32, ptr @53, align 4
  %7 = icmp eq i32 %last_token_type13, %TOKEN_QUOTE
  br i1 %7, label %then14, label %last20

then14:                                           ; preds = %last12
  %last_token_type15 = load i32, ptr @57, align 4
  %8 = call i32 @next_token()
  store i32 %8, ptr @57, align 4
  %9 = call i32 @parse()
  store i32 %9, ptr %elem_ptr, align 4
  %SYM_QUOTE = load i32, ptr @25, align 4
  %10 = call i32 @make_symbol(i32 %SYM_QUOTE)
  store i32 %10, ptr %quote_ptr, align 4
  %data = load [2048 x [2048 x i32]], ptr @15, align 4
  %quote_ptr16 = load i32, ptr %quote_ptr, align 4
  %11 = getelementptr inbounds [2048 x i32], ptr @15, i32 %quote_ptr16
  %12 = load [2048 x i32], ptr %11, align 4
  %DATA_NEXT = load i32, ptr @11, align 4
  %13 = getelementptr inbounds i32, ptr %11, i32 %DATA_NEXT
  %14 = load i32, ptr %13, align 4
  %elem_ptr17 = load i32, ptr %elem_ptr, align 4
  store i32 %elem_ptr17, ptr %13, align 4
  %quote_ptr18 = load i32, ptr %quote_ptr, align 4
  %15 = call i32 @make_list(i32 %quote_ptr18)
  store i32 %15, ptr %list_ptr, align 4
  %list_ptr19 = load i32, ptr %list_ptr, align 4
  ret i32 %list_ptr19

last20:                                           ; preds = %last12
  %last_token_type21 = load i32, ptr @57, align 4
  %TOKEN_LPAREN = load i32, ptr @54, align 4
  %16 = icmp eq i32 %last_token_type21, %TOKEN_LPAREN
  br i1 %16, label %then22, label %last41

then22:                                           ; preds = %last20
  %last_token_type23 = load i32, ptr @57, align 4
  %17 = call i32 @next_token()
  store i32 %17, ptr @57, align 4
  %18 = call i32 @make_list(i32 0)
  store i32 %18, ptr %list_ptr24, align 4
  store i32 0, ptr %cur_elem, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %then22
  %last_token_type25 = load i32, ptr @57, align 4
  %TOKEN_RPAREN = load i32, ptr @55, align 4
  %19 = icmp ne i32 %last_token_type25, %TOKEN_RPAREN
  br i1 %19, label %for.body, label %for.last

for.body:                                         ; preds = %for.cond
  %20 = call i32 @parse()
  store i32 %20, ptr %elem_ptr26, align 4
  %cur_elem27 = load i32, ptr %cur_elem, align 4
  %21 = icmp ne i32 %cur_elem27, 0
  br i1 %21, label %then28, label %else

then28:                                           ; preds = %for.body
  %data29 = load [2048 x [2048 x i32]], ptr @15, align 4
  %cur_elem30 = load i32, ptr %cur_elem, align 4
  %22 = getelementptr inbounds [2048 x i32], ptr @15, i32 %cur_elem30
  %23 = load [2048 x i32], ptr %22, align 4
  %DATA_NEXT31 = load i32, ptr @11, align 4
  %24 = getelementptr inbounds i32, ptr %22, i32 %DATA_NEXT31
  %25 = load i32, ptr %24, align 4
  %elem_ptr32 = load i32, ptr %elem_ptr26, align 4
  store i32 %elem_ptr32, ptr %24, align 4
  br label %last36

else:                                             ; preds = %for.body
  %data33 = load [2048 x [2048 x i32]], ptr @15, align 4
  %list_ptr34 = load i32, ptr %list_ptr24, align 4
  %26 = getelementptr inbounds [2048 x i32], ptr @15, i32 %list_ptr34
  %27 = load [2048 x i32], ptr %26, align 4
  %DATA_VALUE = load i32, ptr @10, align 4
  %28 = getelementptr inbounds i32, ptr %26, i32 %DATA_VALUE
  %29 = load i32, ptr %28, align 4
  %elem_ptr35 = load i32, ptr %elem_ptr26, align 4
  store i32 %elem_ptr35, ptr %28, align 4
  br label %last36

last36:                                           ; preds = %else, %then28
  %cur_elem37 = load i32, ptr %cur_elem, align 4
  %elem_ptr38 = load i32, ptr %elem_ptr26, align 4
  store i32 %elem_ptr38, ptr %cur_elem, align 4
  br label %for.inc

for.inc:                                          ; preds = %last36
  br label %for.cond

for.last:                                         ; preds = %for.cond
  %last_token_type39 = load i32, ptr @57, align 4
  %30 = call i32 @next_token()
  store i32 %30, ptr @57, align 4
  %list_ptr40 = load i32, ptr %list_ptr24, align 4
  ret i32 %list_ptr40

last41:                                           ; preds = %last20
  %ERR_PARSE_ERROR = load i32, ptr @2, align 4
  %31 = call i32 @panic(i32 %ERR_PARSE_ERROR)
  ret i32 %31
}

define i32 @make_bool(i32 %value) {
entry:
  %data_ptr = alloca i32, align 4
  %value1 = alloca i32, align 4
  store i32 %value, ptr %value1, align 4
  %0 = call i32 @alloc_data()
  store i32 %0, ptr %data_ptr, align 4
  %data = load [2048 x [2048 x i32]], ptr @15, align 4
  %data_ptr2 = load i32, ptr %data_ptr, align 4
  %1 = getelementptr inbounds [2048 x i32], ptr @15, i32 %data_ptr2
  %2 = load [2048 x i32], ptr %1, align 4
  %DATA_TYPE = load i32, ptr @9, align 4
  %3 = getelementptr inbounds i32, ptr %1, i32 %DATA_TYPE
  %4 = load i32, ptr %3, align 4
  %DATA_TYPE_SYMBOL = load i32, ptr @17, align 4
  store i32 %DATA_TYPE_SYMBOL, ptr %3, align 4
  %value3 = load i32, ptr %value1, align 4
  %5 = icmp ne i32 %value3, 0
  br i1 %5, label %then, label %else

then:                                             ; preds = %entry
  %data4 = load [2048 x [2048 x i32]], ptr @15, align 4
  %data_ptr5 = load i32, ptr %data_ptr, align 4
  %6 = getelementptr inbounds [2048 x i32], ptr @15, i32 %data_ptr5
  %7 = load [2048 x i32], ptr %6, align 4
  %DATA_VALUE = load i32, ptr @10, align 4
  %8 = getelementptr inbounds i32, ptr %6, i32 %DATA_VALUE
  %9 = load i32, ptr %8, align 4
  %SYM_T = load i32, ptr @35, align 4
  store i32 %SYM_T, ptr %8, align 4
  br label %last

else:                                             ; preds = %entry
  %data6 = load [2048 x [2048 x i32]], ptr @15, align 4
  %data_ptr7 = load i32, ptr %data_ptr, align 4
  %10 = getelementptr inbounds [2048 x i32], ptr @15, i32 %data_ptr7
  %11 = load [2048 x i32], ptr %10, align 4
  %DATA_VALUE8 = load i32, ptr @10, align 4
  %12 = getelementptr inbounds i32, ptr %10, i32 %DATA_VALUE8
  %13 = load i32, ptr %12, align 4
  %SYM_F = load i32, ptr @36, align 4
  store i32 %SYM_F, ptr %12, align 4
  br label %last

last:                                             ; preds = %else, %then
  %data_ptr9 = load i32, ptr %data_ptr, align 4
  ret i32 %data_ptr9
}

define i32 @is_true(i32 %bool_ptr) {
entry:
  %bool_ptr1 = alloca i32, align 4
  store i32 %bool_ptr, ptr %bool_ptr1, align 4
  %bool_ptr2 = load i32, ptr %bool_ptr1, align 4
  %0 = icmp ne i32 %bool_ptr2, 0
  %1 = xor i1 %0, true
  %2 = zext i1 %1 to i32
  %3 = icmp ne i32 %2, 0
  br i1 %3, label %then, label %last

then:                                             ; preds = %entry
  ret i32 1

last:                                             ; preds = %entry
  %data = load [2048 x [2048 x i32]], ptr @15, align 4
  %bool_ptr3 = load i32, ptr %bool_ptr1, align 4
  %4 = getelementptr inbounds [2048 x i32], ptr @15, i32 %bool_ptr3
  %5 = load [2048 x i32], ptr %4, align 4
  %DATA_TYPE = load i32, ptr @9, align 4
  %6 = getelementptr inbounds i32, ptr %4, i32 %DATA_TYPE
  %7 = load i32, ptr %6, align 4
  %DATA_TYPE_SYMBOL = load i32, ptr @17, align 4
  %8 = icmp ne i32 %7, %DATA_TYPE_SYMBOL
  br i1 %8, label %then4, label %last5

then4:                                            ; preds = %last
  %ERR_TYPE_MISMATCH = load i32, ptr @7, align 4
  %9 = call i32 @panic(i32 %ERR_TYPE_MISMATCH)
  br label %last5

last5:                                            ; preds = %then4, %last
  %data6 = load [2048 x [2048 x i32]], ptr @15, align 4
  %bool_ptr7 = load i32, ptr %bool_ptr1, align 4
  %10 = getelementptr inbounds [2048 x i32], ptr @15, i32 %bool_ptr7
  %11 = load [2048 x i32], ptr %10, align 4
  %DATA_VALUE = load i32, ptr @10, align 4
  %12 = getelementptr inbounds i32, ptr %10, i32 %DATA_VALUE
  %13 = load i32, ptr %12, align 4
  %SYM_T = load i32, ptr @35, align 4
  %14 = icmp eq i32 %13, %SYM_T
  br i1 %14, label %then8, label %last9

then8:                                            ; preds = %last5
  ret i32 1

last9:                                            ; preds = %last5
  %data10 = load [2048 x [2048 x i32]], ptr @15, align 4
  %bool_ptr11 = load i32, ptr %bool_ptr1, align 4
  %15 = getelementptr inbounds [2048 x i32], ptr @15, i32 %bool_ptr11
  %16 = load [2048 x i32], ptr %15, align 4
  %DATA_VALUE12 = load i32, ptr @10, align 4
  %17 = getelementptr inbounds i32, ptr %15, i32 %DATA_VALUE12
  %18 = load i32, ptr %17, align 4
  %SYM_F = load i32, ptr @36, align 4
  %19 = icmp eq i32 %18, %SYM_F
  br i1 %19, label %then13, label %last14

then13:                                           ; preds = %last9
  ret i32 0

last14:                                           ; preds = %last9
  %ERR_TYPE_MISMATCH15 = load i32, ptr @7, align 4
  %20 = call i32 @panic(i32 %ERR_TYPE_MISMATCH15)
  ret i32 %20
}

define i32 @is_equal(i32 %val1, i32 %val2) {
entry:
  %list2 = alloca i32, align 4
  %list1 = alloca i32, align 4
  %val11 = alloca i32, align 4
  store i32 %val1, ptr %val11, align 4
  %val22 = alloca i32, align 4
  store i32 %val2, ptr %val22, align 4
  %val13 = load i32, ptr %val11, align 4
  %val24 = load i32, ptr %val22, align 4
  %0 = icmp eq i32 %val13, %val24
  br i1 %0, label %then, label %last

then:                                             ; preds = %entry
  ret i32 1

last:                                             ; preds = %entry
  %val15 = load i32, ptr %val11, align 4
  %1 = icmp ne i32 %val15, 0
  %2 = xor i1 %1, true
  %3 = zext i1 %2 to i32
  %4 = icmp ne i32 %3, 0
  br i1 %4, label %trueBB, label %nextBB

nextBB:                                           ; preds = %last
  %val26 = load i32, ptr %val22, align 4
  %5 = icmp ne i32 %val26, 0
  %6 = xor i1 %5, true
  %7 = zext i1 %6 to i32
  %8 = icmp ne i32 %7, 0
  %9 = zext i1 %8 to i32
  br label %mergeBB

trueBB:                                           ; preds = %last
  br label %mergeBB

mergeBB:                                          ; preds = %trueBB, %nextBB
  %10 = phi i32 [ %9, %nextBB ], [ 1, %trueBB ]
  %11 = icmp ne i32 %10, 0
  br i1 %11, label %then7, label %last8

then7:                                            ; preds = %mergeBB
  ret i32 0

last8:                                            ; preds = %mergeBB
  %data = load [2048 x [2048 x i32]], ptr @15, align 4
  %val19 = load i32, ptr %val11, align 4
  %12 = getelementptr inbounds [2048 x i32], ptr @15, i32 %val19
  %13 = load [2048 x i32], ptr %12, align 4
  %DATA_TYPE = load i32, ptr @9, align 4
  %14 = getelementptr inbounds i32, ptr %12, i32 %DATA_TYPE
  %15 = load i32, ptr %14, align 4
  %data10 = load [2048 x [2048 x i32]], ptr @15, align 4
  %val211 = load i32, ptr %val22, align 4
  %16 = getelementptr inbounds [2048 x i32], ptr @15, i32 %val211
  %17 = load [2048 x i32], ptr %16, align 4
  %DATA_TYPE12 = load i32, ptr @9, align 4
  %18 = getelementptr inbounds i32, ptr %16, i32 %DATA_TYPE12
  %19 = load i32, ptr %18, align 4
  %20 = icmp ne i32 %15, %19
  br i1 %20, label %then13, label %last14

then13:                                           ; preds = %last8
  ret i32 0

last14:                                           ; preds = %last8
  %data15 = load [2048 x [2048 x i32]], ptr @15, align 4
  %val116 = load i32, ptr %val11, align 4
  %21 = getelementptr inbounds [2048 x i32], ptr @15, i32 %val116
  %22 = load [2048 x i32], ptr %21, align 4
  %DATA_TYPE17 = load i32, ptr @9, align 4
  %23 = getelementptr inbounds i32, ptr %21, i32 %DATA_TYPE17
  %24 = load i32, ptr %23, align 4
  %DATA_TYPE_SYMBOL = load i32, ptr @17, align 4
  %25 = icmp eq i32 %24, %DATA_TYPE_SYMBOL
  br i1 %25, label %then18, label %last24

then18:                                           ; preds = %last14
  %data19 = load [2048 x [2048 x i32]], ptr @15, align 4
  %val120 = load i32, ptr %val11, align 4
  %26 = getelementptr inbounds [2048 x i32], ptr @15, i32 %val120
  %27 = load [2048 x i32], ptr %26, align 4
  %DATA_VALUE = load i32, ptr @10, align 4
  %28 = getelementptr inbounds i32, ptr %26, i32 %DATA_VALUE
  %29 = load i32, ptr %28, align 4
  %data21 = load [2048 x [2048 x i32]], ptr @15, align 4
  %val222 = load i32, ptr %val22, align 4
  %30 = getelementptr inbounds [2048 x i32], ptr @15, i32 %val222
  %31 = load [2048 x i32], ptr %30, align 4
  %DATA_VALUE23 = load i32, ptr @10, align 4
  %32 = getelementptr inbounds i32, ptr %30, i32 %DATA_VALUE23
  %33 = load i32, ptr %32, align 4
  %34 = call i32 @is_eq_sym(i32 %29, i32 %33)
  ret i32 %34

last24:                                           ; preds = %last14
  %data25 = load [2048 x [2048 x i32]], ptr @15, align 4
  %val126 = load i32, ptr %val11, align 4
  %35 = getelementptr inbounds [2048 x i32], ptr @15, i32 %val126
  %36 = load [2048 x i32], ptr %35, align 4
  %DATA_TYPE27 = load i32, ptr @9, align 4
  %37 = getelementptr inbounds i32, ptr %35, i32 %DATA_TYPE27
  %38 = load i32, ptr %37, align 4
  %DATA_TYPE_NUMBER = load i32, ptr @18, align 4
  %39 = icmp eq i32 %38, %DATA_TYPE_NUMBER
  br i1 %39, label %then28, label %last35

then28:                                           ; preds = %last24
  %data29 = load [2048 x [2048 x i32]], ptr @15, align 4
  %val130 = load i32, ptr %val11, align 4
  %40 = getelementptr inbounds [2048 x i32], ptr @15, i32 %val130
  %41 = load [2048 x i32], ptr %40, align 4
  %DATA_VALUE31 = load i32, ptr @10, align 4
  %42 = getelementptr inbounds i32, ptr %40, i32 %DATA_VALUE31
  %43 = load i32, ptr %42, align 4
  %data32 = load [2048 x [2048 x i32]], ptr @15, align 4
  %val233 = load i32, ptr %val22, align 4
  %44 = getelementptr inbounds [2048 x i32], ptr @15, i32 %val233
  %45 = load [2048 x i32], ptr %44, align 4
  %DATA_VALUE34 = load i32, ptr @10, align 4
  %46 = getelementptr inbounds i32, ptr %44, i32 %DATA_VALUE34
  %47 = load i32, ptr %46, align 4
  %48 = icmp eq i32 %43, %47
  %49 = sext i1 %48 to i32
  ret i32 %49

last35:                                           ; preds = %last24
  %data36 = load [2048 x [2048 x i32]], ptr @15, align 4
  %val137 = load i32, ptr %val11, align 4
  %50 = getelementptr inbounds [2048 x i32], ptr @15, i32 %val137
  %51 = load [2048 x i32], ptr %50, align 4
  %DATA_TYPE38 = load i32, ptr @9, align 4
  %52 = getelementptr inbounds i32, ptr %50, i32 %DATA_TYPE38
  %53 = load i32, ptr %52, align 4
  %DATA_TYPE_LIST = load i32, ptr @19, align 4
  %54 = icmp eq i32 %53, %DATA_TYPE_LIST
  br i1 %54, label %then39, label %last68

then39:                                           ; preds = %last35
  %data40 = load [2048 x [2048 x i32]], ptr @15, align 4
  %val141 = load i32, ptr %val11, align 4
  %55 = getelementptr inbounds [2048 x i32], ptr @15, i32 %val141
  %56 = load [2048 x i32], ptr %55, align 4
  %DATA_VALUE42 = load i32, ptr @10, align 4
  %57 = getelementptr inbounds i32, ptr %55, i32 %DATA_VALUE42
  %58 = load i32, ptr %57, align 4
  store i32 %58, ptr %list1, align 4
  %data43 = load [2048 x [2048 x i32]], ptr @15, align 4
  %val244 = load i32, ptr %val22, align 4
  %59 = getelementptr inbounds [2048 x i32], ptr @15, i32 %val244
  %60 = load [2048 x i32], ptr %59, align 4
  %DATA_VALUE45 = load i32, ptr @10, align 4
  %61 = getelementptr inbounds i32, ptr %59, i32 %DATA_VALUE45
  %62 = load i32, ptr %61, align 4
  store i32 %62, ptr %list2, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %then39
  %list146 = load i32, ptr %list1, align 4
  %63 = icmp ne i32 %list146, 0
  br i1 %63, label %nextBB47, label %falseBB

nextBB47:                                         ; preds = %for.cond
  %list248 = load i32, ptr %list2, align 4
  %64 = icmp ne i32 %list248, 0
  %65 = zext i1 %64 to i32
  br label %mergeBB49

falseBB:                                          ; preds = %for.cond
  br label %mergeBB49

mergeBB49:                                        ; preds = %falseBB, %nextBB47
  %66 = phi i32 [ %65, %nextBB47 ], [ 0, %falseBB ]
  %67 = icmp ne i32 %66, 0
  br i1 %67, label %for.body, label %for.last

for.body:                                         ; preds = %mergeBB49
  %list150 = load i32, ptr %list1, align 4
  %list251 = load i32, ptr %list2, align 4
  %68 = call i32 @is_equal(i32 %list150, i32 %list251)
  %69 = icmp ne i32 %68, 0
  %70 = xor i1 %69, true
  %71 = zext i1 %70 to i32
  %72 = icmp ne i32 %71, 0
  br i1 %72, label %then52, label %last53

then52:                                           ; preds = %for.body
  ret i32 0

last53:                                           ; preds = %for.body
  %list154 = load i32, ptr %list1, align 4
  %data55 = load [2048 x [2048 x i32]], ptr @15, align 4
  %list156 = load i32, ptr %list1, align 4
  %73 = getelementptr inbounds [2048 x i32], ptr @15, i32 %list156
  %74 = load [2048 x i32], ptr %73, align 4
  %DATA_NEXT = load i32, ptr @11, align 4
  %75 = getelementptr inbounds i32, ptr %73, i32 %DATA_NEXT
  %76 = load i32, ptr %75, align 4
  store i32 %76, ptr %list1, align 4
  %list257 = load i32, ptr %list2, align 4
  %data58 = load [2048 x [2048 x i32]], ptr @15, align 4
  %list259 = load i32, ptr %list2, align 4
  %77 = getelementptr inbounds [2048 x i32], ptr @15, i32 %list259
  %78 = load [2048 x i32], ptr %77, align 4
  %DATA_NEXT60 = load i32, ptr @11, align 4
  %79 = getelementptr inbounds i32, ptr %77, i32 %DATA_NEXT60
  %80 = load i32, ptr %79, align 4
  store i32 %80, ptr %list2, align 4
  br label %for.inc

for.inc:                                          ; preds = %last53
  br label %for.cond

for.last:                                         ; preds = %mergeBB49
  %list161 = load i32, ptr %list1, align 4
  %81 = icmp ne i32 %list161, 0
  br i1 %81, label %trueBB64, label %nextBB62

nextBB62:                                         ; preds = %for.last
  %list263 = load i32, ptr %list2, align 4
  %82 = icmp ne i32 %list263, 0
  %83 = zext i1 %82 to i32
  br label %mergeBB65

trueBB64:                                         ; preds = %for.last
  br label %mergeBB65

mergeBB65:                                        ; preds = %trueBB64, %nextBB62
  %84 = phi i32 [ %83, %nextBB62 ], [ 1, %trueBB64 ]
  %85 = icmp ne i32 %84, 0
  br i1 %85, label %then66, label %last67

then66:                                           ; preds = %mergeBB65
  ret i32 0

last67:                                           ; preds = %mergeBB65
  ret i32 1

last68:                                           ; preds = %last35
  %ERR_TYPE_MISMATCH = load i32, ptr @7, align 4
  %86 = call i32 @panic(i32 %ERR_TYPE_MISMATCH)
  ret i32 %86
}

define i32 @unwrap_list(i32 %list_ptr) {
entry:
  %list_ptr1 = alloca i32, align 4
  store i32 %list_ptr, ptr %list_ptr1, align 4
  %data = load [2048 x [2048 x i32]], ptr @15, align 4
  %list_ptr2 = load i32, ptr %list_ptr1, align 4
  %0 = getelementptr inbounds [2048 x i32], ptr @15, i32 %list_ptr2
  %1 = load [2048 x i32], ptr %0, align 4
  %DATA_TYPE = load i32, ptr @9, align 4
  %2 = getelementptr inbounds i32, ptr %0, i32 %DATA_TYPE
  %3 = load i32, ptr %2, align 4
  %DATA_TYPE_LIST = load i32, ptr @19, align 4
  %4 = icmp ne i32 %3, %DATA_TYPE_LIST
  br i1 %4, label %then, label %last

then:                                             ; preds = %entry
  %ERR_TYPE_MISMATCH = load i32, ptr @7, align 4
  %5 = call i32 @panic(i32 %ERR_TYPE_MISMATCH)
  br label %last

last:                                             ; preds = %then, %entry
  %data3 = load [2048 x [2048 x i32]], ptr @15, align 4
  %list_ptr4 = load i32, ptr %list_ptr1, align 4
  %6 = getelementptr inbounds [2048 x i32], ptr @15, i32 %list_ptr4
  %7 = load [2048 x i32], ptr %6, align 4
  %DATA_VALUE = load i32, ptr @10, align 4
  %8 = getelementptr inbounds i32, ptr %6, i32 %DATA_VALUE
  %9 = load i32, ptr %8, align 4
  ret i32 %9
}

define i32 @unwrap_symbol(i32 %sym_ptr) {
entry:
  %sym_ptr1 = alloca i32, align 4
  store i32 %sym_ptr, ptr %sym_ptr1, align 4
  %data = load [2048 x [2048 x i32]], ptr @15, align 4
  %sym_ptr2 = load i32, ptr %sym_ptr1, align 4
  %0 = getelementptr inbounds [2048 x i32], ptr @15, i32 %sym_ptr2
  %1 = load [2048 x i32], ptr %0, align 4
  %DATA_TYPE = load i32, ptr @9, align 4
  %2 = getelementptr inbounds i32, ptr %0, i32 %DATA_TYPE
  %3 = load i32, ptr %2, align 4
  %DATA_TYPE_SYMBOL = load i32, ptr @17, align 4
  %4 = icmp ne i32 %3, %DATA_TYPE_SYMBOL
  br i1 %4, label %then, label %last

then:                                             ; preds = %entry
  %ERR_TYPE_MISMATCH = load i32, ptr @7, align 4
  %5 = call i32 @panic(i32 %ERR_TYPE_MISMATCH)
  br label %last

last:                                             ; preds = %then, %entry
  %data3 = load [2048 x [2048 x i32]], ptr @15, align 4
  %sym_ptr4 = load i32, ptr %sym_ptr1, align 4
  %6 = getelementptr inbounds [2048 x i32], ptr @15, i32 %sym_ptr4
  %7 = load [2048 x i32], ptr %6, align 4
  %DATA_VALUE = load i32, ptr @10, align 4
  %8 = getelementptr inbounds i32, ptr %6, i32 %DATA_VALUE
  %9 = load i32, ptr %8, align 4
  ret i32 %9
}

define i32 @unwrap_number(i32 %num_ptr) {
entry:
  %num_ptr1 = alloca i32, align 4
  store i32 %num_ptr, ptr %num_ptr1, align 4
  %data = load [2048 x [2048 x i32]], ptr @15, align 4
  %num_ptr2 = load i32, ptr %num_ptr1, align 4
  %0 = getelementptr inbounds [2048 x i32], ptr @15, i32 %num_ptr2
  %1 = load [2048 x i32], ptr %0, align 4
  %DATA_TYPE = load i32, ptr @9, align 4
  %2 = getelementptr inbounds i32, ptr %0, i32 %DATA_TYPE
  %3 = load i32, ptr %2, align 4
  %DATA_TYPE_NUMBER = load i32, ptr @18, align 4
  %4 = icmp ne i32 %3, %DATA_TYPE_NUMBER
  br i1 %4, label %then, label %last

then:                                             ; preds = %entry
  %ERR_TYPE_MISMATCH = load i32, ptr @7, align 4
  %5 = call i32 @panic(i32 %ERR_TYPE_MISMATCH)
  br label %last

last:                                             ; preds = %then, %entry
  %data3 = load [2048 x [2048 x i32]], ptr @15, align 4
  %num_ptr4 = load i32, ptr %num_ptr1, align 4
  %6 = getelementptr inbounds [2048 x i32], ptr @15, i32 %num_ptr4
  %7 = load [2048 x i32], ptr %6, align 4
  %DATA_VALUE = load i32, ptr @10, align 4
  %8 = getelementptr inbounds i32, ptr %6, i32 %DATA_VALUE
  %9 = load i32, ptr %8, align 4
  ret i32 %9
}

define i32 @eval(i32 %data_ptr, i32 %env_ptr) {
entry:
  %result_ptr422 = alloca i32, align 4
  %rhs = alloca i32, align 4
  %lhs = alloca i32, align 4
  %value2_ptr417 = alloca i32, align 4
  %value1_ptr414 = alloca i32, align 4
  %arg2_ptr396 = alloca i32, align 4
  %elem_ptr = alloca i32, align 4
  %value_ptr370 = alloca i32, align 4
  %cur_ptr366 = alloca i32, align 4
  %result_ptr365 = alloca i32, align 4
  %value_ptr356 = alloca i32, align 4
  %sym_ptr354 = alloca i32, align 4
  %arg2_ptr336 = alloca i32, align 4
  %result_ptr328 = alloca i32, align 4
  %arg2_ptr310 = alloca i32, align 4
  %body_result_ptr = alloca i32, align 4
  %body_ptr291 = alloca i32, align 4
  %result_ptr286 = alloca i32, align 4
  %test_ptr = alloca i32, align 4
  %cur_ptr = alloca i32, align 4
  %result_ptr273 = alloca i32, align 4
  %head_ptr267 = alloca i32, align 4
  %value2_ptr264 = alloca i32, align 4
  %value1_ptr261 = alloca i32, align 4
  %arg2_ptr243 = alloca i32, align 4
  %result_ptr234 = alloca i32, align 4
  %head_ptr228 = alloca i32, align 4
  %value_ptr225 = alloca i32, align 4
  %result_ptr204 = alloca i32, align 4
  %value_ptr201 = alloca i32, align 4
  %result_ptr182 = alloca i32, align 4
  %value2_ptr = alloca i32, align 4
  %value1_ptr = alloca i32, align 4
  %arg2_ptr = alloca i32, align 4
  %result_ptr151 = alloca i32, align 4
  %value_ptr148 = alloca i32, align 4
  %result_ptr128 = alloca i32, align 4
  %value_ptr = alloca i32, align 4
  %sym_ptr = alloca i32, align 4
  %result_ptr = alloca i32, align 4
  %arg_ptr70 = alloca i32, align 4
  %param_sym_ptr = alloca i32, align 4
  %cur_arg_ptr = alloca i32, align 4
  %cur_param_ptr = alloca i32, align 4
  %func_env_ptr = alloca i32, align 4
  %outer_ptr = alloca i32, align 4
  %body_ptr = alloca i32, align 4
  %param_list_ptr = alloca i32, align 4
  %func_ptr = alloca i32, align 4
  %arg_ptr = alloca i32, align 4
  %head_ptr = alloca i32, align 4
  %val_ptr = alloca i32, align 4
  %data_ptr1 = alloca i32, align 4
  store i32 %data_ptr, ptr %data_ptr1, align 4
  %env_ptr2 = alloca i32, align 4
  store i32 %env_ptr, ptr %env_ptr2, align 4
  %data = load [2048 x [2048 x i32]], ptr @15, align 4
  %data_ptr3 = load i32, ptr %data_ptr1, align 4
  %0 = getelementptr inbounds [2048 x i32], ptr @15, i32 %data_ptr3
  %1 = load [2048 x i32], ptr %0, align 4
  %DATA_TYPE = load i32, ptr @9, align 4
  %2 = getelementptr inbounds i32, ptr %0, i32 %DATA_TYPE
  %3 = load i32, ptr %2, align 4
  %DATA_TYPE_SYMBOL = load i32, ptr @17, align 4
  %4 = icmp eq i32 %3, %DATA_TYPE_SYMBOL
  br i1 %4, label %then, label %last10

then:                                             ; preds = %entry
  %env_ptr4 = load i32, ptr %env_ptr2, align 4
  %data5 = load [2048 x [2048 x i32]], ptr @15, align 4
  %data_ptr6 = load i32, ptr %data_ptr1, align 4
  %5 = getelementptr inbounds [2048 x i32], ptr @15, i32 %data_ptr6
  %6 = load [2048 x i32], ptr %5, align 4
  %DATA_VALUE = load i32, ptr @10, align 4
  %7 = getelementptr inbounds i32, ptr %5, i32 %DATA_VALUE
  %8 = load i32, ptr %7, align 4
  %9 = call i32 @env_find(i32 %env_ptr4, i32 %8)
  store i32 %9, ptr %val_ptr, align 4
  %val_ptr7 = load i32, ptr %val_ptr, align 4
  %10 = icmp ne i32 %val_ptr7, 0
  %11 = xor i1 %10, true
  %12 = zext i1 %11 to i32
  %13 = icmp ne i32 %12, 0
  br i1 %13, label %then8, label %last

then8:                                            ; preds = %then
  %ERR_SYMBOL_NOT_FOUND = load i32, ptr @3, align 4
  %14 = call i32 @panic(i32 %ERR_SYMBOL_NOT_FOUND)
  br label %last

last:                                             ; preds = %then8, %then
  %val_ptr9 = load i32, ptr %val_ptr, align 4
  %15 = call i32 @copy_ptr(i32 %val_ptr9)
  ret i32 %15

last10:                                           ; preds = %entry
  %data11 = load [2048 x [2048 x i32]], ptr @15, align 4
  %data_ptr12 = load i32, ptr %data_ptr1, align 4
  %16 = getelementptr inbounds [2048 x i32], ptr @15, i32 %data_ptr12
  %17 = load [2048 x i32], ptr %16, align 4
  %DATA_TYPE13 = load i32, ptr @9, align 4
  %18 = getelementptr inbounds i32, ptr %16, i32 %DATA_TYPE13
  %19 = load i32, ptr %18, align 4
  %DATA_TYPE_NUMBER = load i32, ptr @18, align 4
  %20 = icmp eq i32 %19, %DATA_TYPE_NUMBER
  br i1 %20, label %then14, label %last16

then14:                                           ; preds = %last10
  %data_ptr15 = load i32, ptr %data_ptr1, align 4
  %21 = call i32 @copy_ptr(i32 %data_ptr15)
  ret i32 %21

last16:                                           ; preds = %last10
  %data17 = load [2048 x [2048 x i32]], ptr @15, align 4
  %data_ptr18 = load i32, ptr %data_ptr1, align 4
  %22 = getelementptr inbounds [2048 x i32], ptr @15, i32 %data_ptr18
  %23 = load [2048 x i32], ptr %22, align 4
  %DATA_TYPE19 = load i32, ptr @9, align 4
  %24 = getelementptr inbounds i32, ptr %22, i32 %DATA_TYPE19
  %25 = load i32, ptr %24, align 4
  %DATA_TYPE_LIST = load i32, ptr @19, align 4
  %26 = icmp eq i32 %25, %DATA_TYPE_LIST
  br i1 %26, label %then20, label %last489

then20:                                           ; preds = %last16
  %data21 = load [2048 x [2048 x i32]], ptr @15, align 4
  %data_ptr22 = load i32, ptr %data_ptr1, align 4
  %27 = getelementptr inbounds [2048 x i32], ptr @15, i32 %data_ptr22
  %28 = load [2048 x i32], ptr %27, align 4
  %DATA_VALUE23 = load i32, ptr @10, align 4
  %29 = getelementptr inbounds i32, ptr %27, i32 %DATA_VALUE23
  %30 = load i32, ptr %29, align 4
  store i32 %30, ptr %head_ptr, align 4
  %head_ptr24 = load i32, ptr %head_ptr, align 4
  %31 = icmp ne i32 %head_ptr24, 0
  %32 = xor i1 %31, true
  %33 = zext i1 %32 to i32
  %34 = icmp ne i32 %33, 0
  br i1 %34, label %then25, label %last26

then25:                                           ; preds = %then20
  %ERR_INVALID_LIST = load i32, ptr @4, align 4
  %35 = call i32 @panic(i32 %ERR_INVALID_LIST)
  br label %last26

last26:                                           ; preds = %then25, %then20
  %data27 = load [2048 x [2048 x i32]], ptr @15, align 4
  %head_ptr28 = load i32, ptr %head_ptr, align 4
  %36 = getelementptr inbounds [2048 x i32], ptr @15, i32 %head_ptr28
  %37 = load [2048 x i32], ptr %36, align 4
  %DATA_NEXT = load i32, ptr @11, align 4
  %38 = getelementptr inbounds i32, ptr %36, i32 %DATA_NEXT
  %39 = load i32, ptr %38, align 4
  store i32 %39, ptr %arg_ptr, align 4
  %data29 = load [2048 x [2048 x i32]], ptr @15, align 4
  %head_ptr30 = load i32, ptr %head_ptr, align 4
  %40 = getelementptr inbounds [2048 x i32], ptr @15, i32 %head_ptr30
  %41 = load [2048 x i32], ptr %40, align 4
  %DATA_TYPE31 = load i32, ptr @9, align 4
  %42 = getelementptr inbounds i32, ptr %40, i32 %DATA_TYPE31
  %43 = load i32, ptr %42, align 4
  %DATA_TYPE_SYMBOL32 = load i32, ptr @17, align 4
  %44 = icmp ne i32 %43, %DATA_TYPE_SYMBOL32
  br i1 %44, label %trueBB, label %nextBB

nextBB:                                           ; preds = %last26
  %env_ptr33 = load i32, ptr %env_ptr2, align 4
  %data34 = load [2048 x [2048 x i32]], ptr @15, align 4
  %head_ptr35 = load i32, ptr %head_ptr, align 4
  %45 = getelementptr inbounds [2048 x i32], ptr @15, i32 %head_ptr35
  %46 = load [2048 x i32], ptr %45, align 4
  %DATA_VALUE36 = load i32, ptr @10, align 4
  %47 = getelementptr inbounds i32, ptr %45, i32 %DATA_VALUE36
  %48 = load i32, ptr %47, align 4
  %49 = call i32 @env_find(i32 %env_ptr33, i32 %48)
  %50 = icmp ne i32 %49, 0
  %51 = zext i1 %50 to i32
  br label %mergeBB

trueBB:                                           ; preds = %last26
  br label %mergeBB

mergeBB:                                          ; preds = %trueBB, %nextBB
  %52 = phi i32 [ %51, %nextBB ], [ 1, %trueBB ]
  %53 = icmp ne i32 %52, 0
  br i1 %53, label %then37, label %last96

then37:                                           ; preds = %mergeBB
  %head_ptr38 = load i32, ptr %head_ptr, align 4
  %env_ptr39 = load i32, ptr %env_ptr2, align 4
  %54 = call i32 @eval(i32 %head_ptr38, i32 %env_ptr39)
  store i32 %54, ptr %func_ptr, align 4
  %func_ptr40 = load i32, ptr %func_ptr, align 4
  %55 = icmp ne i32 %func_ptr40, 0
  %56 = xor i1 %55, true
  %57 = zext i1 %56 to i32
  %58 = icmp ne i32 %57, 0
  br i1 %58, label %trueBB45, label %nextBB41

nextBB41:                                         ; preds = %then37
  %data42 = load [2048 x [2048 x i32]], ptr @15, align 4
  %func_ptr43 = load i32, ptr %func_ptr, align 4
  %59 = getelementptr inbounds [2048 x i32], ptr @15, i32 %func_ptr43
  %60 = load [2048 x i32], ptr %59, align 4
  %DATA_TYPE44 = load i32, ptr @9, align 4
  %61 = getelementptr inbounds i32, ptr %59, i32 %DATA_TYPE44
  %62 = load i32, ptr %61, align 4
  %DATA_TYPE_FUNC = load i32, ptr @21, align 4
  %63 = icmp ne i32 %62, %DATA_TYPE_FUNC
  %64 = zext i1 %63 to i32
  br label %mergeBB46

trueBB45:                                         ; preds = %then37
  br label %mergeBB46

mergeBB46:                                        ; preds = %trueBB45, %nextBB41
  %65 = phi i32 [ %64, %nextBB41 ], [ 1, %trueBB45 ]
  %66 = icmp ne i32 %65, 0
  br i1 %66, label %then47, label %last48

then47:                                           ; preds = %mergeBB46
  %ERR_INVALID_FUNC = load i32, ptr @5, align 4
  %67 = call i32 @panic(i32 %ERR_INVALID_FUNC)
  br label %last48

last48:                                           ; preds = %then47, %mergeBB46
  %data49 = load [2048 x [2048 x i32]], ptr @15, align 4
  %func_ptr50 = load i32, ptr %func_ptr, align 4
  %68 = getelementptr inbounds [2048 x i32], ptr @15, i32 %func_ptr50
  %69 = load [2048 x i32], ptr %68, align 4
  %DATA_VALUE51 = load i32, ptr @10, align 4
  %70 = getelementptr inbounds i32, ptr %68, i32 %DATA_VALUE51
  %71 = load i32, ptr %70, align 4
  store i32 %71, ptr %param_list_ptr, align 4
  %data52 = load [2048 x [2048 x i32]], ptr @15, align 4
  %param_list_ptr53 = load i32, ptr %param_list_ptr, align 4
  %72 = getelementptr inbounds [2048 x i32], ptr @15, i32 %param_list_ptr53
  %73 = load [2048 x i32], ptr %72, align 4
  %DATA_NEXT54 = load i32, ptr @11, align 4
  %74 = getelementptr inbounds i32, ptr %72, i32 %DATA_NEXT54
  %75 = load i32, ptr %74, align 4
  store i32 %75, ptr %body_ptr, align 4
  %data55 = load [2048 x [2048 x i32]], ptr @15, align 4
  %body_ptr56 = load i32, ptr %body_ptr, align 4
  %76 = getelementptr inbounds [2048 x i32], ptr @15, i32 %body_ptr56
  %77 = load [2048 x i32], ptr %76, align 4
  %DATA_NEXT57 = load i32, ptr @11, align 4
  %78 = getelementptr inbounds i32, ptr %76, i32 %DATA_NEXT57
  %79 = load i32, ptr %78, align 4
  store i32 %79, ptr %outer_ptr, align 4
  %outer_ptr58 = load i32, ptr %outer_ptr, align 4
  %80 = call i32 @copy_ptr(i32 %outer_ptr58)
  %81 = call i32 @make_env(i32 %80)
  store i32 %81, ptr %func_env_ptr, align 4
  %data59 = load [2048 x [2048 x i32]], ptr @15, align 4
  %param_list_ptr60 = load i32, ptr %param_list_ptr, align 4
  %82 = getelementptr inbounds [2048 x i32], ptr @15, i32 %param_list_ptr60
  %83 = load [2048 x i32], ptr %82, align 4
  %DATA_VALUE61 = load i32, ptr @10, align 4
  %84 = getelementptr inbounds i32, ptr %82, i32 %DATA_VALUE61
  %85 = load i32, ptr %84, align 4
  store i32 %85, ptr %cur_param_ptr, align 4
  %arg_ptr62 = load i32, ptr %arg_ptr, align 4
  store i32 %arg_ptr62, ptr %cur_arg_ptr, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %last48
  %cur_param_ptr63 = load i32, ptr %cur_param_ptr, align 4
  %86 = icmp ne i32 %cur_param_ptr63, 0
  br i1 %86, label %nextBB64, label %falseBB

nextBB64:                                         ; preds = %for.cond
  %cur_arg_ptr65 = load i32, ptr %cur_arg_ptr, align 4
  %87 = icmp ne i32 %cur_arg_ptr65, 0
  %88 = zext i1 %87 to i32
  br label %mergeBB66

falseBB:                                          ; preds = %for.cond
  br label %mergeBB66

mergeBB66:                                        ; preds = %falseBB, %nextBB64
  %89 = phi i32 [ %88, %nextBB64 ], [ 0, %falseBB ]
  %90 = icmp ne i32 %89, 0
  br i1 %90, label %for.body, label %for.last

for.body:                                         ; preds = %mergeBB66
  %data67 = load [2048 x [2048 x i32]], ptr @15, align 4
  %cur_param_ptr68 = load i32, ptr %cur_param_ptr, align 4
  %91 = getelementptr inbounds [2048 x i32], ptr @15, i32 %cur_param_ptr68
  %92 = load [2048 x i32], ptr %91, align 4
  %DATA_VALUE69 = load i32, ptr @10, align 4
  %93 = getelementptr inbounds i32, ptr %91, i32 %DATA_VALUE69
  %94 = load i32, ptr %93, align 4
  store i32 %94, ptr %param_sym_ptr, align 4
  %cur_arg_ptr71 = load i32, ptr %cur_arg_ptr, align 4
  %env_ptr72 = load i32, ptr %env_ptr2, align 4
  %95 = call i32 @eval(i32 %cur_arg_ptr71, i32 %env_ptr72)
  store i32 %95, ptr %arg_ptr70, align 4
  %func_env_ptr73 = load i32, ptr %func_env_ptr, align 4
  %param_sym_ptr74 = load i32, ptr %param_sym_ptr, align 4
  %arg_ptr75 = load i32, ptr %arg_ptr70, align 4
  call void @env_add(i32 %func_env_ptr73, i32 %param_sym_ptr74, i32 %arg_ptr75)
  %cur_param_ptr76 = load i32, ptr %cur_param_ptr, align 4
  %data77 = load [2048 x [2048 x i32]], ptr @15, align 4
  %cur_param_ptr78 = load i32, ptr %cur_param_ptr, align 4
  %96 = getelementptr inbounds [2048 x i32], ptr @15, i32 %cur_param_ptr78
  %97 = load [2048 x i32], ptr %96, align 4
  %DATA_NEXT79 = load i32, ptr @11, align 4
  %98 = getelementptr inbounds i32, ptr %96, i32 %DATA_NEXT79
  %99 = load i32, ptr %98, align 4
  store i32 %99, ptr %cur_param_ptr, align 4
  %cur_arg_ptr80 = load i32, ptr %cur_arg_ptr, align 4
  %data81 = load [2048 x [2048 x i32]], ptr @15, align 4
  %cur_arg_ptr82 = load i32, ptr %cur_arg_ptr, align 4
  %100 = getelementptr inbounds [2048 x i32], ptr @15, i32 %cur_arg_ptr82
  %101 = load [2048 x i32], ptr %100, align 4
  %DATA_NEXT83 = load i32, ptr @11, align 4
  %102 = getelementptr inbounds i32, ptr %100, i32 %DATA_NEXT83
  %103 = load i32, ptr %102, align 4
  store i32 %103, ptr %cur_arg_ptr, align 4
  br label %for.inc

for.inc:                                          ; preds = %for.body
  br label %for.cond

for.last:                                         ; preds = %mergeBB66
  %cur_param_ptr84 = load i32, ptr %cur_param_ptr, align 4
  %104 = icmp ne i32 %cur_param_ptr84, 0
  br i1 %104, label %trueBB87, label %nextBB85

nextBB85:                                         ; preds = %for.last
  %cur_arg_ptr86 = load i32, ptr %cur_arg_ptr, align 4
  %105 = icmp ne i32 %cur_arg_ptr86, 0
  %106 = zext i1 %105 to i32
  br label %mergeBB88

trueBB87:                                         ; preds = %for.last
  br label %mergeBB88

mergeBB88:                                        ; preds = %trueBB87, %nextBB85
  %107 = phi i32 [ %106, %nextBB85 ], [ 1, %trueBB87 ]
  %108 = icmp ne i32 %107, 0
  br i1 %108, label %then89, label %last90

then89:                                           ; preds = %mergeBB88
  %ERR_INVALID_ARG_NUM = load i32, ptr @6, align 4
  %109 = call i32 @panic(i32 %ERR_INVALID_ARG_NUM)
  br label %last90

last90:                                           ; preds = %then89, %mergeBB88
  %body_ptr91 = load i32, ptr %body_ptr, align 4
  %func_env_ptr92 = load i32, ptr %func_env_ptr, align 4
  %110 = call i32 @eval(i32 %body_ptr91, i32 %func_env_ptr92)
  store i32 %110, ptr %result_ptr, align 4
  %func_env_ptr93 = load i32, ptr %func_env_ptr, align 4
  call void @free_data(i32 %func_env_ptr93)
  %func_ptr94 = load i32, ptr %func_ptr, align 4
  call void @free_data(i32 %func_ptr94)
  %result_ptr95 = load i32, ptr %result_ptr, align 4
  ret i32 %result_ptr95

last96:                                           ; preds = %mergeBB
  %data97 = load [2048 x [2048 x i32]], ptr @15, align 4
  %head_ptr98 = load i32, ptr %head_ptr, align 4
  %111 = getelementptr inbounds [2048 x i32], ptr @15, i32 %head_ptr98
  %112 = load [2048 x i32], ptr %111, align 4
  %DATA_VALUE99 = load i32, ptr @10, align 4
  %113 = getelementptr inbounds i32, ptr %111, i32 %DATA_VALUE99
  %114 = load i32, ptr %113, align 4
  store i32 %114, ptr %sym_ptr, align 4
  %sym_ptr100 = load i32, ptr %sym_ptr, align 4
  %SYM_QUOTE = load i32, ptr @25, align 4
  %115 = icmp eq i32 %sym_ptr100, %SYM_QUOTE
  br i1 %115, label %then101, label %last113

then101:                                          ; preds = %last96
  %arg_ptr102 = load i32, ptr %arg_ptr, align 4
  %116 = icmp ne i32 %arg_ptr102, 0
  %117 = xor i1 %116, true
  %118 = zext i1 %117 to i32
  %119 = icmp ne i32 %118, 0
  br i1 %119, label %trueBB107, label %nextBB103

nextBB103:                                        ; preds = %then101
  %data104 = load [2048 x [2048 x i32]], ptr @15, align 4
  %arg_ptr105 = load i32, ptr %arg_ptr, align 4
  %120 = getelementptr inbounds [2048 x i32], ptr @15, i32 %arg_ptr105
  %121 = load [2048 x i32], ptr %120, align 4
  %DATA_NEXT106 = load i32, ptr @11, align 4
  %122 = getelementptr inbounds i32, ptr %120, i32 %DATA_NEXT106
  %123 = load i32, ptr %122, align 4
  %124 = icmp ne i32 %123, 0
  %125 = zext i1 %124 to i32
  br label %mergeBB108

trueBB107:                                        ; preds = %then101
  br label %mergeBB108

mergeBB108:                                       ; preds = %trueBB107, %nextBB103
  %126 = phi i32 [ %125, %nextBB103 ], [ 1, %trueBB107 ]
  %127 = icmp ne i32 %126, 0
  br i1 %127, label %then109, label %last111

then109:                                          ; preds = %mergeBB108
  %ERR_INVALID_ARG_NUM110 = load i32, ptr @6, align 4
  %128 = call i32 @panic(i32 %ERR_INVALID_ARG_NUM110)
  br label %last111

last111:                                          ; preds = %then109, %mergeBB108
  %arg_ptr112 = load i32, ptr %arg_ptr, align 4
  %129 = call i32 @copy_ptr(i32 %arg_ptr112)
  ret i32 %129

last113:                                          ; preds = %last96
  %sym_ptr114 = load i32, ptr %sym_ptr, align 4
  %SYM_ATOM = load i32, ptr @26, align 4
  %130 = icmp eq i32 %sym_ptr114, %SYM_ATOM
  br i1 %130, label %then115, label %last135

then115:                                          ; preds = %last113
  %arg_ptr116 = load i32, ptr %arg_ptr, align 4
  %131 = icmp ne i32 %arg_ptr116, 0
  %132 = xor i1 %131, true
  %133 = zext i1 %132 to i32
  %134 = icmp ne i32 %133, 0
  br i1 %134, label %trueBB121, label %nextBB117

nextBB117:                                        ; preds = %then115
  %data118 = load [2048 x [2048 x i32]], ptr @15, align 4
  %arg_ptr119 = load i32, ptr %arg_ptr, align 4
  %135 = getelementptr inbounds [2048 x i32], ptr @15, i32 %arg_ptr119
  %136 = load [2048 x i32], ptr %135, align 4
  %DATA_NEXT120 = load i32, ptr @11, align 4
  %137 = getelementptr inbounds i32, ptr %135, i32 %DATA_NEXT120
  %138 = load i32, ptr %137, align 4
  %139 = icmp ne i32 %138, 0
  %140 = zext i1 %139 to i32
  br label %mergeBB122

trueBB121:                                        ; preds = %then115
  br label %mergeBB122

mergeBB122:                                       ; preds = %trueBB121, %nextBB117
  %141 = phi i32 [ %140, %nextBB117 ], [ 1, %trueBB121 ]
  %142 = icmp ne i32 %141, 0
  br i1 %142, label %then123, label %last125

then123:                                          ; preds = %mergeBB122
  %ERR_INVALID_ARG_NUM124 = load i32, ptr @6, align 4
  %143 = call i32 @panic(i32 %ERR_INVALID_ARG_NUM124)
  br label %last125

last125:                                          ; preds = %then123, %mergeBB122
  %arg_ptr126 = load i32, ptr %arg_ptr, align 4
  %env_ptr127 = load i32, ptr %env_ptr2, align 4
  %144 = call i32 @eval(i32 %arg_ptr126, i32 %env_ptr127)
  store i32 %144, ptr %value_ptr, align 4
  %data129 = load [2048 x [2048 x i32]], ptr @15, align 4
  %value_ptr130 = load i32, ptr %value_ptr, align 4
  %145 = getelementptr inbounds [2048 x i32], ptr @15, i32 %value_ptr130
  %146 = load [2048 x i32], ptr %145, align 4
  %DATA_TYPE131 = load i32, ptr @9, align 4
  %147 = getelementptr inbounds i32, ptr %145, i32 %DATA_TYPE131
  %148 = load i32, ptr %147, align 4
  %DATA_TYPE_LIST132 = load i32, ptr @19, align 4
  %149 = icmp ne i32 %148, %DATA_TYPE_LIST132
  %150 = sext i1 %149 to i32
  %151 = call i32 @make_bool(i32 %150)
  store i32 %151, ptr %result_ptr128, align 4
  %value_ptr133 = load i32, ptr %value_ptr, align 4
  call void @free_data(i32 %value_ptr133)
  %result_ptr134 = load i32, ptr %result_ptr128, align 4
  ret i32 %result_ptr134

last135:                                          ; preds = %last113
  %sym_ptr136 = load i32, ptr %sym_ptr, align 4
  %SYM_NUMBER = load i32, ptr @27, align 4
  %152 = icmp eq i32 %sym_ptr136, %SYM_NUMBER
  br i1 %152, label %then137, label %last158

then137:                                          ; preds = %last135
  %arg_ptr138 = load i32, ptr %arg_ptr, align 4
  %153 = icmp ne i32 %arg_ptr138, 0
  %154 = xor i1 %153, true
  %155 = zext i1 %154 to i32
  %156 = icmp ne i32 %155, 0
  br i1 %156, label %trueBB143, label %nextBB139

nextBB139:                                        ; preds = %then137
  %data140 = load [2048 x [2048 x i32]], ptr @15, align 4
  %arg_ptr141 = load i32, ptr %arg_ptr, align 4
  %157 = getelementptr inbounds [2048 x i32], ptr @15, i32 %arg_ptr141
  %158 = load [2048 x i32], ptr %157, align 4
  %DATA_NEXT142 = load i32, ptr @11, align 4
  %159 = getelementptr inbounds i32, ptr %157, i32 %DATA_NEXT142
  %160 = load i32, ptr %159, align 4
  %161 = icmp ne i32 %160, 0
  %162 = zext i1 %161 to i32
  br label %mergeBB144

trueBB143:                                        ; preds = %then137
  br label %mergeBB144

mergeBB144:                                       ; preds = %trueBB143, %nextBB139
  %163 = phi i32 [ %162, %nextBB139 ], [ 1, %trueBB143 ]
  %164 = icmp ne i32 %163, 0
  br i1 %164, label %then145, label %last147

then145:                                          ; preds = %mergeBB144
  %ERR_INVALID_ARG_NUM146 = load i32, ptr @6, align 4
  %165 = call i32 @panic(i32 %ERR_INVALID_ARG_NUM146)
  br label %last147

last147:                                          ; preds = %then145, %mergeBB144
  %arg_ptr149 = load i32, ptr %arg_ptr, align 4
  %env_ptr150 = load i32, ptr %env_ptr2, align 4
  %166 = call i32 @eval(i32 %arg_ptr149, i32 %env_ptr150)
  store i32 %166, ptr %value_ptr148, align 4
  %data152 = load [2048 x [2048 x i32]], ptr @15, align 4
  %value_ptr153 = load i32, ptr %value_ptr148, align 4
  %167 = getelementptr inbounds [2048 x i32], ptr @15, i32 %value_ptr153
  %168 = load [2048 x i32], ptr %167, align 4
  %DATA_TYPE154 = load i32, ptr @9, align 4
  %169 = getelementptr inbounds i32, ptr %167, i32 %DATA_TYPE154
  %170 = load i32, ptr %169, align 4
  %DATA_TYPE_NUMBER155 = load i32, ptr @18, align 4
  %171 = icmp eq i32 %170, %DATA_TYPE_NUMBER155
  %172 = sext i1 %171 to i32
  %173 = call i32 @make_bool(i32 %172)
  store i32 %173, ptr %result_ptr151, align 4
  %value_ptr156 = load i32, ptr %value_ptr148, align 4
  call void @free_data(i32 %value_ptr156)
  %result_ptr157 = load i32, ptr %result_ptr151, align 4
  ret i32 %result_ptr157

last158:                                          ; preds = %last135
  %sym_ptr159 = load i32, ptr %sym_ptr, align 4
  %SYM_EQ = load i32, ptr @28, align 4
  %174 = icmp eq i32 %sym_ptr159, %SYM_EQ
  br i1 %174, label %then160, label %last188

then160:                                          ; preds = %last158
  %data161 = load [2048 x [2048 x i32]], ptr @15, align 4
  %arg_ptr162 = load i32, ptr %arg_ptr, align 4
  %175 = getelementptr inbounds [2048 x i32], ptr @15, i32 %arg_ptr162
  %176 = load [2048 x i32], ptr %175, align 4
  %DATA_NEXT163 = load i32, ptr @11, align 4
  %177 = getelementptr inbounds i32, ptr %175, i32 %DATA_NEXT163
  %178 = load i32, ptr %177, align 4
  store i32 %178, ptr %arg2_ptr, align 4
  %arg_ptr164 = load i32, ptr %arg_ptr, align 4
  %179 = icmp ne i32 %arg_ptr164, 0
  %180 = xor i1 %179, true
  %181 = zext i1 %180 to i32
  %182 = icmp ne i32 %181, 0
  br i1 %182, label %trueBB167, label %nextBB165

nextBB165:                                        ; preds = %then160
  %arg2_ptr166 = load i32, ptr %arg2_ptr, align 4
  %183 = icmp ne i32 %arg2_ptr166, 0
  %184 = xor i1 %183, true
  %185 = zext i1 %184 to i32
  %186 = icmp ne i32 %185, 0
  %187 = zext i1 %186 to i32
  br label %mergeBB168

trueBB167:                                        ; preds = %then160
  br label %mergeBB168

mergeBB168:                                       ; preds = %trueBB167, %nextBB165
  %188 = phi i32 [ %187, %nextBB165 ], [ 1, %trueBB167 ]
  %189 = icmp ne i32 %188, 0
  br i1 %189, label %trueBB173, label %nextBB169

nextBB169:                                        ; preds = %mergeBB168
  %data170 = load [2048 x [2048 x i32]], ptr @15, align 4
  %arg2_ptr171 = load i32, ptr %arg2_ptr, align 4
  %190 = getelementptr inbounds [2048 x i32], ptr @15, i32 %arg2_ptr171
  %191 = load [2048 x i32], ptr %190, align 4
  %DATA_NEXT172 = load i32, ptr @11, align 4
  %192 = getelementptr inbounds i32, ptr %190, i32 %DATA_NEXT172
  %193 = load i32, ptr %192, align 4
  %194 = icmp ne i32 %193, 0
  %195 = zext i1 %194 to i32
  br label %mergeBB174

trueBB173:                                        ; preds = %mergeBB168
  br label %mergeBB174

mergeBB174:                                       ; preds = %trueBB173, %nextBB169
  %196 = phi i32 [ %195, %nextBB169 ], [ 1, %trueBB173 ]
  %197 = icmp ne i32 %196, 0
  br i1 %197, label %then175, label %last177

then175:                                          ; preds = %mergeBB174
  %ERR_INVALID_ARG_NUM176 = load i32, ptr @6, align 4
  %198 = call i32 @panic(i32 %ERR_INVALID_ARG_NUM176)
  br label %last177

last177:                                          ; preds = %then175, %mergeBB174
  %arg_ptr178 = load i32, ptr %arg_ptr, align 4
  %env_ptr179 = load i32, ptr %env_ptr2, align 4
  %199 = call i32 @eval(i32 %arg_ptr178, i32 %env_ptr179)
  store i32 %199, ptr %value1_ptr, align 4
  %arg2_ptr180 = load i32, ptr %arg2_ptr, align 4
  %env_ptr181 = load i32, ptr %env_ptr2, align 4
  %200 = call i32 @eval(i32 %arg2_ptr180, i32 %env_ptr181)
  store i32 %200, ptr %value2_ptr, align 4
  %value1_ptr183 = load i32, ptr %value1_ptr, align 4
  %value2_ptr184 = load i32, ptr %value2_ptr, align 4
  %201 = call i32 @is_equal(i32 %value1_ptr183, i32 %value2_ptr184)
  %202 = call i32 @make_bool(i32 %201)
  store i32 %202, ptr %result_ptr182, align 4
  %value1_ptr185 = load i32, ptr %value1_ptr, align 4
  call void @free_data(i32 %value1_ptr185)
  %value2_ptr186 = load i32, ptr %value2_ptr, align 4
  call void @free_data(i32 %value2_ptr186)
  %result_ptr187 = load i32, ptr %result_ptr182, align 4
  ret i32 %result_ptr187

last188:                                          ; preds = %last158
  %sym_ptr189 = load i32, ptr %sym_ptr, align 4
  %SYM_CAR = load i32, ptr @29, align 4
  %203 = icmp eq i32 %sym_ptr189, %SYM_CAR
  br i1 %203, label %then190, label %last212

then190:                                          ; preds = %last188
  %arg_ptr191 = load i32, ptr %arg_ptr, align 4
  %204 = icmp ne i32 %arg_ptr191, 0
  %205 = xor i1 %204, true
  %206 = zext i1 %205 to i32
  %207 = icmp ne i32 %206, 0
  br i1 %207, label %trueBB196, label %nextBB192

nextBB192:                                        ; preds = %then190
  %data193 = load [2048 x [2048 x i32]], ptr @15, align 4
  %arg_ptr194 = load i32, ptr %arg_ptr, align 4
  %208 = getelementptr inbounds [2048 x i32], ptr @15, i32 %arg_ptr194
  %209 = load [2048 x i32], ptr %208, align 4
  %DATA_NEXT195 = load i32, ptr @11, align 4
  %210 = getelementptr inbounds i32, ptr %208, i32 %DATA_NEXT195
  %211 = load i32, ptr %210, align 4
  %212 = icmp ne i32 %211, 0
  %213 = zext i1 %212 to i32
  br label %mergeBB197

trueBB196:                                        ; preds = %then190
  br label %mergeBB197

mergeBB197:                                       ; preds = %trueBB196, %nextBB192
  %214 = phi i32 [ %213, %nextBB192 ], [ 1, %trueBB196 ]
  %215 = icmp ne i32 %214, 0
  br i1 %215, label %then198, label %last200

then198:                                          ; preds = %mergeBB197
  %ERR_INVALID_ARG_NUM199 = load i32, ptr @6, align 4
  %216 = call i32 @panic(i32 %ERR_INVALID_ARG_NUM199)
  br label %last200

last200:                                          ; preds = %then198, %mergeBB197
  %arg_ptr202 = load i32, ptr %arg_ptr, align 4
  %env_ptr203 = load i32, ptr %env_ptr2, align 4
  %217 = call i32 @eval(i32 %arg_ptr202, i32 %env_ptr203)
  store i32 %217, ptr %value_ptr201, align 4
  %value_ptr205 = load i32, ptr %value_ptr201, align 4
  %218 = call i32 @unwrap_list(i32 %value_ptr205)
  %219 = call i32 @copy_ptr(i32 %218)
  store i32 %219, ptr %result_ptr204, align 4
  %result_ptr206 = load i32, ptr %result_ptr204, align 4
  %220 = icmp ne i32 %result_ptr206, 0
  %221 = xor i1 %220, true
  %222 = zext i1 %221 to i32
  %223 = icmp ne i32 %222, 0
  br i1 %223, label %then207, label %last209

then207:                                          ; preds = %last200
  %ERR_INVALID_LIST208 = load i32, ptr @4, align 4
  %224 = call i32 @panic(i32 %ERR_INVALID_LIST208)
  br label %last209

last209:                                          ; preds = %then207, %last200
  %value_ptr210 = load i32, ptr %value_ptr201, align 4
  call void @free_data(i32 %value_ptr210)
  %result_ptr211 = load i32, ptr %result_ptr204, align 4
  ret i32 %result_ptr211

last212:                                          ; preds = %last188
  %sym_ptr213 = load i32, ptr %sym_ptr, align 4
  %SYM_CDR = load i32, ptr @30, align 4
  %225 = icmp eq i32 %sym_ptr213, %SYM_CDR
  br i1 %225, label %then214, label %last240

then214:                                          ; preds = %last212
  %arg_ptr215 = load i32, ptr %arg_ptr, align 4
  %226 = icmp ne i32 %arg_ptr215, 0
  %227 = xor i1 %226, true
  %228 = zext i1 %227 to i32
  %229 = icmp ne i32 %228, 0
  br i1 %229, label %trueBB220, label %nextBB216

nextBB216:                                        ; preds = %then214
  %data217 = load [2048 x [2048 x i32]], ptr @15, align 4
  %arg_ptr218 = load i32, ptr %arg_ptr, align 4
  %230 = getelementptr inbounds [2048 x i32], ptr @15, i32 %arg_ptr218
  %231 = load [2048 x i32], ptr %230, align 4
  %DATA_NEXT219 = load i32, ptr @11, align 4
  %232 = getelementptr inbounds i32, ptr %230, i32 %DATA_NEXT219
  %233 = load i32, ptr %232, align 4
  %234 = icmp ne i32 %233, 0
  %235 = zext i1 %234 to i32
  br label %mergeBB221

trueBB220:                                        ; preds = %then214
  br label %mergeBB221

mergeBB221:                                       ; preds = %trueBB220, %nextBB216
  %236 = phi i32 [ %235, %nextBB216 ], [ 1, %trueBB220 ]
  %237 = icmp ne i32 %236, 0
  br i1 %237, label %then222, label %last224

then222:                                          ; preds = %mergeBB221
  %ERR_INVALID_ARG_NUM223 = load i32, ptr @6, align 4
  %238 = call i32 @panic(i32 %ERR_INVALID_ARG_NUM223)
  br label %last224

last224:                                          ; preds = %then222, %mergeBB221
  %arg_ptr226 = load i32, ptr %arg_ptr, align 4
  %env_ptr227 = load i32, ptr %env_ptr2, align 4
  %239 = call i32 @eval(i32 %arg_ptr226, i32 %env_ptr227)
  store i32 %239, ptr %value_ptr225, align 4
  %value_ptr229 = load i32, ptr %value_ptr225, align 4
  %240 = call i32 @unwrap_list(i32 %value_ptr229)
  store i32 %240, ptr %head_ptr228, align 4
  %head_ptr230 = load i32, ptr %head_ptr228, align 4
  %241 = icmp ne i32 %head_ptr230, 0
  %242 = xor i1 %241, true
  %243 = zext i1 %242 to i32
  %244 = icmp ne i32 %243, 0
  br i1 %244, label %then231, label %last233

then231:                                          ; preds = %last224
  %ERR_INVALID_LIST232 = load i32, ptr @4, align 4
  %245 = call i32 @panic(i32 %ERR_INVALID_LIST232)
  br label %last233

last233:                                          ; preds = %then231, %last224
  %data235 = load [2048 x [2048 x i32]], ptr @15, align 4
  %head_ptr236 = load i32, ptr %head_ptr228, align 4
  %246 = getelementptr inbounds [2048 x i32], ptr @15, i32 %head_ptr236
  %247 = load [2048 x i32], ptr %246, align 4
  %DATA_NEXT237 = load i32, ptr @11, align 4
  %248 = getelementptr inbounds i32, ptr %246, i32 %DATA_NEXT237
  %249 = load i32, ptr %248, align 4
  %250 = call i32 @copy_ptr(i32 %249)
  %251 = call i32 @make_list(i32 %250)
  store i32 %251, ptr %result_ptr234, align 4
  %value_ptr238 = load i32, ptr %value_ptr225, align 4
  call void @free_data(i32 %value_ptr238)
  %result_ptr239 = load i32, ptr %result_ptr234, align 4
  ret i32 %result_ptr239

last240:                                          ; preds = %last212
  %sym_ptr241 = load i32, ptr %sym_ptr, align 4
  %SYM_CONS = load i32, ptr @31, align 4
  %252 = icmp eq i32 %sym_ptr241, %SYM_CONS
  br i1 %252, label %then242, label %last278

then242:                                          ; preds = %last240
  %data244 = load [2048 x [2048 x i32]], ptr @15, align 4
  %arg_ptr245 = load i32, ptr %arg_ptr, align 4
  %253 = getelementptr inbounds [2048 x i32], ptr @15, i32 %arg_ptr245
  %254 = load [2048 x i32], ptr %253, align 4
  %DATA_NEXT246 = load i32, ptr @11, align 4
  %255 = getelementptr inbounds i32, ptr %253, i32 %DATA_NEXT246
  %256 = load i32, ptr %255, align 4
  store i32 %256, ptr %arg2_ptr243, align 4
  %arg_ptr247 = load i32, ptr %arg_ptr, align 4
  %257 = icmp ne i32 %arg_ptr247, 0
  %258 = xor i1 %257, true
  %259 = zext i1 %258 to i32
  %260 = icmp ne i32 %259, 0
  br i1 %260, label %trueBB250, label %nextBB248

nextBB248:                                        ; preds = %then242
  %arg2_ptr249 = load i32, ptr %arg2_ptr243, align 4
  %261 = icmp ne i32 %arg2_ptr249, 0
  %262 = xor i1 %261, true
  %263 = zext i1 %262 to i32
  %264 = icmp ne i32 %263, 0
  %265 = zext i1 %264 to i32
  br label %mergeBB251

trueBB250:                                        ; preds = %then242
  br label %mergeBB251

mergeBB251:                                       ; preds = %trueBB250, %nextBB248
  %266 = phi i32 [ %265, %nextBB248 ], [ 1, %trueBB250 ]
  %267 = icmp ne i32 %266, 0
  br i1 %267, label %trueBB256, label %nextBB252

nextBB252:                                        ; preds = %mergeBB251
  %data253 = load [2048 x [2048 x i32]], ptr @15, align 4
  %arg2_ptr254 = load i32, ptr %arg2_ptr243, align 4
  %268 = getelementptr inbounds [2048 x i32], ptr @15, i32 %arg2_ptr254
  %269 = load [2048 x i32], ptr %268, align 4
  %DATA_NEXT255 = load i32, ptr @11, align 4
  %270 = getelementptr inbounds i32, ptr %268, i32 %DATA_NEXT255
  %271 = load i32, ptr %270, align 4
  %272 = icmp ne i32 %271, 0
  %273 = zext i1 %272 to i32
  br label %mergeBB257

trueBB256:                                        ; preds = %mergeBB251
  br label %mergeBB257

mergeBB257:                                       ; preds = %trueBB256, %nextBB252
  %274 = phi i32 [ %273, %nextBB252 ], [ 1, %trueBB256 ]
  %275 = icmp ne i32 %274, 0
  br i1 %275, label %then258, label %last260

then258:                                          ; preds = %mergeBB257
  %ERR_INVALID_ARG_NUM259 = load i32, ptr @6, align 4
  %276 = call i32 @panic(i32 %ERR_INVALID_ARG_NUM259)
  br label %last260

last260:                                          ; preds = %then258, %mergeBB257
  %arg_ptr262 = load i32, ptr %arg_ptr, align 4
  %env_ptr263 = load i32, ptr %env_ptr2, align 4
  %277 = call i32 @eval(i32 %arg_ptr262, i32 %env_ptr263)
  store i32 %277, ptr %value1_ptr261, align 4
  %arg2_ptr265 = load i32, ptr %arg2_ptr243, align 4
  %env_ptr266 = load i32, ptr %env_ptr2, align 4
  %278 = call i32 @eval(i32 %arg2_ptr265, i32 %env_ptr266)
  store i32 %278, ptr %value2_ptr264, align 4
  %value1_ptr268 = load i32, ptr %value1_ptr261, align 4
  %279 = call i32 @copy_data(i32 %value1_ptr268)
  store i32 %279, ptr %head_ptr267, align 4
  %data269 = load [2048 x [2048 x i32]], ptr @15, align 4
  %head_ptr270 = load i32, ptr %head_ptr267, align 4
  %280 = getelementptr inbounds [2048 x i32], ptr @15, i32 %head_ptr270
  %281 = load [2048 x i32], ptr %280, align 4
  %DATA_NEXT271 = load i32, ptr @11, align 4
  %282 = getelementptr inbounds i32, ptr %280, i32 %DATA_NEXT271
  %283 = load i32, ptr %282, align 4
  %value2_ptr272 = load i32, ptr %value2_ptr264, align 4
  %284 = call i32 @unwrap_list(i32 %value2_ptr272)
  %285 = call i32 @copy_ptr(i32 %284)
  store i32 %285, ptr %282, align 4
  %head_ptr274 = load i32, ptr %head_ptr267, align 4
  %286 = call i32 @make_list(i32 %head_ptr274)
  store i32 %286, ptr %result_ptr273, align 4
  %value1_ptr275 = load i32, ptr %value1_ptr261, align 4
  call void @free_data(i32 %value1_ptr275)
  %value2_ptr276 = load i32, ptr %value2_ptr264, align 4
  call void @free_data(i32 %value2_ptr276)
  %result_ptr277 = load i32, ptr %result_ptr273, align 4
  ret i32 %result_ptr277

last278:                                          ; preds = %last240
  %sym_ptr279 = load i32, ptr %sym_ptr, align 4
  %SYM_COND = load i32, ptr @32, align 4
  %287 = icmp eq i32 %sym_ptr279, %SYM_COND
  br i1 %287, label %then280, label %last307

then280:                                          ; preds = %last278
  %arg_ptr281 = load i32, ptr %arg_ptr, align 4
  store i32 %arg_ptr281, ptr %cur_ptr, align 4
  br label %for.cond282

for.cond282:                                      ; preds = %for.inc305, %then280
  %cur_ptr283 = load i32, ptr %cur_ptr, align 4
  %288 = icmp ne i32 %cur_ptr283, 0
  br i1 %288, label %for.body284, label %for.last306

for.body284:                                      ; preds = %for.cond282
  %cur_ptr285 = load i32, ptr %cur_ptr, align 4
  %289 = call i32 @unwrap_list(i32 %cur_ptr285)
  store i32 %289, ptr %test_ptr, align 4
  %test_ptr287 = load i32, ptr %test_ptr, align 4
  %env_ptr288 = load i32, ptr %env_ptr2, align 4
  %290 = call i32 @eval(i32 %test_ptr287, i32 %env_ptr288)
  store i32 %290, ptr %result_ptr286, align 4
  %result_ptr289 = load i32, ptr %result_ptr286, align 4
  %291 = call i32 @is_true(i32 %result_ptr289)
  %292 = icmp ne i32 %291, 0
  br i1 %292, label %then290, label %last299

then290:                                          ; preds = %for.body284
  %data292 = load [2048 x [2048 x i32]], ptr @15, align 4
  %test_ptr293 = load i32, ptr %test_ptr, align 4
  %293 = getelementptr inbounds [2048 x i32], ptr @15, i32 %test_ptr293
  %294 = load [2048 x i32], ptr %293, align 4
  %DATA_NEXT294 = load i32, ptr @11, align 4
  %295 = getelementptr inbounds i32, ptr %293, i32 %DATA_NEXT294
  %296 = load i32, ptr %295, align 4
  store i32 %296, ptr %body_ptr291, align 4
  %body_ptr295 = load i32, ptr %body_ptr291, align 4
  %env_ptr296 = load i32, ptr %env_ptr2, align 4
  %297 = call i32 @eval(i32 %body_ptr295, i32 %env_ptr296)
  store i32 %297, ptr %body_result_ptr, align 4
  %result_ptr297 = load i32, ptr %result_ptr286, align 4
  call void @free_data(i32 %result_ptr297)
  %body_result_ptr298 = load i32, ptr %body_result_ptr, align 4
  ret i32 %body_result_ptr298

last299:                                          ; preds = %for.body284
  %result_ptr300 = load i32, ptr %result_ptr286, align 4
  call void @free_data(i32 %result_ptr300)
  %cur_ptr301 = load i32, ptr %cur_ptr, align 4
  %data302 = load [2048 x [2048 x i32]], ptr @15, align 4
  %cur_ptr303 = load i32, ptr %cur_ptr, align 4
  %298 = getelementptr inbounds [2048 x i32], ptr @15, i32 %cur_ptr303
  %299 = load [2048 x i32], ptr %298, align 4
  %DATA_NEXT304 = load i32, ptr @11, align 4
  %300 = getelementptr inbounds i32, ptr %298, i32 %DATA_NEXT304
  %301 = load i32, ptr %300, align 4
  store i32 %301, ptr %cur_ptr, align 4
  br label %for.inc305

for.inc305:                                       ; preds = %last299
  br label %for.cond282

for.last306:                                      ; preds = %for.cond282
  ret i32 0

last307:                                          ; preds = %last278
  %sym_ptr308 = load i32, ptr %sym_ptr, align 4
  %SYM_LAMBDA = load i32, ptr @33, align 4
  %302 = icmp eq i32 %sym_ptr308, %SYM_LAMBDA
  br i1 %302, label %then309, label %last333

then309:                                          ; preds = %last307
  %data311 = load [2048 x [2048 x i32]], ptr @15, align 4
  %arg_ptr312 = load i32, ptr %arg_ptr, align 4
  %303 = getelementptr inbounds [2048 x i32], ptr @15, i32 %arg_ptr312
  %304 = load [2048 x i32], ptr %303, align 4
  %DATA_NEXT313 = load i32, ptr @11, align 4
  %305 = getelementptr inbounds i32, ptr %303, i32 %DATA_NEXT313
  %306 = load i32, ptr %305, align 4
  store i32 %306, ptr %arg2_ptr310, align 4
  %arg_ptr314 = load i32, ptr %arg_ptr, align 4
  %307 = icmp ne i32 %arg_ptr314, 0
  %308 = xor i1 %307, true
  %309 = zext i1 %308 to i32
  %310 = icmp ne i32 %309, 0
  br i1 %310, label %trueBB317, label %nextBB315

nextBB315:                                        ; preds = %then309
  %arg2_ptr316 = load i32, ptr %arg2_ptr310, align 4
  %311 = icmp ne i32 %arg2_ptr316, 0
  %312 = xor i1 %311, true
  %313 = zext i1 %312 to i32
  %314 = icmp ne i32 %313, 0
  %315 = zext i1 %314 to i32
  br label %mergeBB318

trueBB317:                                        ; preds = %then309
  br label %mergeBB318

mergeBB318:                                       ; preds = %trueBB317, %nextBB315
  %316 = phi i32 [ %315, %nextBB315 ], [ 1, %trueBB317 ]
  %317 = icmp ne i32 %316, 0
  br i1 %317, label %trueBB323, label %nextBB319

nextBB319:                                        ; preds = %mergeBB318
  %data320 = load [2048 x [2048 x i32]], ptr @15, align 4
  %arg2_ptr321 = load i32, ptr %arg2_ptr310, align 4
  %318 = getelementptr inbounds [2048 x i32], ptr @15, i32 %arg2_ptr321
  %319 = load [2048 x i32], ptr %318, align 4
  %DATA_NEXT322 = load i32, ptr @11, align 4
  %320 = getelementptr inbounds i32, ptr %318, i32 %DATA_NEXT322
  %321 = load i32, ptr %320, align 4
  %322 = icmp ne i32 %321, 0
  %323 = zext i1 %322 to i32
  br label %mergeBB324

trueBB323:                                        ; preds = %mergeBB318
  br label %mergeBB324

mergeBB324:                                       ; preds = %trueBB323, %nextBB319
  %324 = phi i32 [ %323, %nextBB319 ], [ 1, %trueBB323 ]
  %325 = icmp ne i32 %324, 0
  br i1 %325, label %then325, label %last327

then325:                                          ; preds = %mergeBB324
  %ERR_INVALID_ARG_NUM326 = load i32, ptr @6, align 4
  %326 = call i32 @panic(i32 %ERR_INVALID_ARG_NUM326)
  br label %last327

last327:                                          ; preds = %then325, %mergeBB324
  %arg_ptr329 = load i32, ptr %arg_ptr, align 4
  %327 = call i32 @copy_data(i32 %arg_ptr329)
  %arg2_ptr330 = load i32, ptr %arg2_ptr310, align 4
  %328 = call i32 @copy_data(i32 %arg2_ptr330)
  %env_ptr331 = load i32, ptr %env_ptr2, align 4
  %329 = call i32 @copy_ptr(i32 %env_ptr331)
  %330 = call i32 @make_func(i32 %327, i32 %328, i32 %329)
  store i32 %330, ptr %result_ptr328, align 4
  %result_ptr332 = load i32, ptr %result_ptr328, align 4
  ret i32 %result_ptr332

last333:                                          ; preds = %last307
  %sym_ptr334 = load i32, ptr %sym_ptr, align 4
  %SYM_DEFINE = load i32, ptr @34, align 4
  %331 = icmp eq i32 %sym_ptr334, %SYM_DEFINE
  br i1 %331, label %then335, label %last362

then335:                                          ; preds = %last333
  %data337 = load [2048 x [2048 x i32]], ptr @15, align 4
  %arg_ptr338 = load i32, ptr %arg_ptr, align 4
  %332 = getelementptr inbounds [2048 x i32], ptr @15, i32 %arg_ptr338
  %333 = load [2048 x i32], ptr %332, align 4
  %DATA_NEXT339 = load i32, ptr @11, align 4
  %334 = getelementptr inbounds i32, ptr %332, i32 %DATA_NEXT339
  %335 = load i32, ptr %334, align 4
  store i32 %335, ptr %arg2_ptr336, align 4
  %arg_ptr340 = load i32, ptr %arg_ptr, align 4
  %336 = icmp ne i32 %arg_ptr340, 0
  %337 = xor i1 %336, true
  %338 = zext i1 %337 to i32
  %339 = icmp ne i32 %338, 0
  br i1 %339, label %trueBB343, label %nextBB341

nextBB341:                                        ; preds = %then335
  %arg2_ptr342 = load i32, ptr %arg2_ptr336, align 4
  %340 = icmp ne i32 %arg2_ptr342, 0
  %341 = xor i1 %340, true
  %342 = zext i1 %341 to i32
  %343 = icmp ne i32 %342, 0
  %344 = zext i1 %343 to i32
  br label %mergeBB344

trueBB343:                                        ; preds = %then335
  br label %mergeBB344

mergeBB344:                                       ; preds = %trueBB343, %nextBB341
  %345 = phi i32 [ %344, %nextBB341 ], [ 1, %trueBB343 ]
  %346 = icmp ne i32 %345, 0
  br i1 %346, label %trueBB349, label %nextBB345

nextBB345:                                        ; preds = %mergeBB344
  %data346 = load [2048 x [2048 x i32]], ptr @15, align 4
  %arg2_ptr347 = load i32, ptr %arg2_ptr336, align 4
  %347 = getelementptr inbounds [2048 x i32], ptr @15, i32 %arg2_ptr347
  %348 = load [2048 x i32], ptr %347, align 4
  %DATA_NEXT348 = load i32, ptr @11, align 4
  %349 = getelementptr inbounds i32, ptr %347, i32 %DATA_NEXT348
  %350 = load i32, ptr %349, align 4
  %351 = icmp ne i32 %350, 0
  %352 = zext i1 %351 to i32
  br label %mergeBB350

trueBB349:                                        ; preds = %mergeBB344
  br label %mergeBB350

mergeBB350:                                       ; preds = %trueBB349, %nextBB345
  %353 = phi i32 [ %352, %nextBB345 ], [ 1, %trueBB349 ]
  %354 = icmp ne i32 %353, 0
  br i1 %354, label %then351, label %last353

then351:                                          ; preds = %mergeBB350
  %ERR_INVALID_ARG_NUM352 = load i32, ptr @6, align 4
  %355 = call i32 @panic(i32 %ERR_INVALID_ARG_NUM352)
  br label %last353

last353:                                          ; preds = %then351, %mergeBB350
  %arg_ptr355 = load i32, ptr %arg_ptr, align 4
  %356 = call i32 @unwrap_symbol(i32 %arg_ptr355)
  store i32 %356, ptr %sym_ptr354, align 4
  %arg2_ptr357 = load i32, ptr %arg2_ptr336, align 4
  %env_ptr358 = load i32, ptr %env_ptr2, align 4
  %357 = call i32 @eval(i32 %arg2_ptr357, i32 %env_ptr358)
  store i32 %357, ptr %value_ptr356, align 4
  %env_ptr359 = load i32, ptr %env_ptr2, align 4
  %sym_ptr360 = load i32, ptr %sym_ptr354, align 4
  %value_ptr361 = load i32, ptr %value_ptr356, align 4
  call void @env_set(i32 %env_ptr359, i32 %sym_ptr360, i32 %value_ptr361)
  ret i32 0

last362:                                          ; preds = %last333
  %sym_ptr363 = load i32, ptr %sym_ptr, align 4
  %SYM_LIST = load i32, ptr @37, align 4
  %358 = icmp eq i32 %sym_ptr363, %SYM_LIST
  br i1 %358, label %then364, label %last395

then364:                                          ; preds = %last362
  %359 = call i32 @make_list(i32 0)
  store i32 %359, ptr %result_ptr365, align 4
  store i32 0, ptr %cur_ptr366, align 4
  br label %for.cond367

for.cond367:                                      ; preds = %for.inc392, %then364
  %arg_ptr368 = load i32, ptr %arg_ptr, align 4
  %360 = icmp ne i32 %arg_ptr368, 0
  br i1 %360, label %for.body369, label %for.last393

for.body369:                                      ; preds = %for.cond367
  %arg_ptr371 = load i32, ptr %arg_ptr, align 4
  %env_ptr372 = load i32, ptr %env_ptr2, align 4
  %361 = call i32 @eval(i32 %arg_ptr371, i32 %env_ptr372)
  store i32 %361, ptr %value_ptr370, align 4
  %value_ptr373 = load i32, ptr %value_ptr370, align 4
  %362 = call i32 @copy_data(i32 %value_ptr373)
  store i32 %362, ptr %elem_ptr, align 4
  %value_ptr374 = load i32, ptr %value_ptr370, align 4
  call void @free_data(i32 %value_ptr374)
  %cur_ptr375 = load i32, ptr %cur_ptr366, align 4
  %363 = icmp ne i32 %cur_ptr375, 0
  br i1 %363, label %then376, label %else

then376:                                          ; preds = %for.body369
  %data377 = load [2048 x [2048 x i32]], ptr @15, align 4
  %cur_ptr378 = load i32, ptr %cur_ptr366, align 4
  %364 = getelementptr inbounds [2048 x i32], ptr @15, i32 %cur_ptr378
  %365 = load [2048 x i32], ptr %364, align 4
  %DATA_NEXT379 = load i32, ptr @11, align 4
  %366 = getelementptr inbounds i32, ptr %364, i32 %DATA_NEXT379
  %367 = load i32, ptr %366, align 4
  %elem_ptr380 = load i32, ptr %elem_ptr, align 4
  store i32 %elem_ptr380, ptr %366, align 4
  br label %last385

else:                                             ; preds = %for.body369
  %data381 = load [2048 x [2048 x i32]], ptr @15, align 4
  %result_ptr382 = load i32, ptr %result_ptr365, align 4
  %368 = getelementptr inbounds [2048 x i32], ptr @15, i32 %result_ptr382
  %369 = load [2048 x i32], ptr %368, align 4
  %DATA_VALUE383 = load i32, ptr @10, align 4
  %370 = getelementptr inbounds i32, ptr %368, i32 %DATA_VALUE383
  %371 = load i32, ptr %370, align 4
  %elem_ptr384 = load i32, ptr %elem_ptr, align 4
  store i32 %elem_ptr384, ptr %370, align 4
  br label %last385

last385:                                          ; preds = %else, %then376
  %cur_ptr386 = load i32, ptr %cur_ptr366, align 4
  %elem_ptr387 = load i32, ptr %elem_ptr, align 4
  store i32 %elem_ptr387, ptr %cur_ptr366, align 4
  %arg_ptr388 = load i32, ptr %arg_ptr, align 4
  %data389 = load [2048 x [2048 x i32]], ptr @15, align 4
  %arg_ptr390 = load i32, ptr %arg_ptr, align 4
  %372 = getelementptr inbounds [2048 x i32], ptr @15, i32 %arg_ptr390
  %373 = load [2048 x i32], ptr %372, align 4
  %DATA_NEXT391 = load i32, ptr @11, align 4
  %374 = getelementptr inbounds i32, ptr %372, i32 %DATA_NEXT391
  %375 = load i32, ptr %374, align 4
  store i32 %375, ptr %arg_ptr, align 4
  br label %for.inc392

for.inc392:                                       ; preds = %last385
  br label %for.cond367

for.last393:                                      ; preds = %for.cond367
  %result_ptr394 = load i32, ptr %result_ptr365, align 4
  ret i32 %result_ptr394

last395:                                          ; preds = %last362
  %data397 = load [2048 x [2048 x i32]], ptr @15, align 4
  %arg_ptr398 = load i32, ptr %arg_ptr, align 4
  %376 = getelementptr inbounds [2048 x i32], ptr @15, i32 %arg_ptr398
  %377 = load [2048 x i32], ptr %376, align 4
  %DATA_NEXT399 = load i32, ptr @11, align 4
  %378 = getelementptr inbounds i32, ptr %376, i32 %DATA_NEXT399
  %379 = load i32, ptr %378, align 4
  store i32 %379, ptr %arg2_ptr396, align 4
  %arg_ptr400 = load i32, ptr %arg_ptr, align 4
  %380 = icmp ne i32 %arg_ptr400, 0
  %381 = xor i1 %380, true
  %382 = zext i1 %381 to i32
  %383 = icmp ne i32 %382, 0
  br i1 %383, label %trueBB403, label %nextBB401

nextBB401:                                        ; preds = %last395
  %arg2_ptr402 = load i32, ptr %arg2_ptr396, align 4
  %384 = icmp ne i32 %arg2_ptr402, 0
  %385 = xor i1 %384, true
  %386 = zext i1 %385 to i32
  %387 = icmp ne i32 %386, 0
  %388 = zext i1 %387 to i32
  br label %mergeBB404

trueBB403:                                        ; preds = %last395
  br label %mergeBB404

mergeBB404:                                       ; preds = %trueBB403, %nextBB401
  %389 = phi i32 [ %388, %nextBB401 ], [ 1, %trueBB403 ]
  %390 = icmp ne i32 %389, 0
  br i1 %390, label %trueBB409, label %nextBB405

nextBB405:                                        ; preds = %mergeBB404
  %data406 = load [2048 x [2048 x i32]], ptr @15, align 4
  %arg2_ptr407 = load i32, ptr %arg2_ptr396, align 4
  %391 = getelementptr inbounds [2048 x i32], ptr @15, i32 %arg2_ptr407
  %392 = load [2048 x i32], ptr %391, align 4
  %DATA_NEXT408 = load i32, ptr @11, align 4
  %393 = getelementptr inbounds i32, ptr %391, i32 %DATA_NEXT408
  %394 = load i32, ptr %393, align 4
  %395 = icmp ne i32 %394, 0
  %396 = zext i1 %395 to i32
  br label %mergeBB410

trueBB409:                                        ; preds = %mergeBB404
  br label %mergeBB410

mergeBB410:                                       ; preds = %trueBB409, %nextBB405
  %397 = phi i32 [ %396, %nextBB405 ], [ 1, %trueBB409 ]
  %398 = icmp ne i32 %397, 0
  br i1 %398, label %then411, label %last413

then411:                                          ; preds = %mergeBB410
  %ERR_INVALID_ARG_NUM412 = load i32, ptr @6, align 4
  %399 = call i32 @panic(i32 %ERR_INVALID_ARG_NUM412)
  br label %last413

last413:                                          ; preds = %then411, %mergeBB410
  %arg_ptr415 = load i32, ptr %arg_ptr, align 4
  %env_ptr416 = load i32, ptr %env_ptr2, align 4
  %400 = call i32 @eval(i32 %arg_ptr415, i32 %env_ptr416)
  store i32 %400, ptr %value1_ptr414, align 4
  %arg2_ptr418 = load i32, ptr %arg2_ptr396, align 4
  %env_ptr419 = load i32, ptr %env_ptr2, align 4
  %401 = call i32 @eval(i32 %arg2_ptr418, i32 %env_ptr419)
  store i32 %401, ptr %value2_ptr417, align 4
  %value1_ptr420 = load i32, ptr %value1_ptr414, align 4
  %402 = call i32 @unwrap_number(i32 %value1_ptr420)
  store i32 %402, ptr %lhs, align 4
  %value2_ptr421 = load i32, ptr %value2_ptr417, align 4
  %403 = call i32 @unwrap_number(i32 %value2_ptr421)
  store i32 %403, ptr %rhs, align 4
  store i32 0, ptr %result_ptr422, align 4
  %sym_ptr423 = load i32, ptr %sym_ptr, align 4
  %SYM_ADD = load i32, ptr @38, align 4
  %404 = icmp eq i32 %sym_ptr423, %SYM_ADD
  br i1 %404, label %then424, label %else428

then424:                                          ; preds = %last413
  %result_ptr425 = load i32, ptr %result_ptr422, align 4
  %lhs426 = load i32, ptr %lhs, align 4
  %rhs427 = load i32, ptr %rhs, align 4
  %405 = add nsw i32 %lhs426, %rhs427
  %406 = call i32 @make_number(i32 %405)
  store i32 %406, ptr %result_ptr422, align 4
  br label %last485

else428:                                          ; preds = %last413
  %sym_ptr429 = load i32, ptr %sym_ptr, align 4
  %SYM_SUB = load i32, ptr @39, align 4
  %407 = icmp eq i32 %sym_ptr429, %SYM_SUB
  br i1 %407, label %then430, label %else434

then430:                                          ; preds = %else428
  %result_ptr431 = load i32, ptr %result_ptr422, align 4
  %lhs432 = load i32, ptr %lhs, align 4
  %rhs433 = load i32, ptr %rhs, align 4
  %408 = sub nsw i32 %lhs432, %rhs433
  %409 = call i32 @make_number(i32 %408)
  store i32 %409, ptr %result_ptr422, align 4
  br label %last484

else434:                                          ; preds = %else428
  %sym_ptr435 = load i32, ptr %sym_ptr, align 4
  %SYM_MUL = load i32, ptr @40, align 4
  %410 = icmp eq i32 %sym_ptr435, %SYM_MUL
  br i1 %410, label %then436, label %else440

then436:                                          ; preds = %else434
  %result_ptr437 = load i32, ptr %result_ptr422, align 4
  %lhs438 = load i32, ptr %lhs, align 4
  %rhs439 = load i32, ptr %rhs, align 4
  %411 = mul nsw i32 %lhs438, %rhs439
  %412 = call i32 @make_number(i32 %411)
  store i32 %412, ptr %result_ptr422, align 4
  br label %last483

else440:                                          ; preds = %else434
  %sym_ptr441 = load i32, ptr %sym_ptr, align 4
  %SYM_DIV = load i32, ptr @41, align 4
  %413 = icmp eq i32 %sym_ptr441, %SYM_DIV
  br i1 %413, label %then442, label %else446

then442:                                          ; preds = %else440
  %result_ptr443 = load i32, ptr %result_ptr422, align 4
  %lhs444 = load i32, ptr %lhs, align 4
  %rhs445 = load i32, ptr %rhs, align 4
  %414 = sdiv i32 %lhs444, %rhs445
  %415 = call i32 @make_number(i32 %414)
  store i32 %415, ptr %result_ptr422, align 4
  br label %last482

else446:                                          ; preds = %else440
  %sym_ptr447 = load i32, ptr %sym_ptr, align 4
  %SYM_LT = load i32, ptr @43, align 4
  %416 = icmp eq i32 %sym_ptr447, %SYM_LT
  br i1 %416, label %then448, label %else452

then448:                                          ; preds = %else446
  %result_ptr449 = load i32, ptr %result_ptr422, align 4
  %lhs450 = load i32, ptr %lhs, align 4
  %rhs451 = load i32, ptr %rhs, align 4
  %417 = icmp slt i32 %lhs450, %rhs451
  %418 = sext i1 %417 to i32
  %419 = call i32 @make_bool(i32 %418)
  store i32 %419, ptr %result_ptr422, align 4
  br label %last481

else452:                                          ; preds = %else446
  %sym_ptr453 = load i32, ptr %sym_ptr, align 4
  %SYM_LE = load i32, ptr @45, align 4
  %420 = icmp eq i32 %sym_ptr453, %SYM_LE
  br i1 %420, label %then454, label %else458

then454:                                          ; preds = %else452
  %result_ptr455 = load i32, ptr %result_ptr422, align 4
  %lhs456 = load i32, ptr %lhs, align 4
  %rhs457 = load i32, ptr %rhs, align 4
  %421 = icmp sle i32 %lhs456, %rhs457
  %422 = sext i1 %421 to i32
  %423 = call i32 @make_bool(i32 %422)
  store i32 %423, ptr %result_ptr422, align 4
  br label %last480

else458:                                          ; preds = %else452
  %sym_ptr459 = load i32, ptr %sym_ptr, align 4
  %SYM_GT = load i32, ptr @42, align 4
  %424 = icmp eq i32 %sym_ptr459, %SYM_GT
  br i1 %424, label %then460, label %else464

then460:                                          ; preds = %else458
  %result_ptr461 = load i32, ptr %result_ptr422, align 4
  %lhs462 = load i32, ptr %lhs, align 4
  %rhs463 = load i32, ptr %rhs, align 4
  %425 = icmp sgt i32 %lhs462, %rhs463
  %426 = sext i1 %425 to i32
  %427 = call i32 @make_bool(i32 %426)
  store i32 %427, ptr %result_ptr422, align 4
  br label %last479

else464:                                          ; preds = %else458
  %sym_ptr465 = load i32, ptr %sym_ptr, align 4
  %SYM_GE = load i32, ptr @44, align 4
  %428 = icmp eq i32 %sym_ptr465, %SYM_GE
  br i1 %428, label %then466, label %else470

then466:                                          ; preds = %else464
  %result_ptr467 = load i32, ptr %result_ptr422, align 4
  %lhs468 = load i32, ptr %lhs, align 4
  %rhs469 = load i32, ptr %rhs, align 4
  %429 = icmp sge i32 %lhs468, %rhs469
  %430 = sext i1 %429 to i32
  %431 = call i32 @make_bool(i32 %430)
  store i32 %431, ptr %result_ptr422, align 4
  br label %last478

else470:                                          ; preds = %else464
  %sym_ptr471 = load i32, ptr %sym_ptr, align 4
  %SYM_EQ_NUM = load i32, ptr @46, align 4
  %432 = icmp eq i32 %sym_ptr471, %SYM_EQ_NUM
  br i1 %432, label %then472, label %else476

then472:                                          ; preds = %else470
  %result_ptr473 = load i32, ptr %result_ptr422, align 4
  %lhs474 = load i32, ptr %lhs, align 4
  %rhs475 = load i32, ptr %rhs, align 4
  %433 = icmp eq i32 %lhs474, %rhs475
  %434 = sext i1 %433 to i32
  %435 = call i32 @make_bool(i32 %434)
  store i32 %435, ptr %result_ptr422, align 4
  br label %last477

else476:                                          ; preds = %else470
  %ERR_INVALID_SYMBOL = load i32, ptr @8, align 4
  %436 = call i32 @panic(i32 %ERR_INVALID_SYMBOL)
  br label %last477

last477:                                          ; preds = %else476, %then472
  br label %last478

last478:                                          ; preds = %last477, %then466
  br label %last479

last479:                                          ; preds = %last478, %then460
  br label %last480

last480:                                          ; preds = %last479, %then454
  br label %last481

last481:                                          ; preds = %last480, %then448
  br label %last482

last482:                                          ; preds = %last481, %then442
  br label %last483

last483:                                          ; preds = %last482, %then436
  br label %last484

last484:                                          ; preds = %last483, %then430
  br label %last485

last485:                                          ; preds = %last484, %then424
  %value1_ptr486 = load i32, ptr %value1_ptr414, align 4
  call void @free_data(i32 %value1_ptr486)
  %value2_ptr487 = load i32, ptr %value2_ptr417, align 4
  call void @free_data(i32 %value2_ptr487)
  %result_ptr488 = load i32, ptr %result_ptr422, align 4
  ret i32 %result_ptr488

last489:                                          ; preds = %last16
  %ERR_INVALID_DATA_TYPE = load i32, ptr @0, align 4
  %437 = call i32 @panic(i32 %ERR_INVALID_DATA_TYPE)
  ret i32 %437
}

define void @print(i32 %data_ptr) {
entry:
  %list_ptr = alloca i32, align 4
  %data_ptr1 = alloca i32, align 4
  store i32 %data_ptr, ptr %data_ptr1, align 4
  %data = load [2048 x [2048 x i32]], ptr @15, align 4
  %data_ptr2 = load i32, ptr %data_ptr1, align 4
  %0 = getelementptr inbounds [2048 x i32], ptr @15, i32 %data_ptr2
  %1 = load [2048 x i32], ptr %0, align 4
  %DATA_TYPE = load i32, ptr @9, align 4
  %2 = getelementptr inbounds i32, ptr %0, i32 %DATA_TYPE
  %3 = load i32, ptr %2, align 4
  %DATA_TYPE_SYMBOL = load i32, ptr @17, align 4
  %4 = icmp eq i32 %3, %DATA_TYPE_SYMBOL
  br i1 %4, label %then, label %last

then:                                             ; preds = %entry
  %data3 = load [2048 x [2048 x i32]], ptr @15, align 4
  %data_ptr4 = load i32, ptr %data_ptr1, align 4
  %5 = getelementptr inbounds [2048 x i32], ptr @15, i32 %data_ptr4
  %6 = load [2048 x i32], ptr %5, align 4
  %DATA_VALUE = load i32, ptr @10, align 4
  %7 = getelementptr inbounds i32, ptr %5, i32 %DATA_VALUE
  %8 = load i32, ptr %7, align 4
  call void @print_sym(i32 %8)
  ret void

last:                                             ; preds = %entry
  %data5 = load [2048 x [2048 x i32]], ptr @15, align 4
  %data_ptr6 = load i32, ptr %data_ptr1, align 4
  %9 = getelementptr inbounds [2048 x i32], ptr @15, i32 %data_ptr6
  %10 = load [2048 x i32], ptr %9, align 4
  %DATA_TYPE7 = load i32, ptr @9, align 4
  %11 = getelementptr inbounds i32, ptr %9, i32 %DATA_TYPE7
  %12 = load i32, ptr %11, align 4
  %DATA_TYPE_NUMBER = load i32, ptr @18, align 4
  %13 = icmp eq i32 %12, %DATA_TYPE_NUMBER
  br i1 %13, label %then8, label %last12

then8:                                            ; preds = %last
  %data9 = load [2048 x [2048 x i32]], ptr @15, align 4
  %data_ptr10 = load i32, ptr %data_ptr1, align 4
  %14 = getelementptr inbounds [2048 x i32], ptr @15, i32 %data_ptr10
  %15 = load [2048 x i32], ptr %14, align 4
  %DATA_VALUE11 = load i32, ptr @10, align 4
  %16 = getelementptr inbounds i32, ptr %14, i32 %DATA_VALUE11
  %17 = load i32, ptr %16, align 4
  call void @putint(i32 %17)
  ret void

last12:                                           ; preds = %last
  %data13 = load [2048 x [2048 x i32]], ptr @15, align 4
  %data_ptr14 = load i32, ptr %data_ptr1, align 4
  %18 = getelementptr inbounds [2048 x i32], ptr @15, i32 %data_ptr14
  %19 = load [2048 x i32], ptr %18, align 4
  %DATA_TYPE15 = load i32, ptr @9, align 4
  %20 = getelementptr inbounds i32, ptr %18, i32 %DATA_TYPE15
  %21 = load i32, ptr %20, align 4
  %DATA_TYPE_LIST = load i32, ptr @19, align 4
  %22 = icmp eq i32 %21, %DATA_TYPE_LIST
  br i1 %22, label %then16, label %last28

then16:                                           ; preds = %last12
  call void @putch(i32 40)
  %data17 = load [2048 x [2048 x i32]], ptr @15, align 4
  %data_ptr18 = load i32, ptr %data_ptr1, align 4
  %23 = getelementptr inbounds [2048 x i32], ptr @15, i32 %data_ptr18
  %24 = load [2048 x i32], ptr %23, align 4
  %DATA_VALUE19 = load i32, ptr @10, align 4
  %25 = getelementptr inbounds i32, ptr %23, i32 %DATA_VALUE19
  %26 = load i32, ptr %25, align 4
  store i32 %26, ptr %list_ptr, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %then16
  %list_ptr20 = load i32, ptr %list_ptr, align 4
  %27 = icmp ne i32 %list_ptr20, 0
  br i1 %27, label %for.body, label %for.last

for.body:                                         ; preds = %for.cond
  %list_ptr21 = load i32, ptr %list_ptr, align 4
  call void @print(i32 %list_ptr21)
  %list_ptr22 = load i32, ptr %list_ptr, align 4
  %data23 = load [2048 x [2048 x i32]], ptr @15, align 4
  %list_ptr24 = load i32, ptr %list_ptr, align 4
  %28 = getelementptr inbounds [2048 x i32], ptr @15, i32 %list_ptr24
  %29 = load [2048 x i32], ptr %28, align 4
  %DATA_NEXT = load i32, ptr @11, align 4
  %30 = getelementptr inbounds i32, ptr %28, i32 %DATA_NEXT
  %31 = load i32, ptr %30, align 4
  store i32 %31, ptr %list_ptr, align 4
  %list_ptr25 = load i32, ptr %list_ptr, align 4
  %32 = icmp ne i32 %list_ptr25, 0
  br i1 %32, label %then26, label %last27

then26:                                           ; preds = %for.body
  call void @putch(i32 32)
  br label %last27

last27:                                           ; preds = %then26, %for.body
  br label %for.inc

for.inc:                                          ; preds = %last27
  br label %for.cond

for.last:                                         ; preds = %for.cond
  call void @putch(i32 41)
  ret void

last28:                                           ; preds = %last12
  %data29 = load [2048 x [2048 x i32]], ptr @15, align 4
  %data_ptr30 = load i32, ptr %data_ptr1, align 4
  %33 = getelementptr inbounds [2048 x i32], ptr @15, i32 %data_ptr30
  %34 = load [2048 x i32], ptr %33, align 4
  %DATA_TYPE31 = load i32, ptr @9, align 4
  %35 = getelementptr inbounds i32, ptr %33, i32 %DATA_TYPE31
  %36 = load i32, ptr %35, align 4
  %DATA_TYPE_FUNC = load i32, ptr @21, align 4
  %37 = icmp eq i32 %36, %DATA_TYPE_FUNC
  br i1 %37, label %then32, label %last34

then32:                                           ; preds = %last28
  call void @putch(i32 35)
  call void @putch(i32 60)
  call void @putch(i32 102)
  call void @putch(i32 117)
  call void @putch(i32 110)
  call void @putch(i32 99)
  call void @putch(i32 32)
  %data_ptr33 = load i32, ptr %data_ptr1, align 4
  call void @putint(i32 %data_ptr33)
  call void @putch(i32 62)
  ret void

last34:                                           ; preds = %last28
  %ERR_INVALID_DATA_TYPE = load i32, ptr @0, align 4
  %38 = call i32 @panic(i32 %ERR_INVALID_DATA_TYPE)
  ret void
}

define i32 @main() {
entry:
  %result_ptr = alloca i32, align 4
  %data_ptr = alloca i32, align 4
  %global_env_ptr = alloca i32, align 4
  call void @init_data()
  %0 = call i32 @make_env(i32 0)
  store i32 %0, ptr %global_env_ptr, align 4
  %last_token_type = load i32, ptr @57, align 4
  %1 = call i32 @next_token()
  store i32 %1, ptr @57, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %entry
  br i1 true, label %for.body, label %for.last

for.body:                                         ; preds = %for.cond
  %2 = call i32 @parse()
  store i32 %2, ptr %data_ptr, align 4
  %data_ptr1 = load i32, ptr %data_ptr, align 4
  %3 = icmp eq i32 %data_ptr1, 0
  br i1 %3, label %then, label %last

then:                                             ; preds = %for.body
  br label %for.last

last:                                             ; preds = %for.body
  %data_ptr2 = load i32, ptr %data_ptr, align 4
  %global_env_ptr3 = load i32, ptr %global_env_ptr, align 4
  %4 = call i32 @eval(i32 %data_ptr2, i32 %global_env_ptr3)
  store i32 %4, ptr %result_ptr, align 4
  %result_ptr4 = load i32, ptr %result_ptr, align 4
  %5 = icmp ne i32 %result_ptr4, 0
  br i1 %5, label %then5, label %last7

then5:                                            ; preds = %last
  %result_ptr6 = load i32, ptr %result_ptr, align 4
  call void @print(i32 %result_ptr6)
  call void @putch(i32 10)
  br label %last7

last7:                                            ; preds = %then5, %last
  %data_ptr8 = load i32, ptr %data_ptr, align 4
  call void @free_data(i32 %data_ptr8)
  %result_ptr9 = load i32, ptr %result_ptr, align 4
  call void @free_data(i32 %result_ptr9)
  br label %for.inc

for.inc:                                          ; preds = %last7
  br label %for.cond

for.last:                                         ; preds = %then, %for.cond
  ret i32 0
}
