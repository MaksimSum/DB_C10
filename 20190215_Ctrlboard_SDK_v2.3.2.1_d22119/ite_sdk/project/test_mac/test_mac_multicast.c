﻿#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ite/itp.h"
#include "ite/ite_mac.h"


//======
#include "lwip/ip_addr.h"

#define err()  do { vTaskSuspendAll(); while(1); } while(0)

#define IGMP_ADD_MAC_FILTER     1
#define IGMP_DEL_MAC_FILTER     2

struct dev_mc_list
{
    struct dev_mc_list *next;
    ip_addr_t ip_addr;
};

static struct dev_mc_list* mc_list;
static int mc_count;
static uint8_t *mc_array;

static uint8_t mc_addr[256][6]; /* from 1 ~ 255, index 0 not used */
static int  mc_mac_index;

/*
*	Map a multicast IP onto multicast MAC for type ethernet.
*/
static inline void ip_eth_mc_map(uint8_t *buf, ip_addr_t *addr)
{
    buf[0] = 0x01;
    buf[1] = 0x00;
    buf[2] = 0x5e;
    buf[3] = ip4_addr2(addr) & 0x7F;
    buf[4] = ip4_addr3(addr) & 0xFF;
    buf[5] = ip4_addr4(addr) & 0xFF;
}

static void gen_mc_mac_addr(void)
{
    int i;
    ip_addr_t ip;
    uint8_t *mac_addr;

    for (i = 1; i <= 255; i++) {
        IP4_ADDR(&ip, 224, 0, 0, i);
        mac_addr = (uint8_t*)&mc_addr[i][0];
        ip_eth_mc_map(mac_addr, &ip);
    }
}

static void mc_upload(void)
{
#define MAX_ADDR_LEN    8

    int flag = 0;
    struct dev_mc_list* mc;

    /*if(mc_count > 64)
    flag |= IFF_ALLMULTI;
    else*/ if (mc_count > 0)
    {
        int i = 0;
        uint8_t *mac_addr;

        flag |= IFF_MULTICAST;
        if (mc_array)
            free(mc_array);
        mc_array = (uint8_t*)malloc(MAX_ADDR_LEN * mc_count);
        memset(mc_array, 0x0, (MAX_ADDR_LEN * mc_count));

        for (mc = mc_list; mc != NULL; mc = mc->next)
        {
            mac_addr = mc_array + MAX_ADDR_LEN*i;
            ip_eth_mc_map(mac_addr, &mc->ip_addr);
            i++;
        }
        if (i != mc_count)
            printf(" mc_count => %d != %d ?????????????\n", i, mc_count);
    }

    iteMacSetRxMode(flag, mc_array, mc_count);

    return;
}

int eth_igmp_mac_filter(ip_addr_t *addr, u8_t action)
{
    int err = 0;
    int i;
    struct dev_mc_list *mc, *mc1;

    ithEnterCritical();

    if (action == IGMP_ADD_MAC_FILTER)
    {
        mc1 = (struct dev_mc_list*)malloc(sizeof(*mc1));
        if (mc1 == NULL)
        {
            err = -2;
            goto free;
        }

        for (mc = mc_list; mc != NULL; mc = mc->next)
        {
            if (ip_addr_cmp(&mc->ip_addr, addr))
                goto free;
        }

        memset((void*)mc1, 0x0, sizeof(*mc1));
        ip_addr_copy(mc1->ip_addr, *addr);
        mc1->next = mc_list;
        mc_list = mc1;
        mc_count++;
    }

    if (action == IGMP_DEL_MAC_FILTER)
    {
        if (mc_count == 0)
            goto done;

        /*  is it the first mc? */
        if (ip_addr_cmp(&mc_list->ip_addr, addr))
        {
            mc = mc_list;
            mc_list = mc_list->next;
            free(mc);
        }
        else
        {
            for (mc = mc_list; mc != NULL; mc = mc->next)
            {
                if ((mc->next) && ip_addr_cmp(&((mc->next)->ip_addr), addr))
                {
                    mc1 = mc->next;
                    mc->next = (mc->next)->next;
                    free(mc1);
                    break;
                }
            }
            if (mc == NULL)
                goto done;
        }
        mc_count--;
    }

    mc_upload();

done:
    ithExitCritical();
    return 0;

free:
    ithExitCritical();
    if (mc1)
        free(mc1);
    return err;
}

//======



#define TEST_PACKET_NUM     255
#define MAX_PACKET_SIZE		64

static uint8_t pattern[MAX_PACKET_SIZE];
static uint8_t tx_addr[256][MAX_PACKET_SIZE];

struct eth_packet {
    uint8_t     *addr;
    uint32_t    size;
};

static struct eth_packet tx_packet[TEST_PACKET_NUM];
static struct eth_packet rx_packet[5];

static sem_t sem;
static uint32_t rx_cnt = 0;
static uint32_t rx_index;


void RxCallback(void* ctx, void* data, int len)
{
    rx_packet[rx_cnt].addr = (uint8_t *)data;
    rx_packet[rx_cnt].size = (uint32_t)len;
    rx_cnt++;

	if (memcmp((void*)data, (void*)mc_addr[mc_mac_index], 6) == 0)
        sem_post(&sem);
}


static uint8_t macaddr[] = {0x02, 0x22, 0x33, 0x44, 0x55, 0x66};
static const uint8_t ioConfig[] = { CFG_GPIO_ETHERNET };
static ITE_MAC_CFG_T mac_cfg;
static uint8_t *tx_pattern_start;

static int
Initialize(void)
{
    int res, i;

    mac_cfg.flags |= (ITH_COUNT_OF(ioConfig) == ITE_MAC_GRMII_PIN_CNT) ? ITE_MAC_RGMII : 0;
    mac_cfg.clk_inv 	  = CFG_NET_MAC_CLOCK_INVERSE;
    mac_cfg.clk_delay	  = CFG_NET_MAC_CLOCK_DELAY;
    mac_cfg.phyAddr = CFG_NET_ETHERNET_PHY_ADDR;
    mac_cfg.ioConfig = ioConfig;
    mac_cfg.linkGpio = CFG_GPIO_ETHERNET_LINK;
    mac_cfg.phy_link_change = itpPhyLinkChange;
    mac_cfg.linkGpio_isr  = itpPhylinkIsr;
    mac_cfg.phy_link_status = itpPhyLinkStatus;
    mac_cfg.phy_read_mode = itpPhyReadMode;

#if defined(CFG_GPIO_ETHERNET_LINK)
    // enable gpio interrupt
    ithIntrEnableIrq(ITH_INTR_GPIO);
#endif
    res = iteMacInitialize(&mac_cfg);
    if(res)
    {
        printf(" iteMacInitialize() fail! \n");
        err();
    }
    res = iteMacSetMacAddr(macaddr);
    if(res)
    {
        printf(" iteMacSetMacAddr() fail! \n");
        err();
    }
    PhyInit(CFG_ETH_MODE);
	
    /** fill test pattern */
	for (i = 0; i < MAX_PACKET_SIZE; i++)
		pattern[i] = (uint8_t)((i+1) % 0x100);

    gen_mc_mac_addr();

    /** generate first loop's tx data */
    for (i = 0; i < TEST_PACKET_NUM; i++)
        tx_packet[i].addr = tx_addr[i];

    tx_packet[0].size = MAX_PACKET_SIZE;
    tx_pattern_start = pattern;
    memcpy((void*)tx_addr[0], (void*)tx_pattern_start++, tx_packet[0].size);
    memcpy((void*)tx_addr[0], (void*)&mc_addr[1][0], 6);

    
    for (i = 1; i < TEST_PACKET_NUM; i++) {
        if ((tx_packet[i - 1].size - 1) < 64) {
            tx_packet[i].size = MAX_PACKET_SIZE;
            tx_pattern_start = pattern;
            memcpy((void*)tx_addr[i], (void*)tx_pattern_start++, tx_packet[i].size);
        }
        else {
            tx_packet[i].size = tx_packet[i - 1].size - 1;
            memcpy((void*)tx_addr[i], (void*)tx_pattern_start++, tx_packet[i].size);
        }
        memcpy((void*)tx_addr[i], (void*)&mc_addr[i + 1][0], 6);
    }

    return res;
}

void GenerateTxData(void)
{
    int i;

    /* for tx packet 0 */
    if ((tx_packet[TEST_PACKET_NUM - 2].size - 1) < 64) {
        tx_packet[0].size = MAX_PACKET_SIZE;
        tx_pattern_start = pattern;
    }
    else
        tx_packet[0].size = tx_packet[TEST_PACKET_NUM - 2].size - 1;

    memcpy((void*)tx_addr[0], (void*)tx_pattern_start++, tx_packet[0].size);
    memcpy((void*)tx_addr[0], (void*)&mc_addr[1][0], 6);

    /* for tx packet 1 ~ TEST_PACKET_NUM-1 */
    for (i = 1; i < TEST_PACKET_NUM; i++) {
        if ((tx_packet[i - 1].size - 1) < 64) {
            tx_packet[i].size = MAX_PACKET_SIZE;
            tx_pattern_start = pattern;
            memcpy((void*)tx_addr[i], (void*)tx_pattern_start++, tx_packet[i].size);
        }
        else {
            tx_packet[i].size = tx_packet[i - 1].size - 1;
            memcpy((void*)tx_addr[i], (void*)tx_pattern_start++, tx_packet[i].size);
        }
        memcpy((void*)tx_addr[i], (void*)&mc_addr[i + 1][0], 6);
    }
}

extern int iteMacDump(void);
static uint8_t tt[MAX_PACKET_SIZE];

static int
MainLoop(
    void)
{
    int res = 0, i, j, pass;
    uint8_t macaddr[6];
    int linkup = 0;
    ip_addr_t ip;
	uint32_t loopCnt = 0;

    res = iteMacOpen(macaddr, RxCallback, NULL, CFG_ETH_MODE);
    if(res)
    {
        printf(" iteMacOpen() fail 0x%08X \n", res);
        err();
    }

#if (CFG_ETH_MODE != ITE_ETH_MAC_LB)
wait_linkup:
    printf("\n wait link up...... \n\n");
    while(!iteEthGetLink()) usleep(10*1000);
	
    linkup = 1;
    printf("\n link up! \n\n");
#endif

    for(;;)
    {
        for (mc_mac_index = 1; mc_mac_index <= 255; mc_mac_index++) {
            printf("mc_mac_index: %d \n", mc_mac_index);
            IP4_ADDR(&ip, 224, 0, 0, mc_mac_index);
            eth_igmp_mac_filter(&ip, IGMP_ADD_MAC_FILTER);
            rx_cnt = 0;
            pass = 0;

            for (j = 0; j < 255; j++)
                {
                res = iteMacSend(tx_packet[j].addr, tx_packet[j].size);
                if (res) {
                    printf("iteMacSend() res=0x%X \n", res);
                    err();
                }
            }

            res = itpSemWaitTimeout(&sem, 6000);
            if (res) {
                printf(" timeout! \n");
                printf("fail ?????????? mc_mac_index = %d, rx_cnt:%d \n", mc_mac_index, rx_cnt);
                if (rx_cnt > 0)
                    goto dump_rx;
                err();
            }
            pass = 1;
            usleep(3 * 1000 * 1000);

dump_rx:
            printf("rx_cnt = %d \n", rx_cnt);
            for (i = 0; i<6; i++)
                printf("%02x ", mc_addr[mc_mac_index][i]);
            printf(" Tx \r\n");

            /* something wrong.... */
            if (!pass || (rx_cnt > 2)) {
                for (i = 0; i < rx_cnt; i++) {
                    printf("\r\n[%04d, %d] \n", rx_packet[i].size, i);
                    for (j = 0; j < rx_packet[i].size; j++) {
                        if (!(j % 0xF))
                            printf("\n");
                        printf("%02x ", rx_packet[i].addr[j]);
                        if (!(j % 0x80)) usleep(5 * 1000); // delay for print buffer
                    }
                    printf(" Rx \r\n");

#if 1// Dump Tx packet
                    {
                        int tx_index = rx_packet[i].addr[5] - 1;
                        for (j = 0; j < tx_packet[tx_index].size; j++) {
                            if (!(j % 0xF))
                                printf("\n");
                            printf("%02x ", tx_packet[tx_index].addr[j]);
                            if (!(j % 0x80)) usleep(5 * 1000); // delay for print buffer
                        }
                        printf(" Tx (len:%d) \r\n", tx_packet[tx_index].size);
            }
#endif
            }
            }
            /* pass */
            else {
                for (i = 0; i < rx_cnt; i++) {
                    printf("\r\n[%04d, %d] \n", rx_packet[i].size, i);
                    for (j = 0; j < 6; j++)
                        printf("%02x ", rx_packet[i].addr[j]);
                    printf(" Rx \r\n");
                }
            }

            if (pass == 0) {
                //iteMacDump();
                err();
			}

			eth_igmp_mac_filter(&ip, IGMP_DEL_MAC_FILTER);
            GenerateTxData(); // for next loop use
        }
    }

    return res;
}


static int
Terminate(void)
{
    return 0;
}


void* TestFunc(void* arg)
{
	int res;
	
#define TEST_STACK_SIZE 102400
	pthread_t task;
	pthread_attr_t attr;
	struct sched_param param;

	pthread_attr_init(&attr);
	pthread_attr_setstacksize(&attr, TEST_STACK_SIZE);
	param.sched_priority = sched_get_priority_max(1);
	pthread_attr_setschedparam(&attr, &param);
	pthread_create(&task, &attr, iteMacThreadFunc, NULL);

    sem_init(&sem, 0, 0);

	res = Initialize();
	if (res)
		goto end;

	res = MainLoop();
	if (res)
		goto end;

	res = Terminate();
	if (res)
		goto end;
	
end:
    return NULL;
}
