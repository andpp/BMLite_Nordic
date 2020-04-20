/*
 * Copyright (c) 2020 Fingerprint Cards AB
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file    bep_host_if.c
 * @brief   BEP Host Interface implementation.
 */

#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "fpc_bep_types.h"
#include "fpc_hcp_common.h"
#include "fpc_com_result.h"
#include "fpc_hcp.h"
#include "platform.h"
#include "com_common.h"

#include "bep_host_if.h"

/** Returns the number of elements in an array. */
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

#define RECEIVE_TIMEOUT 10

/** Maximum attempts for capture image */
static const uint8_t MAX_CAPTURE_ATTEMPTS = 15U;
static const uint16_t CAPTURE_TIMEOUT = 3000;

/**
 * @brief Mock callback functions
 */
__attribute__((weak)) void bmlite_on_error(bmlite_error_t error, int32_t value) { (void)error; (void)value; }

__attribute__((weak)) void bmlite_on_start_capture() {}
__attribute__((weak)) void bmlite_on_finish_capture() {}

__attribute__((weak)) void bmlite_on_finish_enroll() {}
__attribute__((weak)) void bmlite_on_start_enroll() {}

__attribute__((weak)) void bmlite_on_start_enrollcapture() {}
__attribute__((weak)) void bmlite_on_finish_enrollcapture() {}

__attribute__((weak)) void bmlite_on_identify_start() {}
__attribute__((weak)) void bmlite_on_identify_finish() {}



/**
 * @brief Helper function for sending HCP commands
 *
 * @param chain HCP communication chain
 * @param command_id command to send
 * @param arg_key1 first key to add to the command
 * @param arg_data1 first argument data to add
 * @param arg_data1_length first data length of argument data
 * @param arg_key2 second key to add to the command
 * @param arg_data2 second argument data to add
 * @param arg_data2_length second data length of argument data
 * @return ::fpc_bep_result_t
 */
static fpc_bep_result_t send_command_args2(fpc_com_chain_t *chain, fpc_hcp_cmd_t command_id,
        fpc_hcp_arg_t arg_key1, void *arg_data1, uint16_t arg_data1_length,
        fpc_hcp_arg_t arg_key2, void *arg_data2, uint16_t arg_data2_length)
{
    fpc_hcp_packet_t command;
    fpc_bep_result_t bep_result;
    fpc_com_result_t com_result;
    fpc_hcp_arg_data_t args_tx[10] = {{ 0 }};

    memset(&command, 0x0, sizeof(command));
    command.arguments = args_tx;
    command.num_args = ARRAY_SIZE(args_tx);
    command.id = command_id;

    if (arg_key1 != ARG_NONE) {
        if (!fpc_hcp_arg_add(&command, arg_key1, arg_data1_length, false, arg_data1)) {
            bep_result = FPC_BEP_RESULT_NO_MEMORY;
            goto exit;
        }
    }

    if (arg_key2 != ARG_NONE) {
        if (!fpc_hcp_arg_add(&command, arg_key2, arg_data2_length, false, arg_data2)) {
            bep_result = FPC_BEP_RESULT_NO_MEMORY;
            goto exit;
        }
    }

    com_result = fpc_hcp_transmit(&command, chain);
    bep_result = com_to_bep_result(com_result);

    if (bep_result != FPC_BEP_RESULT_OK) {
        bmlite_on_error(BMLITE_ERROR_SEND_CMD, bep_result);
    }

exit:
    fpc_hcp_free(chain, &command);

    return bep_result;
}

static fpc_bep_result_t send_command_no_args(fpc_com_chain_t *chain, fpc_hcp_cmd_t command_id)
{
    return send_command_args2(chain, command_id, ARG_NONE, NULL, 0, ARG_NONE, NULL, 0);
}

static fpc_bep_result_t send_command(fpc_com_chain_t *chain, fpc_hcp_cmd_t command_id,
        fpc_hcp_arg_t arg_key, void *arg_data, uint16_t arg_data_length)
{
    return send_command_args2(chain, command_id, arg_key, arg_data, arg_data_length,
        ARG_NONE, NULL, 0);
}

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
static fpc_bep_result_t receive_result_args2(fpc_com_chain_t *chain,
        fpc_hcp_arg_t arg_key1, void *arg_data1, uint16_t arg_data1_length,
        fpc_hcp_arg_t arg_key2, void *arg_data2, uint16_t arg_data2_length)
{
    fpc_hcp_packet_t response;
    fpc_hcp_arg_data_t args_rx[10] = {{ 0 }};
    fpc_bep_result_t bep_result = FPC_BEP_RESULT_GENERAL_ERROR;
    fpc_hcp_arg_data_t *arg_data;

    memset(&response, 0x0, sizeof(fpc_hcp_cmd_t));
    response.arguments = args_rx;
    response.num_args = ARRAY_SIZE(args_rx);

    do {
        fpc_com_result_t com_result = fpc_hcp_receive(&response, chain);
        bep_result = com_to_bep_result(com_result);
    } while (bep_result == FPC_BEP_RESULT_TIMEOUT);

    if (bep_result != FPC_BEP_RESULT_OK) {
        goto exit;
    }

    /* Check bep result first */
    arg_data = fpc_hcp_arg_get(&response, ARG_RESULT);
    if (arg_data) {
        bep_result = *(int8_t *)arg_data->data;
    } else {
        bep_result = FPC_BEP_RESULT_INVALID_ARGUMENT;
    }
    if (bep_result != FPC_BEP_RESULT_OK) {
        goto exit;
    }

    /* Get first argument */
    if (arg_key1 != ARG_NONE) {
        arg_data = fpc_hcp_arg_get(&response, arg_key1);
        if (arg_data && arg_data->size <= arg_data1_length) {
            memcpy(arg_data1, arg_data->data, arg_data->size);
        } else {
            bep_result = FPC_BEP_RESULT_INVALID_ARGUMENT;
            goto exit;
        }
    }

    /* Get second argument */
    if (arg_key2 != ARG_NONE) {
        arg_data = fpc_hcp_arg_get(&response, arg_key2);
        if (arg_data && arg_data->size <= arg_data2_length) {
            memcpy(arg_data2, arg_data->data, arg_data->size);
        } 
    }

exit:
    fpc_hcp_free(chain, &response);

    return bep_result;
}

static fpc_bep_result_t receive_result_no_args(fpc_com_chain_t *chain)
{
    return receive_result_args2(chain, ARG_NONE, NULL, 0, ARG_NONE, NULL, 0);
}

static fpc_bep_result_t receive_result_args1(fpc_com_chain_t *chain,
        fpc_hcp_arg_t arg_key, void *arg_data, uint16_t arg_data_length)
{
    return receive_result_args2(chain, arg_key, arg_data, arg_data_length, ARG_NONE, NULL, 0);
}


fpc_bep_result_t bep_capture(fpc_com_chain_t *chain, uint16_t timeout)
{
    fpc_bep_result_t bep_result;

    /* Capture finger down */
    bep_result = send_command(chain, CMD_CAPTURE, ARG_TIMEOUT, &timeout, sizeof(timeout));
    if (bep_result != FPC_BEP_RESULT_OK) {
        bmlite_on_error(BMLITE_ERROR_CAPTURE_START, bep_result);
        return bep_result;
    }

    bmlite_on_start_capture();
    bep_result = receive_result_no_args(chain);
    bmlite_on_finish_capture();

   return bep_result;
}

fpc_bep_result_t bep_enroll_finger(fpc_com_chain_t *chain)
{
    uint32_t samples_remaining = 0;
    fpc_bep_result_t bep_result = FPC_BEP_RESULT_OK;
    bool enroll_done = false;

    bmlite_on_start_enroll();

    /* Enroll start */
    bep_result = send_command(chain, CMD_ENROLL, ARG_START, NULL, 0);
    if (bep_result != FPC_BEP_RESULT_OK) {
        bmlite_on_error(BMLITE_ERROR_ENROLL_START, bep_result);
        goto exit;
    }

    bep_result = receive_result_no_args(chain);
    if (bep_result != FPC_BEP_RESULT_OK) {
        bmlite_on_error(BMLITE_ERROR_WRONG_ANSWER, bep_result);
        goto exit;
    }

    for (uint8_t i = 0; i < MAX_CAPTURE_ATTEMPTS; ++i) {

        bmlite_on_start_enrollcapture();
        bep_result = bep_capture(chain, CAPTURE_TIMEOUT);
        bmlite_on_finish_enrollcapture();

        if (bep_result != FPC_BEP_RESULT_OK) {
            bmlite_on_error(BMLITE_ERROR_CAPTURE, bep_result);
            break;
        }

        /* Enroll add */
        bep_result = send_command(chain, CMD_ENROLL, ARG_ADD, NULL, 0);
        if (bep_result != FPC_BEP_RESULT_OK) {
            bmlite_on_error(BMLITE_ERROR_ENROLL_ADD, bep_result);
            continue;
        }

        bep_result = receive_result_args1(chain, ARG_COUNT, &samples_remaining,
            sizeof(samples_remaining));
        if (bep_result != FPC_BEP_RESULT_OK) {
            bmlite_on_error(BMLITE_ERROR_WRONG_ANSWER, bep_result);
            continue;
        }

        if (samples_remaining == 0U) {
            enroll_done = true;
            break;
        }

        bep_result = send_command(chain, CMD_WAIT, ARG_FINGER_UP, NULL, 0);
        if (bep_result != FPC_BEP_RESULT_OK) {
            bmlite_on_error(BMLITE_ERROR_FINGER_WAIT, bep_result);
            continue;
        }

        /* Wait for finger to be lifted from sensor */
        bep_result = receive_result_no_args(chain);
        if (bep_result != FPC_BEP_RESULT_OK) {
            bmlite_on_error(BMLITE_ERROR_WRONG_ANSWER, bep_result);
            continue;
        }
    }

    bep_result = send_command(chain, CMD_ENROLL, ARG_FINISH, NULL, 0);
    if (bep_result == FPC_BEP_RESULT_OK) {
        bep_result = receive_result_no_args(chain);
        if (bep_result != FPC_BEP_RESULT_OK) {
            bmlite_on_error(BMLITE_ERROR_ENROLL_FINISH, bep_result);
        }
    }

exit:
    bmlite_on_finish_enroll();
    return (!enroll_done) ? FPC_BEP_RESULT_GENERAL_ERROR : bep_result;
}

fpc_bep_result_t bep_identify_finger(fpc_com_chain_t *chain, uint16_t *template_id, bool *match)
{
    fpc_bep_result_t bep_result = FPC_BEP_RESULT_OK;

    *match = false;

    bmlite_on_identify_start();
    bep_result = bep_capture(chain, CAPTURE_TIMEOUT);
    if (bep_result != FPC_BEP_RESULT_OK) {
            bmlite_on_error(BMLITE_ERROR_CAPTURE, bep_result);
        goto exit;
    }

    bep_result = bep_image_extract(chain);
    if (bep_result != FPC_BEP_RESULT_OK) {
        goto exit;
    }

    bep_result = send_command(chain, CMD_IDENTIFY, ARG_NONE, NULL, 0);
    if (bep_result != FPC_BEP_RESULT_OK) {
        bmlite_on_error(BMLITE_ERROR_IDENTYFY, bep_result);
        goto exit;
    }

    bep_result = receive_result_args2(chain, ARG_MATCH, match, sizeof(bool),
        ARG_ID, template_id, sizeof(uint16_t));
    if (bep_result != FPC_BEP_RESULT_OK) {
        bmlite_on_error(BMLITE_ERROR_WRONG_ANSWER, bep_result);
        goto exit;
    }

exit:
    bmlite_on_identify_finish();
    return bep_result;
}

fpc_bep_result_t bep_save_template(fpc_com_chain_t *chain, uint16_t template_id)
{
    fpc_bep_result_t bep_result = FPC_BEP_RESULT_OK;

    bep_result = send_command_args2(chain, CMD_TEMPLATE, ARG_SAVE, NULL, 0, ARG_ID, &template_id,
        sizeof(template_id));
    if (bep_result != FPC_BEP_RESULT_OK) {
        bmlite_on_error(BMLITE_ERROR_TEMPLATE_SAVE, bep_result);
        return bep_result;
    }

    return receive_result_no_args(chain);
}

fpc_bep_result_t bep_delete_template(fpc_com_chain_t *chain, uint16_t template_id)
{
    fpc_bep_result_t bep_result = FPC_BEP_RESULT_OK;

    if (template_id == REMOVE_ID_ALL_TEMPLATES) {
        bep_result = send_command_args2(chain, CMD_STORAGE_TEMPLATE, ARG_DELETE, NULL, 0,
            ARG_ALL, NULL, 0);
    } else {
        bep_result = send_command_args2(chain, CMD_STORAGE_TEMPLATE, ARG_DELETE, NULL, 0,
            ARG_ID, &template_id, sizeof(template_id));
    }
    if (bep_result != FPC_BEP_RESULT_OK) {
        bmlite_on_error(BMLITE_ERROR_TEMPLATE_DELETE, bep_result);
        return bep_result;
    }

    return receive_result_no_args(chain);
}

fpc_bep_result_t bep_get_template_count(fpc_com_chain_t *chain, uint32_t *template_count)
{
    fpc_bep_result_t bep_result = FPC_BEP_RESULT_OK;

    bep_result = send_command(chain, CMD_STORAGE_TEMPLATE, ARG_COUNT, NULL, 0);
    if (bep_result != FPC_BEP_RESULT_OK) {
        bmlite_on_error(BMLITE_ERROR_TEMPLATE_COUNT, bep_result);
        return bep_result;
    }

    bep_result = receive_result_args1(chain, ARG_COUNT, template_count, sizeof(template_count[0]));
    if (bep_result != FPC_BEP_RESULT_OK) {
        bmlite_on_error(BMLITE_ERROR_WRONG_ANSWER, bep_result);
        return bep_result;
    }

    return bep_result;
}

fpc_bep_result_t bep_get_template_ids(fpc_com_chain_t *chain, uint16_t *template_ids,
    uint32_t nof_templates)
{
    fpc_bep_result_t bep_result = FPC_BEP_RESULT_OK;

    bep_result = send_command(chain, CMD_STORAGE_TEMPLATE, ARG_ID, NULL, 0);
    if (bep_result != FPC_BEP_RESULT_OK) {
        bmlite_on_error(BMLITE_ERROR_TEMPLATE_GETIDS, bep_result);
        return bep_result;
    }

    bep_result = receive_result_args1(chain, ARG_DATA, template_ids, nof_templates *
            sizeof(template_ids[0]));
    if (bep_result != FPC_BEP_RESULT_OK) {
        bmlite_on_error(BMLITE_ERROR_WRONG_ANSWER, bep_result);

        return bep_result;
    }

    return bep_result;
}

fpc_bep_result_t bep_image_extract(fpc_com_chain_t *chain)
{
    fpc_bep_result_t bep_result;

    bep_result = send_command(chain, CMD_IMAGE, ARG_EXTRACT, NULL, 0);
    if (bep_result != FPC_BEP_RESULT_OK) {
        bmlite_on_error(BMLITE_ERROR_IMAGE_EXTRACT, bep_result);
        return bep_result;
    }

    return receive_result_no_args(chain);
}

fpc_bep_result_t bep_image_get_size(fpc_com_chain_t *chain, uint32_t *size)
{
    fpc_bep_result_t bep_result;

    bep_result = send_command(chain, CMD_IMAGE, ARG_SIZE, NULL, 0);
    if (bep_result != FPC_BEP_RESULT_OK) {
        bmlite_on_error(BMLITE_ERROR_IMAGE_GETSIZE, bep_result);
        return bep_result;
    }

    return receive_result_args1(chain, ARG_SIZE, size, sizeof(size));
}

fpc_bep_result_t bep_image_get(fpc_com_chain_t *chain, uint8_t *data, uint32_t size)
{
    fpc_bep_result_t bep_result;

    bep_result = send_command(chain, CMD_IMAGE, ARG_UPLOAD, NULL, 0);
    if (bep_result != FPC_BEP_RESULT_OK) {
        bmlite_on_error(BMLITE_ERROR_IMAGE_GET, bep_result);
        return bep_result;
    }

    return receive_result_args1(chain, ARG_DATA, data, size);
}

fpc_bep_result_t bep_version(fpc_com_chain_t *chain, char *version, int len)
{
    fpc_bep_result_t bep_result;

    bep_result = send_command_args2(chain, CMD_INFO, ARG_GET, NULL, 0, ARG_VERSION, NULL, 0);
    if (bep_result != FPC_BEP_RESULT_OK) {
        bmlite_on_error(BMLITE_ERROR_GETVERSION, bep_result);
        return bep_result;
    }

    return receive_result_args1(chain, ARG_VERSION, version, len);
}

fpc_bep_result_t bep_reset(fpc_com_chain_t *chain)
{
    fpc_bep_result_t bep_result;

    bep_result = send_command_no_args(chain, CMD_RESET);
    if (bep_result != FPC_BEP_RESULT_OK) {
        bmlite_on_error(BMLITE_ERROR_SW_RESET, bep_result);
        return bep_result;
    }

    return receive_result_no_args(chain);
}

fpc_bep_result_t bep_sensor_calibrate(fpc_com_chain_t *chain)
{
    fpc_bep_result_t bep_result;

    bep_result = send_command_no_args(chain, CMD_STORAGE_CALIBRATION);
    if (bep_result != FPC_BEP_RESULT_OK) {
        bmlite_on_error(BMLITE_ERROR_CALIBRATE, bep_result);
        return bep_result;
    }

    return receive_result_no_args(chain);
}

fpc_bep_result_t bep_sensor_calibrate_remove(fpc_com_chain_t *chain)
{
    fpc_bep_result_t bep_result;

    bep_result = send_command(chain, CMD_STORAGE_CALIBRATION, ARG_DELETE, NULL, 0);
    if (bep_result != FPC_BEP_RESULT_OK) {
        bmlite_on_error(BMLITE_ERROR_CALIBRATE_DELETE, bep_result);
        return bep_result;
    }

    return receive_result_no_args(chain);
}

fpc_bep_result_t bep_sensor_wait_for_finger(fpc_com_chain_t *chain, uint16_t timeout)
{
    fpc_bep_result_t bep_result;

    bep_result = send_command(chain, CMD_WAIT, ARG_FINGER_DOWN, NULL, 0);
    if (bep_result != FPC_BEP_RESULT_OK) {
        bmlite_on_error(BMLITE_ERROR_FINGER_WAIT, bep_result);
        return bep_result;
    }

    /* Wait for finger on sensor */
    return receive_result_no_args(chain);
}

fpc_bep_result_t bep_sensor_wait_finger_not_present(fpc_com_chain_t *chain, uint16_t timeout)
{
    fpc_bep_result_t bep_result;

    bep_result = send_command(chain, CMD_WAIT, ARG_FINGER_UP, NULL, 0);
    if (bep_result != FPC_BEP_RESULT_OK) {
        bmlite_on_error(BMLITE_ERROR_FINGER_WAIT, bep_result);
        return bep_result;
    }

    /* Wait for finger to be lifted from sensor */
    return receive_result_no_args(chain);
}
