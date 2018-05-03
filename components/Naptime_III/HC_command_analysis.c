#include "HC_command_analysis.h"

ble_conn_t                         m_conn;
extern uint16_t                    m_conn_handle;   /**< Handle of the current connection. */

static uint8_t ID_receive_buff[4] = {0};  //�洢���յ���ID����
static uint8_t Handshark_buff1[3];        //�������ݴ洢
static uint8_t Handshark_buff2[5];
static uint8_t Handshark_buff3[3];

bool ID_is_change  = false;        //���յ���ID��ԭ��ID��ͬ��������Ҫ���°�ID
bool ID_is_receive = false;        //ID���յ�
bool Is_white_adv  = false;        //�Ƿ�������㲥
bool Is_device_bond = false;       //�豸�Ƿ��
uint8_t communocate_state[5] = {0x04,0x00,0x00,0x00,0xFF}; //����״̬����

extern uint8_t User_ID[4];         //�洢ʵ���û�ID���������ID�������ֳɹ�����¸����鲢��������д��flash  
extern bool Global_connected_state;//����+���ֳɹ���־

//����ID�����н���
void ble_Com_ID_Analysis(uint8_t * p_data, uint16_t length)
{
	uint32_t err_code;
	ID_is_receive = false;
  communocate_state[4] = 0xFF;                            //0xFF����û�н�������
  memset(Handshark_buff1, 0, sizeof(Handshark_buff1));
  memset(Handshark_buff2, 0, sizeof(Handshark_buff2));
  memset(Handshark_buff3, 0, sizeof(Handshark_buff3));
		
	if( *p_data == 0x00  && length == 5)       //�ж�ID��ʽ�Ƿ����
	{
     memcpy(ID_receive_buff,(p_data+1), 4);  //�����յ�ID���ݻ��浽ID_receive_buff
 
		 if(Is_device_bond)                      //����豸�󶨹��������յ�ID�����ж�       
     {	
			 if(Is_white_adv)                      //���ҽ��а������㲥���Ա�ID����������Ҫ�Ͽ����ӣ�����
			 {
					if((memcmp(ID_receive_buff, User_ID, sizeof(User_ID)) == 0))  //if���,=0
					{
						  if(RTT_PRINT)
							{		 		 		 
									SEGGER_RTT_printf(0,"ID_is_receive \r\n");	
							}								
							ID_is_receive = true;          //ID_is_receive��1���ܽ��յ���������
							ID_is_change	= false;		     //ID_is_change��0�򲻻����flash����			 
					}
					else                               //����ȣ��Ͽ�����
					{
							communocate_state[4] = 0x02;   //�豸�ѱ���
							err_code = ble_State_string_send(&m_conn,communocate_state,5);
						  APP_ERROR_CHECK(err_code);
						  nrf_delay_ms(500);
							if(m_conn_handle != BLE_CONN_HANDLE_INVALID)
					    {
								   err_code = sd_ble_gap_disconnect(m_conn_handle,
         			                                      BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
								   APP_ERROR_CHECK(err_code);
					    }
					}
			 }
			 else                                  //���ҽ�����ͨ�㲥������
			 {
					if((memcmp(ID_receive_buff, User_ID, sizeof(User_ID)) == 0))  //if���,=0
					{
						  if(RTT_PRINT)
							{
									SEGGER_RTT_printf(0,"ID_is_receive \r\n");		
							}
							ID_is_receive = true;          //ID_is_receive��1���ܽ��յ���������
							ID_is_change	= false;         //ID_is_change��0�򲻻����flash����		     					 
					}
					else
					{
							ID_is_receive = true;          //ID_is_receive��1���ܽ��յ���������		
							ID_is_change	= true;          //ID_is_change��1������flash����		
					}
			 }
		 }
		 else                                    //���δ�󶨣�ID_is_receive��1���ܽ��յ��������ݣ�ID_is_change�������ֳɹ����ж��Ƿ�洢��flash
		 {
		     ID_is_receive = true;               //ID_is_receive��1���ܽ��յ���������		
				 ID_is_change	= true;			           //ID_is_change��1������flash����
		 }
	}
	else                                       //����ID���ݸ�ʽ���󣬷��ش����벢�Ͽ�
	{
		 communocate_state[4] = 0x01;            //ID���մ���
		 err_code = ble_State_string_send(&m_conn,communocate_state,5);
		 APP_ERROR_CHECK(err_code);
		 nrf_delay_ms(500);
	   if(m_conn_handle != BLE_CONN_HANDLE_INVALID)
		 {
					err_code = sd_ble_gap_disconnect(m_conn_handle,
                                           BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
					APP_ERROR_CHECK(err_code);
		 }
	}
}

//�������ݽ��н���
void ble_Com_Shakehands_Analysis(uint8_t * p_data, uint16_t length)
{
	if( *p_data == 0x01 && length == 5)  //һ������
	{
		 Handshake_agreement_first(p_data + 1);
		 Handshake_agreement_Second();
	}
	if( *p_data == 0x03 && length == 5)  //��������
	{
		 Handshake_agreement_third(p_data + 1);
	}
}
//һ������
void Handshake_agreement_first(uint8_t * p_data)
{
    uint8_t i;
	  for(i = 0; i < 3; i++)
	  {
		   Handshark_buff1[i] = *(p_data + i) ^ *(p_data + 3);  //��ȡʱ��
	  }
}
//��������
void Handshake_agreement_Second(void)
{
	  uint32_t err_code;
		Handshark_buff2[0] = 0x02;
  	Handshark_buff2[4] = get_random_number();
		Handshark_buff2[1] = Handshark_buff1[0] ^ Handshark_buff2[4];
		Handshark_buff2[2] = Handshark_buff1[1] ^ Handshark_buff2[4];
		Handshark_buff2[3] = Handshark_buff1[2] ^ Handshark_buff2[4];
		err_code = ble_Shakehands_string_send(&m_conn,Handshark_buff2,5);
		APP_ERROR_CHECK(err_code);
}
//��������
void Handshake_agreement_third(uint8_t * p_data)
{
    uint8_t i;
		uint32_t err_code;
	  for(i = 0; i < 3; i++)
	  {
		   Handshark_buff3[i] = *(p_data + i) ^ *(p_data + 3);  //��ȡʱ��
	  }
		if( Handshark_buff1[0] == Handshark_buff3[0] &&
			  Handshark_buff1[1] == Handshark_buff3[1] &&
		    Handshark_buff1[2] == Handshark_buff3[2] )
		 {
			  communocate_state[4] = 0x00;                        //���ֳɹ�
			  memcpy(User_ID,ID_receive_buff, 4);
			  Is_white_adv  = true;
			  connects_timer_stop();
			  Global_connected_state = true;
			  if(RTT_PRINT)
				{
						SEGGER_RTT_printf(0,"\r Handshake Success \r\n");
				}
		 }
		 else
		 {
			  communocate_state[4] = 0x05;                       //ʱ�����
		 }
		 err_code = ble_State_string_send(&m_conn,communocate_state,5);
		 APP_ERROR_CHECK(err_code);
		 if(communocate_state[4] != 0x00)
		 {
				nrf_delay_ms(500);		 
				if(m_conn_handle != BLE_CONN_HANDLE_INVALID)
				{
							err_code = sd_ble_gap_disconnect(m_conn_handle,
																							 BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
				    	APP_ERROR_CHECK(err_code);
				}
		 }
}


