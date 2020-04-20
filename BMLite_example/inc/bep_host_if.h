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

#ifndef BEP_HOST_IF_H
#define BEP_HOST_IF_H

/**
 * @file    bep_host_if.h
 * @brief   BEP Host Interface API
 */

#include <stdint.h>
#include <stdbool.h>
#include "fpc_bep_types.h"
#include "fpc_com_chain.h"

#define REMOVE_ID_ALL_TEMPLATES 0U


typedef enum {
    BMLITE_ERROR_OK = 0,
    BMLITE_ERROR_CAPTURE,
    BMLITE_ERROR_CAPTURE_START,
    BMLITE_ERROR_ENROLL_START,
    BMLITE_ERROR_ENROLL_ADD,
    BMLITE_ERROR_ENROLL_FINISH,
    BMLITE_ERROR_WRONG_ANSWER,
    BMLITE_ERROR_FINGER_WAIT,
    BMLITE_ERROR_IDENTYFY,
    BMLITE_ERROR_TEMPLATE_SAVE,
    BMLITE_ERROR_TEMPLATE_DELETE,
    BMLITE_ERROR_TEMPLATE_COUNT,
    BMLITE_ERROR_TEMPLATE_GETIDS,
    BMLITE_ERROR_IMAGE_EXTRACT,
    BMLITE_ERROR_IMAGE_GETSIZE,
    BMLITE_ERROR_IMAGE_GET,
    BMLITE_ERROR_GETVERSION,
    BMLITE_ERROR_SW_RESET,
    BMLITE_ERROR_CALIBRATE,
    BMLITE_ERROR_CALIBRATE_DELETE,
    BMLITE_ERROR_SEND_CMD
} bmlite_error_t;

/**
 * @brief Error Callback function
 *
 * @param[in] Callback Error Code
 * @param[in] BEP result code
 */
void bmlite_on_error(bmlite_error_t error, int32_t value);

/**
 * @brief Starting Capture Callback function
 */
void bmlite_on_start_capture();

/**
 * @brief Finishing Capture Callback function
 */
void bmlite_on_finish_capture();

/**
 * @brief Starting Enroll Callback function
 */
void bmlite_on_start_enroll();

/**
 * @brief Finishing Enroll Callback function
 */
void bmlite_on_finish_enroll();

/**
 * @brief Starting Capture for Enroll Callback function
 */
void bmlite_on_start_enrollcapture();

/**
 * @brief Finishing Capture for Enroll Callback function
 */
void bmlite_on_finish_enrollcapture();

/**
 * @brief Starting Identify Callback function
 */
void bmlite_on_identify_start();

/**
 * @brief Finishing Identify Callback function
 */
void bmlite_on_identify_finish();


/**
 * @brief Sends HCP commands for capturing an image in Bio MCU
 *
 * @param[in] chain HCP com chain
 * @param[in] timeout Timeout in ms
 * @return ::fpc_bep_result_t
 */
fpc_bep_result_t bep_capture(fpc_com_chain_t *chain, uint16_t timeout);

/**
 * @brief Sends HCP commands for enrolling a finger in Bio MCU
 *
 * @param[in] chain HCP com chain
 * @return ::fpc_bep_result_t
 */
fpc_bep_result_t bep_enroll_finger(fpc_com_chain_t *chain);

/**
 * @brief Sends HCP commands for identifying a finger in Bio MCU
 *
 * @param[in] chain HCP com chain
 * @param[out] template_id Template id that was identified (only valid if match is true)
 * @param[out] match True if match
 * @return ::fpc_bep_result_t
 */
fpc_bep_result_t bep_identify_finger(fpc_com_chain_t *chain, uint16_t *template_id, bool *match);


/**
 * @brief Sends HCP commands for saving template of an enrolled finger in Bio MCU
 *
 * @param[in] chain HCP com chain
 * @param[in] template_id Template id to save
 * @return ::fpc_bep_result_t
 */
fpc_bep_result_t bep_save_template(fpc_com_chain_t *chain, uint16_t template_id);

/**
 * @brief Remove template(s) stored
 *
 * @param[in] chain HCP com chain
 * @param[in] template_id template id to remove, if =REMOVE_ID_ALL_TEMPLATES removes all templates
 * @return ::fpc_bep_result_t
 */
fpc_bep_result_t bep_delete_template(fpc_com_chain_t *chain, uint16_t template_id);

/**
 * @brief Gets all template ids from Bio MCU
 *
 * @param[in] chain HCP com chain
 * @param[out] template_ids Should be allocated by the caller for storing template ids and should
 *                          have capacity to hold bio_get_template_count
 * @param[in] nof_templates This should be acquired using bio_get_template_count
 * @return ::fpc_bep_result_t
 */
fpc_bep_result_t bep_get_template_ids(fpc_com_chain_t *chain, uint16_t *template_ids,
        uint32_t nof_templates);

/**
 * @brief Get the number of templates in Bio MCU
 *
 * @param[in] chain HCP com chain
 * @param[out] template_count
 * @return ::fpc_bep_result_t
 */
fpc_bep_result_t bep_get_template_count(fpc_com_chain_t *chain, uint32_t *template_count);


/**
 * @brief Sends HCP commands for extracting a template from a previously capture image in Bio MCU
 *
 * @param[in] chain HCP com chain
 * @return ::fpc_bep_result_t
 */
fpc_bep_result_t bep_image_extract(fpc_com_chain_t *chain);

/**
 * @brief Sends HCP commands for getting the image size of a previously capture image in Bio MCU
 *
 * @param[in] chain HCP com chain
 * @return ::fpc_bep_result_t
 */
fpc_bep_result_t bep_image_get_size(fpc_com_chain_t *chain, uint32_t *size);

/**
 * @brief Sends HCP commands for getting the image data of a previously capture image in Bio MCU
 *
 * @param[in] chain HCP com chain
 * @return ::fpc_bep_result_t
 */
fpc_bep_result_t bep_image_get(fpc_com_chain_t *chain, uint8_t *data, uint32_t size);

/**
 * @brief Sends HCP commands for fetching the version of the Bio MCU FW
 *
 * @param[in] chain HCP com chain
 * @param[out] version Version string buffer
 * @param[out] len Length of version string buffer
 * @return ::fpc_bep_result_t
 */
fpc_bep_result_t bep_version(fpc_com_chain_t *chain, char *version, int len);

/**
 * @brief Sends HCP commands for resetting the Bio MCU
 *
 * @param[in] chain HCP com chain
 * @return ::fpc_bep_result_t
 */
fpc_bep_result_t bep_reset(fpc_com_chain_t *chain);

/**
 * @brief Sends HCP commands for calibrating the sensor in the Bio MCU
 *
 * @param[in] chain HCP com chain
 * @return ::fpc_bep_result_t
 */
fpc_bep_result_t bep_sensor_calibrate(fpc_com_chain_t *chain);

/**
 * @brief Sends HCP commands for removing the sensor calibration in the Bio MCU
 *
 * @param[in] chain HCP com chain
 * @return ::fpc_bep_result_t
 */
fpc_bep_result_t bep_sensor_calibrate_remove(fpc_com_chain_t *chain);

/**
 * @brief Sends HCP commands for setting the sensor in sleep and waiting for finger in the Bio MCU
 *
 * @param[in] chain HCP com chain
 * @param[in] timeout Timeout in ms
 * @return ::fpc_bep_result_t
 */
fpc_bep_result_t bep_sensor_wait_for_finger(fpc_com_chain_t *chain, uint16_t timeout);

/**
 * @brief Sends HCP commands that returns when finger is not on the sensor in the Bio MCU
 *
 * @param[in] chain HCP com chain
 * @param[in] timeout Timeout in ms
 * @return ::fpc_bep_result_t
 */
fpc_bep_result_t bep_sensor_wait_finger_not_present(fpc_com_chain_t *chain, uint16_t timeout);

#endif /* BEP_HOST_IF_H */
