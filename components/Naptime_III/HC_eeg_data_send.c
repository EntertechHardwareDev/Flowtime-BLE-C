#include "HC_eeg_data_send.h"

ble_EEG_t               m_EEG;                                     /**< Structure used to identify the heart rate service. */

/*******************��������������*********************/
uint32_t Num_Time;       //ÿһ֡���ݰ�ͷ2���ֽڣ������ۼ�
uint8_t send_num = 0;    //һ�뷢��50֡���ݣ���¼���͵��ڼ�֡����
uint8_t Send_Flag = 0;   //��һ֡���ݷ������
uint8_t Data_send[17];   //�������ݻ���
extern uint8_t ADCData2[750];

//���øú������͵�һ֡����
uint32_t ble_send_data(uint8_t *pdata)
{
		uint32_t err_code;
    send_num = 0;
	
		for(uint8_t i = 0; i < 15 ; i++)
		{
			Data_send[i+2] = *(pdata+i);
		}
		Data_send[0] = Num_Time >> 8;    //��Ӱ�ͷ--2���ֽ�
		Data_send[1] = Num_Time & 0xFF;
			 
		err_code = ble_EEG_DATA_send(&m_EEG, Data_send, 17);   //���ݷ��ͣ�����17�ֽ�
		if (NRF_SUCCESS == err_code)
		{
			 send_num++;	
			 Num_Time++;		
			 Send_Flag = 1;  //��֡���ݷ�����ɻᴥ��BLE_EVT_TX_comPLETE,��main������ͨ���ñ�־λ�ж��Ƿ��������ʣ������
		}
		return err_code;
}

//���������ɺ������ݷ���,����BLE_EVT_TX_comPLETEʱ�䴦������
void ble_send_more_data(uint8_t *pdata)
{
	uint32_t err_code;

	do{
			 if (send_num > 49)  //һ������ȫ���������,��ر�־λ��λ
			 { 
					send_num = 0x00;
					Send_Flag = 0;
					memset(&ADCData2,0,sizeof(ADCData2));
					return; 
			 }

			 for(uint8_t i = 0; i < 15 ;i++)
			 {
					Data_send[i+2]=*(pdata + send_num * 15 + i);
			 }
			 Data_send[0] = Num_Time >> 8;
			 Data_send[1] = Num_Time & 0xFF;

			 err_code = ble_EEG_DATA_send(&m_EEG,Data_send, 17 );
			 if (NRF_SUCCESS == err_code)
			 {
					send_num++;	
					Num_Time++;		
	   }
	 }while(err_code != BLE_ERROR_NO_TX_PACKETS);
}

void ble_state_send(uint8_t pdata)
{
   uint32_t err_code;
	 err_code = ble_EEG_ELE_STATE_send(&m_EEG,pdata, 1);
	 APP_ERROR_CHECK(err_code);
}
