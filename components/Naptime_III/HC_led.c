#include "HC_led.h"

led_indication_t m_stable_state = BSP_INDICATE_IDLE;

extern bool Is_pwm_init;             //��LED�Ž���PWM���ã����͹���
extern bool Is_device_bond;          //�豸�Ƿ��   
extern bool Is_led_timer_start;      //LED����ʱ���ʱ�жϣ����������;�л�״̬�����¼�ʱ
extern bool Into_factory_test_mode;  //�Ƿ���빤������ģʽ

uint8_t LED_Red_pro = 50;            //LEDռ�ձȵ���
uint8_t LED_Green_pro = 25;
uint8_t LED_Blue_pro = 40;

bool led_blue_timerout = false;       //��������ʱ�䳬ʱ��־
bool led_red_timerout = false;        //�������ʱ�䳬ʱ��־

uint32_t bsp_led_indication(led_indication_t indicate)
{
		if(Is_pwm_init == false) //���û�г�ʼ��PWM����ʼ��PWM
		{
				led_pwm_init();
		}
		if(Is_led_timer_start)  //��������˶�ʱ�����ȹرն�ʱ��
		{
				led_timer_stop();
		}

    switch (indicate)
    {
			case  BSP_INDICATE_IDLE:                //�ر�LED�ͳ�ʱ��ʱ����PWMȥ��ʼ����LED������͵�ƽ
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
            m_stable_state = indicate;
				    break;

    	case  BSP_INDICATE_CONNECTED:           //��������״̬��������ʱ��ʱ��
      			LED_BLUE(250,LED_Blue_pro);  
						led_timer_start();
			      m_stable_state = indicate;	      //��¼��ǰled״̬�����ڳ�ʱ�Ұ������º�ָ���ǰ״̬
				    break;

	    case  BSP_INDICATE_WITH_WHITELIST:      //��������״̬������1HZƵ����˸��������ʱ��ʱ��
			      LED_BLUE(1,LED_Blue_pro);
						led_timer_start();
            m_stable_state = indicate;        //��¼��ǰled״̬�����ڳ�ʱ�Ұ������º�ָ���ǰ״̬
 			      break;

    	case  BSP_INDICATE_WITHOUT_WHITELIST:   //���ƿ�����˸��Ƶ��5HZ������󶨹���򿪳�ʱ��ʱ��
      			LED_BLUE(5,LED_Blue_pro);
						if(Is_device_bond)
						{
						    led_timer_start();
						}
            m_stable_state = indicate;			      
				    break;

	    case  BSP_INDICATE_Battery_LOW:         //���������˸��Ƶ��1HZ��������ʱ��ʱ����led�л��ȹر����е�
				    LED_OFF();
      			LED_RED(1,LED_Red_pro);
						led_timer_start();
            m_stable_state = indicate;
				    break;

	    case  BSP_INDICATE_Battery_CHARGING:    //���״̬����Ƴ�����led�л��ȹر����е�
				    if(Is_pwm_init == true)
						{
					    	PWM_uint();
						}
	          nrf_gpio_cfg_output(LED_GPIO_RED);
	          NRF_GPIO->OUTSET = 1<<LED_GPIO_RED;
            m_stable_state = indicate;
				    break;

    	case  BSP_INDICATE_Battery_CHARGEOVER:  //������״̬���̵Ƴ�����led�л��ȹر����е�
				    if(Is_pwm_init == true)       
						{
					    	PWM_uint();
						}
	          nrf_gpio_cfg_output(LED_GPIO_GREEN);
	          NRF_GPIO->OUTSET = 1<<LED_GPIO_GREEN;
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
    return NRF_SUCCESS;
}

void leds_state_update(void)                             //LED��ʱ��ʱ���ص�����
{
	  if(m_stable_state == BSP_INDICATE_CONNECTED ||       //��3��״̬�£���ʱ���־λ��1���ر����ƣ�����������£�LED�ָ�״̬��
			 m_stable_state == BSP_INDICATE_WITH_WHITELIST || 
		   m_stable_state == BSP_INDICATE_WITHOUT_WHITELIST)
		{
				LED_BLUE(1,0);
				led_blue_timerout = true;	
		}
	  if(m_stable_state == BSP_INDICATE_Battery_LOW)       //�͵���״̬���Ӻ󣬳�ʱ��ʱ���򿪣����ڳ�ʱ���Ƴ�ʱ��־��1
		{
				LED_RED(1,0);
				led_red_timerout = true;	
		}
		PWM_uint();                                          //LED�Ѿ�Ϩ�𣬽�PWMȥ��ʼ��
}
