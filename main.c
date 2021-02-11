#include <stdbool.h>
#include <stdint.h>

#include "app_scheduler.h"
#include "app_timer.h"
#include "boards.h"
#include "nrf_bootloader_info.h"
#include "nrf_delay.h"
#include "nrf_dfu.h"
#include "nrf_dfu_settings.h"
#include "nrf_dfu_utils.h"
#include "nrf_dfu_validation.h"
#include "nrf_drv_clock.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_pwr_mgmt.h"

#define SCHED_QUEUE_SIZE      64
#define SCHED_EVENT_DATA_SIZE NRF_DFU_SCHED_EVENT_DATA_SIZE

static void log_init(void) {
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

APP_TIMER_DEF(led_timer_id);
void led_timer_handler(void * p_context) {
    static uint8_t led = 0;
    bsp_board_led_invert(led);
    led = (led + 1) % LEDS_NUMBER;
}

void m_user_observer(nrf_dfu_evt_type_t notification) {
    switch (notification) {
        case NRF_DFU_EVT_DFU_INITIALIZED:
            NRF_LOG_INFO("m_user_observer: NRF_DFU_EVT_DFU_INITIALIZED");
            break;
        case NRF_DFU_EVT_TRANSPORT_ACTIVATED:
            NRF_LOG_INFO("m_user_observer: NRF_DFU_EVT_TRANSPORT_ACTIVATED");
            break;
        case NRF_DFU_EVT_TRANSPORT_DEACTIVATED:
            NRF_LOG_INFO("m_user_observer: NRF_DFU_EVT_TRANSPORT_DEACTIVATED");
            break;
        case NRF_DFU_EVT_DFU_STARTED:
            NRF_LOG_INFO("m_user_observer: NRF_DFU_EVT_DFU_STARTED");
            break;
        case NRF_DFU_EVT_OBJECT_RECEIVED:
            NRF_LOG_INFO("m_user_observer: NRF_DFU_EVT_OBJECT_RECEIVED");
            break;
        case NRF_DFU_EVT_DFU_FAILED:
            NRF_LOG_INFO("m_user_observer: NRF_DFU_EVT_DFU_FAILED");
            break;
        case NRF_DFU_EVT_DFU_COMPLETED:
            NRF_LOG_INFO("m_user_observer: NRF_DFU_EVT_DFU_COMPLETED");
            NRF_LOG_INFO("we can restart now");
            NRF_LOG_FINAL_FLUSH();
            nrf_pwr_mgmt_shutdown(NRF_PWR_MGMT_SHUTDOWN_RESET);
            break;
        case NRF_DFU_EVT_DFU_ABORTED:
            NRF_LOG_INFO("m_user_observer: NRF_DFU_EVT_DFU_ABORTED");
            break;
        default:
            break;
    }
}

static void scheduler_init(void) {
    APP_SCHED_INIT(SCHED_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
    NRF_LOG_INFO("Scheduler initialized");
}

void dfu_init(void) {
    uint32_t err_code;

    scheduler_init();

    nrf_dfu_settings_init(true);

    err_code = nrf_dfu_init(m_user_observer);
    APP_ERROR_CHECK(err_code);
}

int main(void) {
    log_init();

    nrf_drv_clock_init();
    nrf_drv_clock_lfclk_request(NULL);
    while (!nrf_drv_clock_lfclk_is_running()) { /* Just waiting */
    }

    app_timer_init();
    app_timer_create(&led_timer_id, APP_TIMER_MODE_REPEATED, led_timer_handler);
    app_timer_start(led_timer_id, APP_TIMER_TICKS(250), NULL);

    /* Configure board. */
    bsp_board_init(BSP_INIT_LEDS);

    dfu_init();
    NRF_LOG_INFO("Application version %04x", s_dfu_settings.app_version);

    // NRF_LOG_INFO("update");

    /* Toggle LEDs. */
    while (true) {
        app_sched_execute();
        NRF_LOG_PROCESS();
    }
}
