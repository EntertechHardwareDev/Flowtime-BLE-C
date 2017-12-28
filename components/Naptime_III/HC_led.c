#include "HC_led.h"

led_indication_t m_stable_state = BSP_INDICATE_IDLE;

extern bool Is_pwm_init;             //��LED�Ž���PWM���ã����͹���
extern bool Is_device_bond;          
extern bool Is_led_timer_start;      //LED����ʱ���ʱ�жϣ����������;�л�״̬�����¼�ʱ
extern bool Into_factory_test_mode;

uint8_t LED_Red_pro = 50;            //LEDռ�ձȵ���
uint8_t LED_Green_pro = 25;
uint8_t LED_Blue_pro = 40;

uint8_t led_blue_timerout = 0;       //��������ʱ�䳬ʱ��־
uint8_t led_red_timerout = 0;        //�������ʱ�䳬ʱ��־

uint32_t bsp_led_indication(led_indication_t indicate)
{
    uint32_t err_code = NRF_SUCCESS;
		if(Is_pwm_init == false && !Into_factory_test_mode)         //���û�г�ʼ��PWM����ʼ��PWM
		{
				led_pwm_init();
		}

    switch (indicate)
    {
			case  BSP_INDICATE_IDLE:       //�ر�LED�ͳ�ʱ��ʱ����PWMȥ��ʼ����LED������͵�ƽ
				    if(Is_led_timer_start)
						{
							  led_timer_stop();
						}
						PWM_uint();
            m_stable_state = indicate;  
            break;

      case  BSP_INDICATE_POWER_ON:    //������˸Ƶ��5HZ����˸2�κ�ر�	
            LED_BLUE(5,LED_Blue_pro);
						nrf_delay_ms(500);
			      LED_BLUE(1,0);
            m_stable_state = indicate;
				    break;

      case  BSP_INDICATE_POWER_OFF:   //������˸Ƶ��5HZ����˸2�κ�ر�	
				    if(Is_led_timer_start)
						{
							  led_timer_stop();
						}
						LED_BLUE(1,0);
			      nrf_delay_ms(500);
            LED_BLUE(5,LED_Blue_pro);
            nrf_delay_ms(500);
			      LED_BLUE(1,0);
            m_stable_state = indicate;
				    break;

    	case  BSP_INDICATE_CONNECTED:   //���Ƴ�����������빤������ģʽ�򲻿�����ʱ��ʱ������һֱ����
      			if(Is_led_timer_start)
						{
							  led_timer_stop();
						}
      			LED_BLUE(250,LED_Blue_pro);  
						if(!Into_factory_test_mode)
						{
						    led_timer_start();
						}
			      m_stable_state = indicate;	  //��¼��ǰled״̬�����ڳ�ʱ�Ұ������º�ָ���ǰ״̬
				    break;

	    case  BLE_INDICATE_WITH_WHITELIST:  //��������״̬������1HZƵ����˸��������ʱ��ʱ��
				    if(Is_led_timer_start)
						{
							  led_timer_stop();
						}
						led_timer_start();
			      LED_BLUE(1,LED_Blue_pro);
            m_stable_state = indicate;
 			      break;

    	case  BLE_INDICATE_WITHOUT_WHITELIST: //���ƿ�����˸��Ƶ��5HZ������󶨹���δ���빤������ģʽ��򿪳�ʱ��ʱ��
				    nrf_delay_ms(100);
						if(Is_device_bond && (!Into_factory_test_mode))
						{
						    led_timer_start();
						}
      			LED_BLUE(5,LED_Blue_pro);
            m_stable_state = indicate;			      
				    break;

	    case  BSP_INDICATE_Battery_LOW:     //���������˸��Ƶ��1HZ��������ʱ��ʱ����ע���״̬֮ǰ����Ϊ������������Ҫ�ȹر�����
				    LED_BLUE(1,0);
			      if(Is_led_timer_start)
						{
							  led_timer_stop();
						}
						led_timer_start();
      			LED_RED(1,LED_Red_pro);
            m_stable_state = indicate;
				    break;

	    case  BSP_INDICATE_Battery_CHARGING:    //���״̬����Ƴ���
            LED_RED(250,LED_Red_pro);
            m_stable_state = indicate;
				    break;

    	case  BSP_INDICATE_Battery_CHARGEOVER:  //������״̬���̵Ƴ���
						LED_GREEN(LED_Green_pro);
            m_stable_state = indicate;
				    break;

    	case  BSP_INDICATE_factory_led_test:    //LED��������״̬����ʱ��1s����һ�Σ������̵����ε���
			      led_test_timer_start();	      
						NRF_GPIO->OUTCLR = 1<<LED_GPIO_BLUE;
						NRF_GPIO->OUTSET = 1<<LED_GPIO_RED;
						nrf_delay_ms(300);
						NRF_GPIO->OUTCLR = 1<<LED_GPIO_RED;
						NRF_GPIO->OUTSET = 1<<LED_GPIO_GREEN;
						nrf_delay_ms(300);
						NRF_GPIO->OUTCLR = 1<<LED_GPIO_GREEN;
						NRF_GPIO->OUTSET = 1<<LED_GPIO_BLUE;		
            m_stable_state = indicate;
				    break;

			default:
            break;
    }
    return err_code;
}

void leds_state_update(void)                             //LED��ʱ��ʱ���ص�����
{
	  if(m_stable_state == BSP_INDICATE_CONNECTED ||       //��3��״̬�£���ʱ���־λ��1���ر����ƣ�����������£�LED�ָ�״̬��
			 m_stable_state == BLE_INDICATE_WITH_WHITELIST || 
		   m_stable_state == BLE_INDICATE_WITHOUT_WHITELIST)
		{
				LED_BLUE(1,0);
				led_blue_timerout = 1;	
		}
	  if(m_stable_state == BSP_INDICATE_Battery_LOW)       //�͵���״̬���Ӻ󣬳�ʱ��ʱ���򿪣����ڳ�ʱ���Ƴ�ʱ��־��1
		{
				LED_RED(1,0);
				led_red_timerout = 1;	
		}
		PWM_uint();                                          //LED�Ѿ�Ϩ�𣬽�PWMȥ��ʼ��
}
