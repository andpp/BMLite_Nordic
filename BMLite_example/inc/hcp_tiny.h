#ifndef HCP_H
#define HCP_H

#include "fpc_bep_types.h"
#include "fpc_hcp_common.h"

/** MTU for HCP physical layer */
#define MTU 256

/** Communication acknowledge definition */
#define FPC_BEP_ACK 0x7f01ff7f

typedef struct {
    uint32_t size;
    uint8_t *data;
} HCP_arg_t;

typedef struct {
    /** Send data to BM-Lite */
    fpc_bep_result_t (*write) (uint16_t, const uint8_t *, uint32_t, void *);  
    /** Receive data from BM-Lite */
    fpc_bep_result_t (*read)(uint16_t, uint8_t *, uint32_t, void *);
    /** Receive timeout (msec). Applys ONLY to receiving packet from BM-Lite on physical layer */
    uint32_t phy_rx_timeout;
    /** Data buffer for application layer */
    uint8_t *pkt_buffer;
    /** Size of data buffer */
    uint32_t pkt_size_max;
    /** Current size of incoming or outcoming command packet */
    uint32_t pkt_size;
    /** Buffer of MTU size for transport layer */
    uint8_t *txrx_buffer;
    /** Values of last argument pulled by bmlite_get_arg 
        Values are valid only right after bmlite_get_arg() call */
    HCP_arg_t arg;
    /** Result of execution command on BM-Lite */
    fpc_bep_result_t bep_result;
} HCP_comm_t;

/**
 * @brief Helper function for receiving HCP commands

 * @param command_id command to send
 * @param arg_key1 first key to receive
 * @param arg_data1 first argument data
 * @param arg_data1_length first argument data length
 * @param arg_key2 second key to receive
 * @param arg_data2 second argument data
 * @param arg_data2_length second argument
 * @return ::fpc_bep_result_t
 */
fpc_bep_result_t bmlite_receive(HCP_comm_t *hcp_comm);
fpc_bep_result_t bmlite_send(HCP_comm_t *hcp_comm);
fpc_bep_result_t bmlite_tranceive(HCP_comm_t *hcp_comm);

fpc_bep_result_t bmlite_init_cmd(HCP_comm_t *hcp_comm, uint16_t cmd, uint16_t arg);
fpc_bep_result_t bmlite_add_arg(HCP_comm_t *hcp_comm, uint16_t arg, void *data, uint16_t size);
fpc_bep_result_t bmlite_get_arg(HCP_comm_t *hcp_comm, uint16_t arg_type);
fpc_bep_result_t bmlite_copy_arg(HCP_comm_t *hcp_comm, uint16_t arg_key, void *arg_data, uint16_t arg_data_length);



#endif 