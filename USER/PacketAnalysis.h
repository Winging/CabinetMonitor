#ifndef __PACKET_ANALYSIS__
#define __PACKET_ANALYSIS__

#include "includes.h"

#define PACKET_SOI 0x7E //����ʼ
#define PACKET_EOI 0X0D //������
#define PACKET_DATA_STPOS 15 //���ݴӵ�15���ֽڿ�ʼ

#define COM_START   1
#define COM_DATA    8
#define COM_STOP    1
#define COM_SPEED   9600

/*CID2 �з����� RTN ����*/
#define RTN_OK              0x00    //����
#define RTN_VER_ERROR       0x01    //VER ��
#define RTN_CHKSUM_ERROR    0x02    //CHKSUM ��
#define RTN_LCHKSUM         0x03    //��ʱû��
#define RTN_CID2_INVALID    0x04    //CID2��Ч
#define RTN_COMMAND_ERROR   0X05    //�����ʽ����
#define RTN_INVALID_DATE    0x06    //��Ч����
#define RTN_OTHER_ERROR     0x07    //��������

/*�豸���ͱ�������(CID1)*/
#define CID1_AC                 0x40    //���ص�Դϵͳ(�������)
#define CID1_ALL                0x41    //���ص�Դϵͳ(����ģ��)
#define CID1_DC                 0x42    //���ص�Դϵͳ(ֱ�����)
#define CID1_ENV_SYS            0xE1    //�������ϵͳ
#define CID1_SET_SYS_STATUS     0x01    //��ȡ�������ϵͳ״̬
#define CID1_SET_SYS_TH         0x02    //���û�����ʪ�����桢ƫ����
#define CID1_GET_SYS_TH         0x03    //��ȡ������ʪ�����桢ƫ����
#define CID1_SET_AC_DC          0x04    //����ֱ��������ѹ�����ƫ����
#define CID1_GET_AC_DC          0x05    //��ȡֱ��������ѹ�����ƫ����
#define CID1_SET_DC_ALARM       0x06    //����ֱ����ѹ����Ƿ��ȱѹ�澯��
#define CID1_GET_DC_ALARM       0x07    //��ȡֱ����ѹ����Ƿ��ȱѹ�澯��
#define CID1_SET_AC_ALARM       0x08    //���ý�����ѹ����Ƿ��ȱѹ�澯��
#define CID1_GET_AC_ALARM       0x09    //��ȡ������ѹ����Ƿ��ȱѹ�澯��
#define CID1_SET_TH_HIGH_LOWER  0x0A    //������ʪ�ȹ��߹��͸澯��
#define CID1_GET_TH_HIGH_LOWER  0x0B    //��ȡ��ʪ�ȹ��߹��͸澯��
#define CID1_SET_CLOCK          0x12  

  //����ʱ��(�ꡢ�¡��ա�ʱ����)
#define CID1_GET_CLOCK          0x13    //��ȡʱ��(�ꡢ�¡��ա�ʱ����)
#define CID1_SET_FAN            0x1F    //���÷��ȿ��Ʋ���
#define CID1_GET_FAN           0x20    //��ȡ���ȿ��Ʋ���
#define CID1_SET_HEAT           0x21    //���ü��������Ʋ���
#define CID1_GET_HEAT           0x22    //��ȡ���������Ʋ���


/***************************
 *�� SOI �� EOI 

���� 16 ���ƴ�����,���������� 16 ����-ASCII �뷽ʽ����,ÿ
���ֽ������� ASCII ���ʾ,�統 

CID2=4BH ʱ,����ʱ���� 34H(��4���� ASCII ��)
�� 42H(��B���� ASCII ��)�����ֽ�
 

***************************/
typedef struct PackageRec{
    uint8_t            cSoi;//��ʼλ,0x7E
    uint8_t            cVer;//ͨ��Э��汾��
    uint8_t            AddrHigh;//��ַ ��λ
	uint8_t            AddrLow;//��ַ ��λ
    uint8_t            cCid1;//���Ʊ�ʶ��
    uint8_t            cCid2;//������Ϣ:���Ʊ�ʶ��(���ݻ�����������) ��Ӧ��Ϣ:������ RTN
    uint16_t		InfoLen;//INFO 16�ֽ�
    uint8_t	        Info[70];//INFO;INFO ����ʱ���ֽ���,��� INFO Ϊ��,�� LENTH=0x00H;(��Ҫ��ʱ�����붯̬����)
    uint8_t            cChksum;//У�����;CHKSUM �ļ����ǳ� SOI��EOI �� CHKSUM ��,�����ַ��Դ����ʽ�ۼ����,���ý��ģ 255 ����
    uint8_t            cEoi;//������,0x0D
} Package;



/*��������***************************************************************************/
uint8_t GetCheckSum(uint8_t packet[],uint32_t infoLen);
#endif // STDAFX_H
