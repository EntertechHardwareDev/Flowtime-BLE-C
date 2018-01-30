#include "HC_led.h"

led_indication_t m_stable_state = BSP_INDICATE_IDLE;

extern bool Is_pwm_init;             //��LED�Ž���PWM���ã����͹���
extern bool Is_device_bond;          //�豸�Ƿ��   
extern bool Is_led_timer_start;      //LED����ʱ���ʱ�жϣ����������;�л�״̬�����¼�ʱ
extern bool Into_factory_test_mode;  //�Ƿ���빤������ģʽ
extern bool Is_red_on;
extern bool Is_green_on;
extern bool Is_blue_on;

uint8_t LED_Red_pro = 50;            //LEDռ�ձȵ���
uint8_t LED_Green_pro = 25;
uint8_t LED_Blue_pro = 40;

bool led_blue_timerout = false;       //��������ʱ�䳬ʱ��־
bool led_red_timerout = false;        //�������ʱ�䳬ʱ��־

void LED_timeout_start(void)
{
	  PWM_uint();
	  if(Is_led_timer_start)
		{
			  led_timer_stop();
		}
		led_timer_start();
}

uint32_t bsp_led_indication(led_indication_t indicate)
{
		if(Is_pwm_init == false)       
		{
				led_pwm_init();
		}
		
    switch (indicate)
    {
			case  BSP_INDICATE_IDLE:                //�ر�LED�ͳ�ʱ��ʱ����PWMȥ��ʼ����LED������͵�ƽ
						led_timer_stop();
			      if(Is_pwm_init == true)       
						{
								PWM_uint();
						}
			      m_stable_state = indicate;  
            break;

      case  BSP_INDICATE_POWER_ON:            //������˸Ƶ��5HZ����˸2�κ�ر�
						if(Is_pwm_init == true)       
						{
								PWM_uint();
						} 
	          nrf_gpio_cfg_output(LED_GPIO_BLUE);
	          NRF_GPIO->OUTSET = 1<<LED_GPIO_BLUE;
						nrf_delay_ms(100);	  
	          NRF_GPIO->OUTCLR = 1<<LED_GPIO_BLUE;
						nrf_delay_ms(100);	  
	          NRF_GPIO->OUTSET = 1<<LED_GPIO_BLUE;
						nrf_delay_ms(100);	  
	          NRF_GPIO->OUTCLR = 1<<LED_GPIO_BLUE;
						nrf_delay_ms(100);	  
            m_stable_state = indicate;
				    break;

      case  BSP_INDICATE_POWER_OFF:           //������˸Ƶ��5HZ����˸2�κ�ر�	
//            SEGGER_RTT_printf(0," BSP_INDICATE_POWER_OFF \n");
						if(Is_pwm_init == true)       
						{
								PWM_uint();
						}  
	          nrf_gpio_cfg_output(LED_GPIO_BLUE);
	          NRF_GPIO->OUTCLR = 1<<LED_GPIO_BLUE;
						nrf_delay_ms(500);	  
	          NRF_GPIO->OUTSET = 1<<LED_GPIO_BLUE;
						nrf_delay_ms(100);	  
	          NRF_GPIO->OUTCLR = 1<<LED_GPIO_BLUE;
						nrf_delay_ms(100);	  
	          NRF_GPIO->OUTSET = 1<<LED_GPIO_BLUE;
						nrf_delay_ms(100);	  
	          NRF_GPIO->OUTCLR = 1<<LED_GPIO_BLUE;
            m_stable_state = BSP_INDICATE_IDLE;
				    break;

    	case  BSP_INDICATE_CONNECTED:           //��������״̬��������ʱ��ʱ��
//            SEGGER_RTT_printf(0," BSP_INDICATE_CONNECTED \n");
      			LED_ON_duty(0,0,40);
			      m_stable_state = indicate;	      
				    break;

	    case  BSP_INDICATE_WITH_WHITELIST:      //��������״̬������1HZƵ����˸��������ʱ��ʱ��
//            SEGGER_RTT_printf(0," BSP_INDICATE_WITH_WHITELIST \n");
			      if(Is_blue_on)
						{  
			          LED_ON_duty(0,0,0);
						}
						else
						{
			          LED_ON_duty(0,0,40);  
						}
						ledFlips_timer_start(500);
            m_stable_state = indicate;        //��¼��ǰled״̬�����ڳ�ʱ�Ұ������º�ָ���ǰ״̬
 			      break;

    	case  BSP_INDICATE_WITHOUT_WHITELIST:   //���ƿ�����˸��Ƶ��5HZ������󶨹���򿪳�ʱ��ʱ��
//            SEGGER_RTT_printf(0," BSP_INDICATE_WITHOUT_WHITELIST \n");
			      if(Is_blue_on)
						{
			          LED_ON_duty(0,0,0);
						}
						else
						{
			          LED_ON_duty(0,0,40);  
						}
						ledFlips_timer_start(100);
            m_stable_state = indicate;			      
				    break;

	    case  BSP_INDICATE_Battery_LOW:         //���������˸��Ƶ��1HZ��������ʱ��ʱ����led�л��ȹر����е�
//            SEGGER_RTT_printf(0," BSP_INDICATE_Battery_LOW \n");
			      if(Is_red_on)
						{
			          LED_ON_duty(0,0,0);
						}
						else
						{
			          LED_ON_duty(40,0,0);  
						}
						ledFlips_timer_start(100);
            m_stable_state = indicate;        //��¼��ǰled״̬�����ڳ�ʱ�Ұ������º�ָ���ǰ״̬
				    break;

	    case  BSP_INDICATE_Battery_CHARGING:    //���״̬����Ƴ�����led�л��ȹر����е�
			      if(Is_red_on)
						{
			          LED_ON_duty(0,0,0);
						}
						else
						{
			          LED_ON_duty(60,35,0);  
						}
            m_stable_state = indicate;
				    break;

    	case  BSP_INDICATE_Battery_CHARGEOVER:  //������״̬���̵Ƴ�����led�л��ȹر����е�
            LED_ON_duty(0,60,0);
            m_stable_state = indicate;
				    break;

    	case  BSP_INDICATE_factory_led_test:    //LED��������״̬����ʱ��1s����һ�Σ������̵����ε���
						if(Is_pwm_init == true)       
						{
								PWM_uint();
						}  
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
    return NRF_SUCCESS;
}

void leds_state_update(void)                             //LED��ʱ��ʱ���ص�����
{
    ledFlips_timer_stop();
	  SEGGER_RTT_printf(0," leds_state_update \n");
	  if(m_stable_state == BSP_INDICATE_CONNECTED ||       //��3��״̬�£���ʱ���־λ��1���ر����ƣ�����������£�LED�ָ�״̬��
			 m_stable_state == BSP_INDICATE_WITH_WHITELIST || 
		   m_stable_state == BSP_INDICATE_WITHOUT_WHITELIST)
		{
				led_blue_timerout = true;	
		}
	  if(m_stable_state == BSP_INDICATE_Battery_LOW)       //�͵���״̬���Ӻ󣬳�ʱ��ʱ���򿪣����ڳ�ʱ���Ƴ�ʱ��־��1
		{
				led_red_timerout = true;	
		}
		PWM_uint();                                          //LED�Ѿ�Ϩ�𣬽�PWMȥ��ʼ��
}
