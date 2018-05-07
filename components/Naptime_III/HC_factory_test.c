#include "HC_factory_test.h"

static bool Into_System_test_mode = false;              //�Ƿ����ϵͳ����ģʽ

bool APP_restart = false;                               //APP��λ��־
bool Into_factory_test_mode = false;                    //�Ƿ���빤������ģʽ
bool StoryDeviceID =false;                              //�Ƿ�洢deviceID
bool StorySN =false;                                    //�Ƿ�洢SN
bool deleteUserid =false;                               //�Ƿ�ɾ��UserID

extern bool ads1291_is_init;                            //ADS1291�Ƿ��ʼ��
extern uint8_t device_id_receive[16];                   //���������˷��͹�����deviceID-16λ
extern uint8_t device_sn_receive[16];                   //���������˷��͹�����SN-16λ
extern bool Global_connected_state;                     //����+���ֳɹ���־

extern void sleep_mode_enter(void);
//����Check���ж��Ƿ�Ϊ��λ�������Ƿ���빤������ģʽ
void bootup_check(void)
{	  
	  if(RTT_PRINT)
		{
				SEGGER_RTT_printf(0," NRF_POWER->GPREGRET:%x\r\n\n",NRF_POWER->GPREGRET);
		}
		APP_restart = (NRF_POWER->GPREGRET == 0x55);    //��������λ����λǰ�Ὣ�Ĵ���NRF_POWER->GPREGRET����Ϊ0x55
		if(APP_restart)                                
		{
			 NRF_POWER->GPREGRET = 0;
		}
		
		if(nrf_gpio_pin_read(FACTORY_TEST) == 0)        //�ж��Ƿ���빤������ģʽ
		{
				if(RTT_PRINT)
				{
						SEGGER_RTT_printf(0,"\r Into_factory_test_mode \r\n");
				}
				Into_factory_test_mode = true;
				Uart_init();
				app_uart_put(Nap_Tool_Gotofactorytest);	    //Nap֪ͨTool--����ɹ����빤������
		}
		else
		{
			  Into_factory_test_mode = false;
		}
}
//LED���ԣ�ѭ����˸
void led_test(void)
{
	    uint32_t err_code;
	    pwm_uint();                                   //����led_test֮ǰ���������������ֻ�������PWM�Ѿ���ʼ��
	
	    nrf_gpio_cfg_output(LED_GPIO_BLUE);
      nrf_gpio_cfg_output(LED_GPIO_RED);
	    nrf_gpio_cfg_output(LED_GPIO_GREEN);
	  
	    NRF_GPIO->OUTCLR = 1<<LED_GPIO_BLUE;
	    NRF_GPIO->OUTCLR = 1<<LED_GPIO_RED;
	    NRF_GPIO->OUTCLR = 1<<LED_GPIO_GREEN;
      
	 		err_code = bsp_led_indication(BSP_INDICATE_FACTORY_LED_TEST);   //LED״̬����
      APP_ERROR_CHECK(err_code);
}
//�ֻ������ݽ�����com������յ�������
void App_Nap_data_Analysis(uint8_t *pdata)
{
    uint32_t err_code;
	  if(RTT_PRINT)
		{
				SEGGER_RTT_printf(0,"commmand: %x \r\n",*pdata);
		}
		switch(*pdata)
		{
			 case App_Nap_SystemTest:             
				    Into_System_test_mode = true;							 
						break;
			 
			 case App_Nap_Start1291:             
				    if(ads1291_is_init == false) 
						{
								ads1291_init();
						}							 
						break;
									
			 case App_Nap_Stop1291: 
				    if(ads1291_is_init == true) 
						{
								ADS1291_disable();
						}							 
						break;	
																
			 case App_Nap_write_deviceid : 
				    if(Into_System_test_mode)
						{
								memcpy(device_id_receive,pdata+1, 16);  //ȥ����һ���ֽ�
								StoryDeviceID = true;                   //flash������main��������
						}
						break;
			 
			 case App_Nap_write_SN: 			 
				    if(Into_System_test_mode)
						{
								memcpy(device_sn_receive,pdata+1, 16);  //ȥ����һ���ֽ�
								StorySN = true;                         //flash������main��������
						}
						break;
			 
			 case App_Nap_useriddelete: 
					  deleteUserid = true;                        //flash������main��������                   
						break;
			 
			 case App_Nap_Gotoledtest: 
           if(Into_factory_test_mode)                   //���빤������ģʽ
	         {						 
							led_test(); 
					 }						 
					 break;
					 
				case App_Nap_Poweroff:                          //�ػ�ָ��  
					  if(ads1291_is_init == true)
						{			
					   	 ADS1291_disable();
						}
						Global_connected_state = false;
					  err_code = bsp_led_indication(BSP_INDICATE_IDLE);
					  APP_ERROR_CHECK(err_code);
					  sleep_mode_enter();
					  break;

			 default:
						break;
		}
}

