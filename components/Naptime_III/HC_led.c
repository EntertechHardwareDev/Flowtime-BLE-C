#include "HC_led.h"

led_indication_t m_stable_state = BSP_INDICATE_IDLE;

extern bool Is_pwm_init;              //��LED�Ž���PWM���ã����͹���
extern bool Is_led_timer_start;       //LED����ʱ���ʱ�жϣ����������;�л�״̬�����¼�ʱ
extern bool Into_factory_test_mode;   //�Ƿ���빤������ģʽ

extern bool Is_red_on;
extern bool Is_green_on;
extern bool Is_blue_on;

bool led_blue_timerout = false;       //��������ʱ�䳬ʱ��־
bool led_red_timerout = false;        //�������ʱ�䳬ʱ��־

void LED_timeout_restart(void)
{
	  if(Is_led_timer_start)
		{
			  led_timer_stop();
		}
		led_timer_start();                //120s��ʱ��ʱ��ʱ��
}

uint32_t bsp_led_indication(led_indication_t indicate)
{
		if(Is_pwm_init == false &&
			(indicate == BSP_INDICATE_CONNECTED ||
		   indicate == BSP_INDICATE_WITH_WHITELIST ||
		   indicate == BSP_INDICATE_WITHOUT_WHITELIST ||
		   indicate == BSP_INDICATE_Battery_LOW ||
		   indicate == BSP_INDICATE_Battery_CHARGING ||
		   indicate == BSP_INDICATE_Battery_CHARGEOVER))       
		{
				led_pwm_init();
		}
    switch (indicate)
    {
			case  BSP_INDICATE_IDLE:                //�ر�LED�ͳ�ʱ��ʱ����PWMȥ��ʼ����LED������͵�ƽ
						if(Is_led_timer_start)
						{
								led_timer_stop();
						}
			      if(Is_pwm_init == true)       
						{
								PWM_uint();
						}
						nrf_gpio_cfg_output(LED_GPIO_BLUE);
						nrf_gpio_cfg_output(LED_GPIO_RED);
						nrf_gpio_cfg_output(LED_GPIO_GREEN);
					
						NRF_GPIO->OUTCLR = 1<<LED_GPIO_BLUE;
						NRF_GPIO->OUTCLR = 1<<LED_GPIO_RED;
						NRF_GPIO->OUTCLR = 1<<LED_GPIO_GREEN;
			      m_stable_state = indicate;  
            break;

      case  BSP_INDICATE_POWER_ON:            //������˸Ƶ��5HZ����˸2�κ�ر�
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
      			LED_ON_duty(0,0,70);
			      m_stable_state = indicate;	      
				    break;

	    case  BSP_INDICATE_WITH_WHITELIST:      //��������״̬������1HZƵ����˸��������ʱ��ʱ��
			      if(Is_blue_on)
						{  
			          LED_ON_duty(0,0,0);
						}
						else
						{
			          LED_ON_duty(0,0,70);  
						}
						ledFlips_timer_start(500);
            m_stable_state = indicate;        //��¼��ǰled״̬�����ڳ�ʱ�Ұ������º�ָ���ǰ״̬
 			      break;

    	case  BSP_INDICATE_WITHOUT_WHITELIST:   //���ƿ�����˸��Ƶ��5HZ������󶨹���򿪳�ʱ��ʱ��
			      if(Is_blue_on)
						{
			          LED_ON_duty(0,0,0);
						}
						else
						{
			          LED_ON_duty(0,0,70);  
						}
						ledFlips_timer_start(100);
            m_stable_state = indicate;			      
				    break;

	    case  BSP_INDICATE_Battery_LOW:         //���������˸��Ƶ��1HZ��������ʱ��ʱ����led�л��ȹر����е�
			      if(Is_red_on)
						{
			          LED_ON_duty(0,0,0);
						}
						else
						{
			          LED_ON_duty(40,0,0);  
						}
						ledFlips_timer_start(500);
            m_stable_state = indicate;        //��¼��ǰled״̬�����ڳ�ʱ�Ұ������º�ָ���ǰ״̬
				    break;

	    case  BSP_INDICATE_Battery_CHARGING:    //���״̬����Ƴ�����led�л��ȹر����е�
			      if(Is_red_on)
						{
			          LED_ON_duty(0,0,0);
						}
						else
						{
			          LED_ON_duty(45,20,0);  
						}
            m_stable_state = indicate;
				    break;

    	case  BSP_INDICATE_Battery_CHARGEOVER:  //������״̬���̵Ƴ�����led�л��ȹر����е�
            LED_ON_duty(0,20,0);
            m_stable_state = indicate;
				    break;

    	case  BSP_INDICATE_factory_led_test:    //LED��������״̬����ʱ��1s����һ�Σ������̵����ε��� 
			      led_test_timer_start();	      
						NRF_GPIO->OUTCLR = 1<<LED_GPIO_BLUE;
						NRF_GPIO->OUTSET = 1<<LED_GPIO_RED;
						nrf_delay_ms(330);
						NRF_GPIO->OUTCLR = 1<<LED_GPIO_RED;
						NRF_GPIO->OUTSET = 1<<LED_GPIO_GREEN;
						nrf_delay_ms(330);
						NRF_GPIO->OUTCLR = 1<<LED_GPIO_GREEN;
						NRF_GPIO->OUTSET = 1<<LED_GPIO_BLUE;	
            nrf_delay_ms(330);
            NRF_GPIO->OUTCLR = 1<<LED_GPIO_BLUE;						
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
		nrf_gpio_cfg_output(LED_GPIO_BLUE);
		nrf_gpio_cfg_output(LED_GPIO_RED);
		nrf_gpio_cfg_output(LED_GPIO_GREEN);
		NRF_GPIO->OUTCLR = 1<<LED_GPIO_BLUE;
		NRF_GPIO->OUTCLR = 1<<LED_GPIO_RED;
		NRF_GPIO->OUTCLR = 1<<LED_GPIO_GREEN;
}
