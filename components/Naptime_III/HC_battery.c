#include "HC_battery.h"
#include "math.h"

ble_bas_t                    m_bas;                     /**< Structure used to identify the battery service. */

//ȫ�ֱ���
double bat_vol;                         //ʵ�����
double saft_vol = 3.1;                  //��ȫ��ѹ
double min_work_vol = 3.3;              //�͵���
#define VOLTAGE_AVG_NUM  10             //��ص�ѹ�˲������С

extern bool Into_factory_test_mode;

extern void sleep_mode_enter(void);

void ble_battory_serv_init(void)    //��ط����ʼ��
{
	  uint32_t        err_code;
	  ble_bas_init_t  bas_init;

    memset(&bas_init, 0, sizeof(bas_init));

    // Here the sec level for the Battery Service can be changed/increased.
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&bas_init.battery_level_char_attr_md.cccd_write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&bas_init.battery_level_char_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&bas_init.battery_level_char_attr_md.write_perm);

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&bas_init.battery_level_report_read_perm);

    bas_init.evt_handler          = NULL;
    bas_init.support_notification = false;
    bas_init.p_report_ref         = NULL;
    bas_init.initial_batt_level   = 100;

    err_code = ble_bas_init(&m_bas, &bas_init);
    APP_ERROR_CHECK(err_code);
}

void saadc_callback(nrf_drv_saadc_evt_t const * p_event)
{

}																	 

void saadc_init(void)
{
    ret_code_t err_code;
    nrf_saadc_channel_config_t channel_config = NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN6);   //PIN30��adc����ͨ����
    err_code = nrf_drv_saadc_init(NULL, saadc_callback);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_saadc_channel_init(0, &channel_config);
    APP_ERROR_CHECK(err_code);
	
	  nrf_gpio_cfg_output(TPS_CTRL);   
	  NRF_GPIO->OUTSET = 1<<TPS_CTRL;	 
}

void battery_level_update(void)                    //��ص������µ�bat_vol
{
    uint32_t err_code;
	  uint8_t bat_vol_pre;                           //�����ٷֱ�
    static uint8_t bat_vol_arrary_index = 0;
    static double bat_vol_arrary[VOLTAGE_AVG_NUM] = {0};

	  nrf_saadc_value_t  ADC_value = 0;	             //ADC��ȡ����
  	nrf_drv_saadc_sample_convert(0,&ADC_value);
  	bat_vol = ADC_value * 3.60 / 1024.0 * 2;       //��ز�����ѹ
		
		if( bat_vol_arrary[0] == 0 )                   
		{
				for( int i = 0; i < VOLTAGE_AVG_NUM; i++ )
			  {
						bat_vol_arrary[i] = bat_vol;
				}
		}
		else
		{
				bat_vol_arrary[bat_vol_arrary_index] = bat_vol;
				bat_vol_arrary_index = ( bat_vol_arrary_index + 1 )%VOLTAGE_AVG_NUM;
				if(bat_vol_arrary_index == 0)              //һ��ѭ������һ��ƽ����
				{
						bat_vol = 0;
						for( int i = 0; i < VOLTAGE_AVG_NUM; i++ )
						{
								bat_vol += bat_vol_arrary[i];
						}
						bat_vol = bat_vol / VOLTAGE_AVG_NUM;
						bat_vol_pre = (uint8_t)((bat_vol - 3.10 ) * 100);     //�����ٷֱ�
//	          SEGGER_RTT_printf(0,"\r Voltage %d \r\n",bat_vol_pre);
						
						if(bat_vol_pre > 100)
							 bat_vol_pre = 100;
						do{
							 err_code = ble_bas_battery_level_update(&m_bas, bat_vol_pre);
						}while(err_code == BLE_ERROR_NO_TX_PACKETS);
						
						if(bat_vol < saft_vol)                         //����3.1V,�ػ�
						{
								SEGGER_RTT_printf(0,"\r Voltage is lower than 3.1V \r\n");
								sleep_mode_enter();
						}
			 }
		}

	
}

void Power_Check(void)
{
	  nrf_saadc_value_t  ADC_value = 0;	              //ADC��ȡ����
  	nrf_drv_saadc_sample_convert(0,&ADC_value);
	  bat_vol = ADC_value * 3.6 / 1024.0 * 2;         //��ص�ѹʵ�ʵ�ѹ
	
		if(bat_vol < saft_vol)                          //����3.1V�޷�����
		{
			  SEGGER_RTT_printf(0,"\r Voltage is lower than 3.1V \r\n");
			  sleep_mode_enter();
		}
		else
			  SEGGER_RTT_printf(0,"\r Voltage is higher than 3.1V \r\n");
}

uint8_t connected_power_check(void)
{
		nrf_saadc_value_t  ADC_value = 0;	              //�������,�ȳ�ʼ��ADC
  	nrf_drv_saadc_sample_convert(0,&ADC_value);
	  bat_vol = ADC_value * 3.6 / 1024.0 * 2;         //��ص�ѹ
	
	  if(bat_vol < min_work_vol)                      //����ʹ�õ�ѹ
	  {
			 SEGGER_RTT_printf(0,"\r Voltage is lower than 3.2V \r\n");
			 return true;
	  }
		else
		{
			 SEGGER_RTT_printf(0,"\r Voltage is higher than 3.2V \r\n");
			 return false;
		}
}

void charging_check(void)
{
		uint32_t err_code;	   
		while(nrf_gpio_pin_read(BQ_PG) == 0 && !Into_factory_test_mode)     //USB������Ϊ�ǹ�������ģʽ����粻ִ������������
		{
			 nrf_delay_ms(500);
			 if(nrf_gpio_pin_read(BQ_CHG) == 0)   //charging
			 {
				  err_code = bsp_led_indication(BSP_INDICATE_Battery_CHARGING);
           APP_ERROR_CHECK(err_code);
			 }
			 else                                 //charging_over
			 {
				  err_code = bsp_led_indication(BSP_INDICATE_Battery_CHARGEOVER);
           APP_ERROR_CHECK(err_code);
			 }
		}
		err_code = bsp_led_indication(BSP_INDICATE_IDLE);
    APP_ERROR_CHECK(err_code);
		
		battery_timer_start();   //������ʱ�ɼ���ѹ
}
