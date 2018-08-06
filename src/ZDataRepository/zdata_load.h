/*
* Copyright (C) Yingzhi Zheng.
* Copyright (C) <zhengyingzhi112@163.com>
*/

#ifndef _ZDATA_LOAD_H_INCLUDED_
#define _ZDATA_LOAD_H_INCLUDED_

#include "zdata_struct.h"


#define ZDATA_Ok        0       // ok
#define ZDATA_Error    -1       // ����
#define ZDATA_Ignore    1       // ����

/* �Զ������ݶ�ȡ����
 * result   ������ָ������ڴ�Ĭ���ɵײ����(������zdatabar/zdatatick��)
 * context  �û�����������
 * number   �ص�����
 * data     ����������
 * length   ���������ݵĳ���
 * return   ZDATA_Ok-�ɹ� 
 */
typedef int(*zdata_proc_func_ptr)(void* result, void* context, int number, const char* data, int length);

/* ��������ʱ��ÿ�����ݵĶ�����ڴ���亯��
 * context  �û�����������
 * size     �����С
 * return   ָ���ַ
 */
typedef void*(*zdata_alloc_ptr)(void* context, int size);


/* �������ݼ��ط�ʽ
 * 1. ��ܿ�ֻ�������ݿ��ٶ�ȡ���ٽ����ݰ��л򰴶����ƿ��С�ص�֪ͨ���ϲ�
 * 2. Ҳ����ָ�������ֶι�ϵ�����ڵײ��Զ�����
 * ���ݼ��ؽ����󣬸����ݶ���Ĭ���ɵײ��Զ�����
 */
typedef struct  
{
    zdata_proc_func_ptr proc;       // ����ʹ��ϵͳĬ�ϵ�
    void*               context;   // �û�����������
    const char*         filepath;   // �ļ�·��(��֧�ֶ���ļ�)
    zdatabar_index_t    indexs;     // ��ûָ�� proc ʱ����Ҫ����ø��ֶε�����λ��
    int                 headline;   // �Ƿ��б�ͷ��
    int                 binary_usz; // ����binary����ʱ�ĸ������ݵĴ�С
    void*               alloc_ptr;  // �ڴ���亯��
}zdata_load_conf_t;

/* ����csv�ļ��е����ݵ�������
 * ����csv ���ֶζ�Ӧ������λ��
 */
int zdata_load_from_csv_file(zdatavec_t** ppdvec, const char* filepath, zdatabar_index_t* indexs);

/* ����binary�ļ��е����ݵ�������
 */
int zdata_load_from_binary_file(zdatavec_t** ppdvec, const char* filepath);

/* ����binary�������ݵ��ļ���
 */
int zdata_save_to_binary_file(const char* datapath, const zdatavec_t* dvec);


/* ���� zdatavec_t ����
 */
void zdata_dvec_reset(zdatavec_t* dvec);

/* �ͷ� zdatavec_t ������ڴ�
 */
void zdata_dvec_release(zdatavec_t* dvec);

#endif//_ZDATA_LOAD_H_INCLUDED_
