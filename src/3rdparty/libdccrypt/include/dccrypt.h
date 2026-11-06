/* dccrypt.h
 *
 * delay code encrypt and decrypt library
 *
 * WU Ming start this file on 2010/05/26
 */


#ifndef __DCCRYPT_H__
#define __DCCRYPT_H__

#ifdef __cplusplus
extern "C"{
#endif

#define DCCRYPT_MAX_STR    64      // 最大字符串长度（包括字符串结束符）

#define DCCRYPT_MAX_DC     32      // 最大期限码长度
#define DCCRYPT_MIN_DC     6       // 最小期限码长度

#define DCCRYPT_LEN_PS     13      // 机器编号长度
#define DCCRYPT_LEN_LIMIT  8       // 使用期限长度


/* 加密
 * in:  limit      - 使用期限（yyyymmdd）
 * in:  product_sn - 机器编号
 * out: delay_code - 期限码
 * return:
 *   0 - encrypt successfully
 *  -1 - 使用期限长度不正确
 *  -2 - 使用期限含有非法字符
 *  -3 - 机器编号长度不正确
 *  -4 - 机器编号含有非法字符
 *  -9 - 解密库中无法运行加密函数
 * */
int dc_encrypt(char *limit, char *product_sn, char *delay_code);

/* 解密
 * in:  delay_code - 期限码
 * out: limit      - 使用期限（yyyymmdd）
 * out: product_sn - 机器编号
 * return:
 *   0 - decrypt successfully
 *  -1 - 期限码长度不正确
 *  -2 - 期限码含有非法字符
 *  -9 - 非法的期限码
 * */
int dc_decrypt(char *delay_code, char *limit, char *product_sn);
#ifdef __cplusplus
}
#endif
#endif /* __DCCRYPT_H__ */
