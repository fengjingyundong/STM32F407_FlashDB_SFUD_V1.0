#include "flashdb_app.h"
#include <flashdb.h>
#include <string.h>
#include "main.h"
#include "rtc.h"
#include "time.h"

static uint32_t boot_count = 0;
static time_t boot_time[10] = {0, 1, 2, 3};
/* default KV nodes */
static struct fdb_default_kv_node default_kv_table[] = {
    {"username", "armink", 0},                       /* string KV */
    {"password", "123456", 0},                       /* string KV */
    {"boot_count", &boot_count, sizeof(boot_count)}, /* int type KV */
    {"boot_time", &boot_time, sizeof(boot_time)},    /* int array type KV */
};

static struct fdb_kvdb kvdb = {0}; /* KVDB object */
struct fdb_tsdb tsdb = {0};        /* TSDB object */
static int counts = 0;             /* counts for simulated timestamp */

static void kvdb_basic_sample(fdb_kvdb_t kvdb);
static void kvdb_type_string_sample(fdb_kvdb_t kvdb);
static void kvdb_type_blob_sample(fdb_kvdb_t kvdb);
static void tsdb_sample(fdb_tsdb_t tsdb);

static void lock(fdb_db_t db)
{
        __disable_irq();
}

static void unlock(fdb_db_t db)
{
        __enable_irq();
}

static fdb_time_t get_time(void)
{
        /* Using the counts instead of timestamp.
         * Please change this function to return RTC time.
         */
        struct tm timeStruct = {0};
        fdb_time_t curr_time = {0};
        static fdb_time_t last_time = {0};

        // 他们使用 STM32F407 的 RTC 时钟代替计数器
        RTC_TimeTypeDef sTime = {0};
        RTC_DateTypeDef sDate = {0};

        // Read RTC time and date
        HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
        HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN); // Date must be read to unlock time

        // 转换为 Unix 时间戳

        timeStruct.tm_year = sDate.Year + 100; // tm_year = 年份 - 1900；RTC 的年份偏移是从 2000 开始
        timeStruct.tm_mon = sDate.Month - 1;   // tm_mon 从 0 开始表示 1 月
        timeStruct.tm_mday = sDate.Date;
        timeStruct.tm_hour = sTime.Hours;
        timeStruct.tm_min = sTime.Minutes;
        timeStruct.tm_sec = sTime.Seconds;
        timeStruct.tm_isdst = 0; // 无夏令时

        counts++; /* 计数增加 存储测试使用 */

        curr_time = mktime(&timeStruct); // 转换为 time_t
        if (curr_time <= last_time)
        {
                curr_time = last_time + 1; /* 同一秒内再次调用，加 1 伪造时间戳 */
        }
        last_time = curr_time;

        return curr_time;
}

#define FDB_LOG_TAG "[demo][app]"
static void kvdb_basic_sample(fdb_kvdb_t kvdb)
{
        struct fdb_blob blob;
        int boot_count = 0;

        FDB_INFO("==================== kvdb_basic_sample ====================\n");

        { /* GET the KV value */
                /* get the "boot_count" KV value */
                fdb_kv_get_blob(kvdb, "boot_count", fdb_blob_make(&blob, &boot_count, sizeof(boot_count)));
                /* the blob.saved.len is more than 0 when get the value successful */
                if (blob.saved.len > 0)
                {
                        FDB_INFO("get the 'boot_count' value is %d\n", boot_count);
                }
                else
                {
                        FDB_INFO("get the 'boot_count' failed\n");
                }
        }

        { /* CHANGE the KV value */
                /* increase the boot count */
                boot_count++;
                /* change the "boot_count" KV's value */
                fdb_kv_set_blob(kvdb, "boot_count", fdb_blob_make(&blob, &boot_count, sizeof(boot_count)));
                FDB_INFO("set the 'boot_count' value to %d\n", boot_count);
        }

        FDB_INFO("===========================================================\n");
}

static void kvdb_type_string_sample(fdb_kvdb_t kvdb)
{
        FDB_INFO("==================== kvdb_type_string_sample ====================\n");

        { /* CREATE new Key-Value */
                char temp_data[10] = "36C";

                /* It will create new KV node when "temp" KV not in database. */
                fdb_kv_set(kvdb, "temp", temp_data);
                FDB_INFO("create the 'temp' string KV, value is: %s\n", temp_data);
        }

        { /* GET the KV value */
                char *return_value, temp_data[10] = {0};

                /* Get the "temp" KV value.
                 * NOTE: The return value saved in fdb_kv_get's buffer. Please copy away as soon as possible.
                 */
                return_value = fdb_kv_get(kvdb, "temp");
                /* the return value is NULL when get the value failed */
                if (return_value != NULL)
                {
                        strncpy(temp_data, return_value, sizeof(temp_data));
                        FDB_INFO("get the 'temp' value is: %s\n", temp_data);
                }
        }

        { /* CHANGE the KV value */
                char temp_data[10] = "38C";

                /* change the "temp" KV's value to "38.1" */
                fdb_kv_set(kvdb, "temp", temp_data);
                FDB_INFO("set 'temp' value to %s\n", temp_data);
        }

        { /* DELETE the KV by name */
                fdb_kv_del(kvdb, "temp");
                FDB_INFO("delete the 'temp' finish\n");
        }

        FDB_INFO("===========================================================\n");
}

static void kvdb_type_blob_sample(fdb_kvdb_t kvdb)
{
        struct fdb_blob blob;

        FDB_INFO("==================== kvdb_type_blob_sample ====================\n");

        { /* CREATE new Key-Value */
                int temp_data = 36;

                /* It will create new KV node when "temp" KV not in database.
                 * fdb_blob_make: It's a blob make function, and it will return the blob when make finish.
                 */
                fdb_kv_set_blob(kvdb, "temp", fdb_blob_make(&blob, &temp_data, sizeof(temp_data)));
                FDB_INFO("create the 'temp' blob KV, value is: %d\n", temp_data);
        }

        { /* GET the KV value */
                int temp_data = 0;

                /* get the "temp" KV value */
                fdb_kv_get_blob(kvdb, "temp", fdb_blob_make(&blob, &temp_data, sizeof(temp_data)));
                /* the blob.saved.len is more than 0 when get the value successful */
                if (blob.saved.len > 0)
                {
                        FDB_INFO("get the 'temp' value is: %d\n", temp_data);
                }
        }

        { /* CHANGE the KV value */
                int temp_data = 38;

                /* change the "temp" KV's value to 38 */
                fdb_kv_set_blob(kvdb, "temp", fdb_blob_make(&blob, &temp_data, sizeof(temp_data)));
                FDB_INFO("set 'temp' value to %d\n", temp_data);
        }

        { /* DELETE the KV by name */
                fdb_kv_del(kvdb, "temp");
                FDB_INFO("delete the 'temp' finish\n");
        }

        FDB_INFO("===========================================================\n");
}

#ifdef FDB_USING_TIMESTAMP_64BIT
#define __PRITS "lld"
#else
#define __PRITS "d"
#endif

struct env_status
{
        int temp;
        int humi;
};

static bool query_cb(fdb_tsl_t tsl, void *arg);
static bool query_by_time_cb(fdb_tsl_t tsl, void *arg);
static bool set_status_cb(fdb_tsl_t tsl, void *arg);

static void tsdb_sample(fdb_tsdb_t tsdb)
{
        struct fdb_blob blob;

        FDB_INFO("==================== tsdb_sample ====================\n");

        { /* APPEND new TSL (time series log) */
                struct env_status status;

                /* append new log to TSDB */
                status.temp = 36;
                status.humi = 85;
                fdb_tsl_append(tsdb, fdb_blob_make(&blob, &status, sizeof(status)));
                FDB_INFO("append the new status.temp (%d) and status.humi (%d)\n", status.temp, status.humi);

                status.temp = 38;
                status.humi = 90;
                fdb_tsl_append(tsdb, fdb_blob_make(&blob, &status, sizeof(status)));
                FDB_INFO("append the new status.temp (%d) and status.humi (%d)\n", status.temp, status.humi);
        }

        { /* QUERY the TSDB */
                /* query all TSL in TSDB by iterator */
                fdb_tsl_iter(tsdb, query_cb, tsdb);
        }

        { /* QUERY the TSDB by time */
                /* prepare query time (from 1970-01-01 00:00:00 to 2020-05-05 00:00:00) */
                struct tm tm_from = {.tm_year = 1970 - 1900, .tm_mon = 0, .tm_mday = 1, .tm_hour = 0, .tm_min = 0, .tm_sec = 0};
                struct tm tm_to = {.tm_year = 2020 - 1900, .tm_mon = 4, .tm_mday = 5, .tm_hour = 0, .tm_min = 0, .tm_sec = 0};
                time_t from_time = mktime(&tm_from), to_time = mktime(&tm_to);
                size_t count;
                /* query all TSL in TSDB by time */
                fdb_tsl_iter_by_time(tsdb, from_time, to_time, query_by_time_cb, tsdb);
                /* query all FDB_TSL_WRITE status TSL's count in TSDB by time */
                count = fdb_tsl_query_count(tsdb, from_time, to_time, FDB_TSL_WRITE);
                FDB_INFO("query count is: %zu\n", count);
        }

        { /* SET the TSL status */
                /* Change the TSL status by iterator or time iterator
                 * set_status_cb: the change operation will in this callback
                 *
                 * NOTE: The actions to modify the state must be in order.
                 *       like: FDB_TSL_WRITE -> FDB_TSL_USER_STATUS1 -> FDB_TSL_DELETED -> FDB_TSL_USER_STATUS2
                 *       The intermediate states can also be ignored.
                 *       such as: FDB_TSL_WRITE -> FDB_TSL_DELETED
                 */
                fdb_tsl_iter(tsdb, set_status_cb, tsdb);
        }

        FDB_INFO("===========================================================\n");
}

static bool query_cb(fdb_tsl_t tsl, void *arg)
{
        struct fdb_blob blob;
        struct env_status status;
        fdb_tsdb_t db = arg;

        fdb_blob_read((fdb_db_t)db, fdb_tsl_to_blob(tsl, fdb_blob_make(&blob, &status, sizeof(status))));
        FDB_INFO("[query_cb] queried a TSL: time: %" __PRITS ", temp: %d, humi: %d\n", tsl->time, status.temp, status.humi);

        return false;
}

static bool query_by_time_cb(fdb_tsl_t tsl, void *arg)
{
        struct fdb_blob blob;
        struct env_status status;
        fdb_tsdb_t db = arg;

        fdb_blob_read((fdb_db_t)db, fdb_tsl_to_blob(tsl, fdb_blob_make(&blob, &status, sizeof(status))));
        FDB_INFO("[query_by_time_cb] queried a TSL: time: %" __PRITS ", temp: %d, humi: %d\n", tsl->time, status.temp, status.humi);

        return false;
}

static bool set_status_cb(fdb_tsl_t tsl, void *arg)
{
        fdb_tsdb_t db = arg;

        FDB_INFO("set the TSL (time %" __PRITS ") status from %d to %d\n", tsl->time, tsl->status, FDB_TSL_USER_STATUS1);
        fdb_tsl_set_status(db, tsl, FDB_TSL_USER_STATUS1);

        return false;
}

/* FlashDB_App_Start FlashDB测试demo程序 */
int flashdb_app_Start(void)
{
        fdb_err_t result;

#ifdef FDB_USING_KVDB
        { /* KVDB Sample */
                struct fdb_default_kv default_kv;

                default_kv.kvs = default_kv_table;
                default_kv.num = sizeof(default_kv_table) / sizeof(default_kv_table[0]);
                /* set the lock and unlock function if you want */
                fdb_kvdb_control(&kvdb, FDB_KVDB_CTRL_SET_LOCK, (void *)lock);
                fdb_kvdb_control(&kvdb, FDB_KVDB_CTRL_SET_UNLOCK, (void *)unlock);
                /* Key-Value database initialization
                 *
                 *       &kvdb: database object
                 *       "env": database name
                 * "fdb_kvdb1": The flash partition name base on FAL. Please make sure it's in FAL partition table.
                 *              Please change to YOUR partition name.
                 * &default_kv: The default KV nodes. It will auto add to KVDB when first initialize successfully.
                 *        NULL: The user data if you need, now is empty.
                 */
                result = fdb_kvdb_init(&kvdb, "env", "fdb_kvdb1", &default_kv, NULL);

                if (result != FDB_NO_ERR)
                {
                        return -1;
                }

                /* run basic KV samples */
                kvdb_basic_sample(&kvdb);
                /* run string KV samples */
                kvdb_type_string_sample(&kvdb);
                /* run blob KV samples */
                kvdb_type_blob_sample(&kvdb);
        }
#endif /* FDB_USING_KVDB */

#ifdef FDB_USING_TSDB
        { /* TSDB Sample */
                /* set the lock and unlock function if you want */
                fdb_tsdb_control(&tsdb, FDB_TSDB_CTRL_SET_LOCK, (void *)lock);
                fdb_tsdb_control(&tsdb, FDB_TSDB_CTRL_SET_UNLOCK, (void *)unlock);
                /* Time series database initialization
                 *
                 *       &tsdb: database object
                 *       "log": database name
                 * "fdb_tsdb1": The flash partition name base on FAL. Please make sure it's in FAL partition table.
                 *              Please change to YOUR partition name.
                 *    get_time: The get current timestamp function.
                 *         128: maximum length of each log
                 *        NULL: The user data if you need, now is empty.
                 */
                result = fdb_tsdb_init(&tsdb, "log", "fdb_tsdb1", get_time, 128, NULL);
                /* read last saved time for simulated timestamp */
                fdb_tsdb_control(&tsdb, FDB_TSDB_CTRL_GET_LAST_TIME, &counts);

                if (result != FDB_NO_ERR)
                {
                        return -1;
                }

                /* run TSDB sample */
                tsdb_sample(&tsdb);
        }
#endif /* FDB_USING_TSDB */
        return 0;
}

sfud_flash sfud_norflash0 = {
    .name = "norflash0",
    .spi.name = "SPI1",
    .chip = {"W25Q128BV", SFUD_MF_ID_WINBOND, 0x40, 0x18, 16L * 1024L * 1024L, SFUD_WM_PAGE_256B, 4096, 0x20}};

int spi_flash_init(void)
{
        /* SFUD initialize */
        if (sfud_device_init(&sfud_norflash0) == SFUD_SUCCESS)
        {
                return 0;
        }
        else
        {
                return -1;
        }
}
