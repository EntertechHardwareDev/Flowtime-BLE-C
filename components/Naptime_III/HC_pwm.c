#include "HC_pwm.h"

APP_PWM_INSTANCE(PWM1,1);                   // ����һ��ʹ�ö�ʱ��1����PWM����ʵ��
APP_PWM_INSTANCE(PWM2,2);                   // ����һ��ʹ�ö�ʱ��2����PWM����ʵ��
APP_PWM_INSTANCE(PWM3,3);                   // ����һ��ʹ�ö�ʱ��3����PWM����ʵ��

bool Is_pwm_init = false;

void led_pwm_init(void)
{
    ret_code_t err_code;
    
    /* 3·PWM */
    app_pwm_config_t pwm1_cfg = APP_PWM_DEFAULT_CONFIG_1CH(1000000L, LED_GPIO_RED);           //���PWM��ʼ��1HZ    
    app_pwm_config_t pwm2_cfg = APP_PWM_DEFAULT_CONFIG_1CH(1000000L/250, LED_GPIO_GREEN);     //�̵�PWM��ʼ��250HZ
    app_pwm_config_t pwm3_cfg = APP_PWM_DEFAULT_CONFIG_1CH(1000000L/5, LED_GPIO_BLUE);        //����PWM��ʼ��5HZ
	
	  pwm1_cfg.pin_polarity[0] = APP_PWM_POLARITY_ACTIVE_HIGH;
	  pwm2_cfg.pin_polarity[0] = APP_PWM_POLARITY_ACTIVE_HIGH;
	  pwm3_cfg.pin_polarity[0] = APP_PWM_POLARITY_ACTIVE_HIGH;
	
  	/* ��ʼ����ʹ��PWM. */
    err_code = app_pwm_init(&PWM1,&pwm1_cfg,NULL);
    APP_ERROR_CHECK(err_code);
    app_pwm_enable(&PWM1);//ʹ��PWM1
	
    err_code = app_pwm_init(&PWM2,&pwm2_cfg,NULL);
    APP_ERROR_CHECK(err_code);
    app_pwm_enable(&PWM2);//ʹ��PWM2

    err_code = app_pwm_init(&PWM3,&pwm3_cfg,NULL);
    APP_ERROR_CHECK(err_code);
    app_pwm_enable(&PWM3);//ʹ��PWM3	
		
    Is_pwm_init = true;                             //PWM��ʼ����־
  	LED_OFF();	                                    //�Ʋ���
}

void LED_RED(uint8_t fre,uint8_t PWM)
{
	  ret_code_t err_code;
	  PWM1_fre_changge(fre);                              //�ı�Ƶ��
	  err_code = app_pwm_channel_duty_set(&PWM1, 0, PWM); //����ռ�ձȣ��򿪺��
	  APP_ERROR_CHECK(err_code);
}

void LED_GREEN(uint8_t PWM)
{
	  ret_code_t err_code;
	  err_code = app_pwm_channel_duty_set(&PWM2, 0, PWM); //����ռ�ձȣ����̵�
	  APP_ERROR_CHECK(err_code);
}

void LED_BLUE(uint8_t fre,uint8_t PWM)
{
	  ret_code_t err_code;
	  PWM3_fre_changge(fre);                              //�ı�Ƶ��
	  err_code = app_pwm_channel_duty_set(&PWM3, 0, PWM); //����ռ�ձȣ�������
	  APP_ERROR_CHECK(err_code);
}

void LED_OFF(void)
{
	  ret_code_t err_code;
	  err_code = app_pwm_channel_duty_set(&PWM1, 0, 0);  
	  APP_ERROR_CHECK(err_code);
	  err_code = app_pwm_channel_duty_set(&PWM2, 0, 0);
    APP_ERROR_CHECK(err_code);  
	  err_code = app_pwm_channel_duty_set(&PWM3, 0, 0); 
	  APP_ERROR_CHECK(err_code);
}

void PWM_uint(void)
{
	  app_pwm_uninit(&PWM1);                             //PWMȥ��ʼ��
	  app_pwm_uninit(&PWM2);
	  app_pwm_uninit(&PWM3);
	
	  Is_pwm_init = false;                               //PWM��ʼ����־λ��0
	  nrf_gpio_cfg_output(LED_GPIO_BLUE);                //����LED��IO��Ϊ��ͨ���ģʽ
    nrf_gpio_cfg_output(LED_GPIO_RED);
	  nrf_gpio_cfg_output(LED_GPIO_GREEN);

	  NRF_GPIO->OUTCLR = 1<<LED_GPIO_BLUE;               //IO������͵�ƽ
	  NRF_GPIO->OUTCLR = 1<<LED_GPIO_RED;
	  NRF_GPIO->OUTCLR = 1<<LED_GPIO_GREEN;
}

void PWM1_fre_changge(uint8_t fre)  //�޸�Ƶ��
{
	  app_pwm_uninit(&PWM1);
	  ret_code_t err_code;
    
    app_pwm_config_t pwm1_cfg = APP_PWM_DEFAULT_CONFIG_1CH(1000000L/fre, LED_GPIO_RED);
    
	  pwm1_cfg.pin_polarity[0] = APP_PWM_POLARITY_ACTIVE_HIGH;
  	/* ��ʼ����ʹ��PWM. */
    err_code = app_pwm_init(&PWM1,&pwm1_cfg,NULL);
    APP_ERROR_CHECK(err_code);
	
    app_pwm_enable(&PWM1);//ʹ��PWM
}

void PWM3_fre_changge(uint8_t fre)  //�޸�Ƶ��
{
	  app_pwm_uninit(&PWM3);
	  ret_code_t err_code;
    
    app_pwm_config_t pwm3_cfg = APP_PWM_DEFAULT_CONFIG_1CH(1000000L/fre, LED_GPIO_BLUE);
    
	  pwm3_cfg.pin_polarity[0] = APP_PWM_POLARITY_ACTIVE_HIGH;
  	/* ��ʼ����ʹ��PWM. */
    err_code = app_pwm_init(&PWM3,&pwm3_cfg,NULL);
    APP_ERROR_CHECK(err_code);
	
    app_pwm_enable(&PWM3);//ʹ��PWM
}


