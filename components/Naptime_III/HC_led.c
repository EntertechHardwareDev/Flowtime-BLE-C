#include "HC_led.h"

led_indication_t m_stable_state = BSP_INDICATE_IDLE;

extern bool Is_pwm_init;              //��LED�Ž���PWM���ã����͹���
extern bool Is_led_timer_start;       //LED����ʱ���ʱ�жϣ����������;�л�״̬�����¼�ʱ
extern bool Into_factory_test_mode;   //�Ƿ���빤������ģʽ

extern bool Is_red_on;
extern bool Is_green_on;
extern bool Is_blue_on;

bool led_timerout = false;            //led����ʱ�䳬ʱ��־    

void LED_timeout_restart(void)        //���������;�л�״̬�����¼�ʱ
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
		   indicate == BSP_INDICATE_CONNECTED_BAT_LOW ||
		   indicate == BSP_INDICATE_WITH_WHITELIST ||
		   indicate == BSP_INDICATE_WITH_WHITELIST_BAT_LOW ||
		   indicate == BSP_INDICATE_WITHOUT_WHITELIST ||
		   indicate == BSP_INDICATE_WITHOUT_WHITELIST_BAT_LOW ||
		   indicate == BSP_INDICATE_Battery_CHARGING ||
		   indicate == BSP_INDICATE_Battery_CHARGEOVER))       
		{
				led_pwm_init();
		}
		if(Is_pwm_init == true &&
			(indicate == BSP_INDICATE_IDLE ||
		   indicate == BSP_INDICATE_FACTORY_LED_TEST))       
		{
				PWM_uint();
		}
    switch (indicate)
    {
			case  BSP_INDICATE_IDLE:                //�ر�LED�ͳ�ʱ��ʱ����PWMȥ��ʼ����LED������͵�ƽ
						if(Is_led_timer_start)
						{
								led_timer_stop();
						}
						nrf_gpio_cfg_output(LED_GPIO_BLUE);
						nrf_gpio_cfg_output(LED_GPIO_RED);
						nrf_gpio_cfg_output(LED_GPIO_GREEN);
						NRF_GPIO->OUTCLR = 1<<LED_GPIO_BLUE;
						NRF_GPIO->OUTCLR = 1<<LED_GPIO_RED;
						NRF_GPIO->OUTCLR = 1<<LED_GPIO_GREEN;	
						m_stable_state = BSP_INDICATE_IDLE;  
            break;

    	case  BSP_INDICATE_CONNECTED:           //����״̬�����Ƴ���
      			LED_ON_duty(0,0,70);
			      m_stable_state = indicate;	      //��¼��ǰled״̬ 
				    break;

			case  BSP_INDICATE_CONNECTED_BAT_LOW:   //��Ƴ���
			      LED_ON_duty(40,0,0);  
            m_stable_state = indicate;        //��¼��ǰled״̬
				    break;
			
	    case  BSP_INDICATE_WITH_WHITELIST:      //����״̬������1HZƵ����˸
			      if(Is_blue_on)
						{  
			          LED_ON_duty(0,0,0);
						}
						else
						{
			          LED_ON_duty(0,0,70);  
						}
						ledFlips_timer_start(500);
            m_stable_state = indicate;        //��¼��ǰled״̬
 			      break;

			case  BSP_INDICATE_WITH_WHITELIST_BAT_LOW:     //����״̬�����1HZƵ����˸
			      if(Is_red_on)
						{
			          LED_ON_duty(0,0,0);
						}
						else
						{
			          LED_ON_duty(40,0,0);  
						}
						ledFlips_timer_start(500);
            m_stable_state = indicate;        //��¼��ǰled״̬
				    break;
						
    	case  BSP_INDICATE_WITHOUT_WHITELIST:   //���ƿ�����˸��Ƶ��5HZ
			      if(Is_blue_on)
						{
			          LED_ON_duty(0,0,0);
						}
						else
						{
			          LED_ON_duty(0,0,70);  
						}
						ledFlips_timer_start(100);
            m_stable_state = indicate;		    //��¼��ǰled״̬	      
				    break;
						
			case  BSP_INDICATE_WITHOUT_WHITELIST_BAT_LOW:   //��ƿ�����˸��Ƶ��5HZ
			      if(Is_red_on)
						{
			          LED_ON_duty(0,0,0);
						}
						else
						{
			          LED_ON_duty(40,0,0);  
						}
						ledFlips_timer_start(100);
            m_stable_state = indicate;		    //��¼��ǰled״̬	      
				    break;

	    case  BSP_INDICATE_Battery_CHARGING:    //���״̬���Ƶ�����
			      if(Is_red_on)
						{
			          LED_ON_duty(0,0,0);
						}
						else
						{
			          LED_ON_duty(45,20,0);  
						}
            m_stable_state = BSP_INDICATE_IDLE;  
				    break;

    	case  BSP_INDICATE_Battery_CHARGEOVER:  //������״̬���̵Ƴ���
            LED_ON_duty(0,20,0);
            m_stable_state = BSP_INDICATE_IDLE;
				    break;

    	case  BSP_INDICATE_FACTORY_LED_TEST:    //LED��������״̬����ʱ��1s����һ�Σ������̵����ε���,Ȼ����0.25s
			      led_test_timer_start();	      
						NRF_GPIO->OUTCLR = 1<<LED_GPIO_BLUE;
						NRF_GPIO->OUTSET = 1<<LED_GPIO_RED;
						nrf_delay_ms(250);
						NRF_GPIO->OUTCLR = 1<<LED_GPIO_RED;
						NRF_GPIO->OUTSET = 1<<LED_GPIO_GREEN;
						nrf_delay_ms(250);
						NRF_GPIO->OUTCLR = 1<<LED_GPIO_GREEN;
						NRF_GPIO->OUTSET = 1<<LED_GPIO_BLUE;	
            nrf_delay_ms(250);
            NRF_GPIO->OUTCLR = 1<<LED_GPIO_BLUE;						
            m_stable_state = BSP_INDICATE_IDLE;
				    break;

			default:
            break;
    }
    return NRF_SUCCESS;
}

void leds_state_update(void)                             //LED��ʱ��ʱ���ص�����
{
    uint32_t err_code;	
    ledFlips_timer_stop();
	
	  if(m_stable_state == BSP_INDICATE_CONNECTED ||       //��ʱ���־λ��1���ر�led�ƣ�����������£�LED�ָ�״̬��
			 m_stable_state == BSP_INDICATE_CONNECTED_BAT_LOW ||
			 m_stable_state == BSP_INDICATE_WITH_WHITELIST || 
		   m_stable_state == BSP_INDICATE_WITH_WHITELIST_BAT_LOW ||
			 m_stable_state == BSP_INDICATE_WITHOUT_WHITELIST ||
		   m_stable_state == BSP_INDICATE_WITHOUT_WHITELIST_BAT_LOW
		)
		{
				led_timerout = true;	
		}
    err_code = bsp_led_indication(BSP_INDICATE_IDLE);
    APP_ERROR_CHECK(err_code);
}
