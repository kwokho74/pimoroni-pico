#include "py/obj.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "py/misc.h"
#include "bsec_datatypes.h"
#include "bsec_interface.h"
#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"
#include <stdio.h>

// Function to list and print files in the root directory
STATIC mp_obj_t list_and_print_files(void) {
    // Access the root directory
    const char *path = "/";
    mp_vfs_mount_t *vfs = MP_STATE_VM(vfs_mount_table);
    
    // Ensure there is a mounted filesystem
    if (vfs == NULL) {
        mp_raise_msg(&mp_type_OSError, "No filesystem mounted");
    }

    // Open the root directory
    FILINFO fno;
    DIR dir;
    FRESULT res;

    res = f_opendir(&vfs->fatfs, &dir, path);
    if (res != FR_OK) {
        mp_raise_OSError(MP_EIO);
    }

    // List and print files in the directory
    for (;;) {
        res = f_readdir(&dir, &fno);
        if (res != FR_OK || fno.fname[0] == 0) break;  // Break on error or end of dir
        if (fno.fname[0] == '.') continue;             // Ignore dot entry

        // Print file name using MicroPython's printing function
        mp_printf(&mp_plat_print, "%s\n", fno.fname);
    }

    f_closedir(&dir);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(list_and_print_files_obj, list_and_print_files);

// Combine high and low 32-bit integers into a 64-bit integer
static int64_t from_uint_high_low(uint32_t high, uint32_t low) {
    return ((int64_t)high << 32) | low;
}

// Extract the high 32-bit part of a 64-bit integer
static uint32_t to_uint_high(int64_t value) {
    return (uint32_t)(value >> 32) & 0xFFFFFFFF;
}

// Extract the low 32-bit part of a 64-bit integer
static uint32_t to_uint_low(int64_t value) {
    return (uint32_t)value & 0xFFFFFFFF;
}

// Create a function to list the file names in the root directory of the raspberry pi pico
static mp_obj_t list_files(mp_obj_t path) {
    // open a file for reading
    mp_obj_t file = mp_vfs_open(path, "r");
    if (file == MP_OBJ_NULL) {
        mp_raise_OSError(MP_ENOENT);
    }

    // read the file
    vstr_t vstr;
    vstr_init(&vstr, 32);
    while (1) {
        char c = mp_vfs_read_byte(file);
        if (c == MP_READER_EOF) {
            break;
        }
        vstr_add_byte(&vstr, c);
    }

    // close the file
    mp_vfs_close(file);

    // return the file contents as a string
    return mp_obj_new_str_from_vstr(&mp_type_str, &vstr);
}



// Helper function to create a result tuple with a value
static mp_obj_t to_result_tuple(bsec_library_return_t result, mp_obj_t value) {
    mp_obj_t result_tuple[2];
    result_tuple[0] = mp_obj_new_int(result);
    result_tuple[1] = value;
    return mp_obj_new_tuple(2, result_tuple);
}

// Helper function to create a result tuple with None
static mp_obj_t to_result_tuple_none(bsec_library_return_t result) {
    return to_result_tuple(result, mp_const_none);
}

// Helper function to convert a tuple to bsec_sensor_configuration_t
static void tuple_to_sensor_configuration(mp_obj_t tuple, bsec_sensor_configuration_t *config) {
    mp_obj_t *items;
    size_t len;
    mp_obj_get_array(tuple, &len, &items);
    if (len != 2) {
        mp_raise_ValueError("Tuple length must be 2");
    }

    config->sample_rate = mp_obj_get_float(items[0]);
    config->sensor_id = mp_obj_get_int(items[1]);
}

// Helper function to convert bsec_sensor_configuration_t to tuple
static mp_obj_t sensor_configuration_to_tuple(const bsec_sensor_configuration_t *config) {
    mp_obj_t tuple[2];
    tuple[0] = mp_obj_new_float(config->sample_rate);
    tuple[1] = mp_obj_new_int(config->sensor_id);
    return mp_obj_new_tuple(2, tuple);
}

// Helper function to convert bsec_bme_settings_t to dictionary
static mp_obj_t sensor_settings_to_dict(const bsec_bme_settings_t *settings) {
    mp_obj_t dict = mp_obj_new_dict(0);
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_next_call_high), mp_obj_new_int_from_uint(to_uint_high(settings->next_call)));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_next_call_low), mp_obj_new_int_from_uint(to_uint_low(settings->next_call)));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_process_data), mp_obj_new_int(settings->process_data));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_heater_temperature), mp_obj_new_int(settings->heater_temperature));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_heater_duration), mp_obj_new_int(settings->heater_duration));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_trigger_measurement), mp_obj_new_int(settings->trigger_measurement));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_op_mode), mp_obj_new_int(settings->op_mode));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_run_gas), mp_obj_new_int(settings->run_gas));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_pressure_oversampling), mp_obj_new_int(settings->pressure_oversampling));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_temperature_oversampling), mp_obj_new_int(settings->temperature_oversampling));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_humidity_oversampling), mp_obj_new_int(settings->humidity_oversampling));
    return dict;
}

// Wrapper for bsec_get_version
static mp_obj_t bsec_get_version_wrapper(void) {
    bsec_version_t version;
    bsec_library_return_t result = bsec_get_version(&version);
    if (result != BSEC_OK) {
        return to_result_tuple_none(result);
    }

    mp_obj_t version_tuple[4];
    version_tuple[0] = mp_obj_new_int(version.major);
    version_tuple[1] = mp_obj_new_int(version.minor);
    version_tuple[2] = mp_obj_new_int(version.major_bugfix);
    version_tuple[3] = mp_obj_new_int(version.minor_bugfix);
    return to_result_tuple(result, mp_obj_new_tuple(4, version_tuple));
}
static MP_DEFINE_CONST_FUN_OBJ_0(bsec_get_version_obj, bsec_get_version_wrapper);

// Wrapper for bsec_init
static mp_obj_t bsec_init_wrapper(void) {
    bsec_library_return_t result = bsec_init();
    return mp_obj_new_int(result);
}
static MP_DEFINE_CONST_FUN_OBJ_0(bsec_init_obj, bsec_init_wrapper);

// Wrapper for bsec_set_configuration
static mp_obj_t bsec_set_configuration_wrapper(mp_obj_t config_obj) {
    mp_buffer_info_t config_bufinfo;
    mp_get_buffer_raise(config_obj, &config_bufinfo, MP_BUFFER_READ);

    if (config_bufinfo.len > BSEC_MAX_PROPERTY_BLOB_SIZE) {
        mp_raise_ValueError("Configuration size too large");
    }

    uint8_t work_buffer[BSEC_MAX_WORKBUFFER_SIZE];
    bsec_library_return_t result = bsec_set_configuration((const uint8_t *)config_bufinfo.buf, config_bufinfo.len, work_buffer, BSEC_MAX_WORKBUFFER_SIZE);
    return mp_obj_new_int(result);
}
static MP_DEFINE_CONST_FUN_OBJ_1(bsec_set_configuration_obj, bsec_set_configuration_wrapper);

// Wrapper for bsec_get_state
static mp_obj_t bsec_get_state_wrapper(void) {
    uint8_t state_buffer[BSEC_MAX_STATE_BLOB_SIZE];
    uint8_t work_buffer[BSEC_MAX_WORKBUFFER_SIZE];
    uint32_t n_serialized_state;
    bsec_library_return_t result = bsec_get_state(0, state_buffer, BSEC_MAX_STATE_BLOB_SIZE, work_buffer, BSEC_MAX_WORKBUFFER_SIZE, &n_serialized_state);
    if (result != BSEC_OK) {
        return to_result_tuple_none(result);
    }

    return to_result_tuple(result, mp_obj_new_bytes(state_buffer, n_serialized_state));
}
static MP_DEFINE_CONST_FUN_OBJ_0(bsec_get_state_obj, bsec_get_state_wrapper);

// Wrapper for bsec_set_state
static mp_obj_t bsec_set_state_wrapper(mp_obj_t state_obj) {
    mp_buffer_info_t state_bufinfo;
    mp_get_buffer_raise(state_obj, &state_bufinfo, MP_BUFFER_READ);

    if (state_bufinfo.len > BSEC_MAX_STATE_BLOB_SIZE) {
        mp_raise_ValueError("State size too large");
    }

    uint8_t work_buffer[BSEC_MAX_WORKBUFFER_SIZE];
    bsec_library_return_t result = bsec_set_state((const uint8_t *)state_bufinfo.buf, state_bufinfo.len, work_buffer, BSEC_MAX_WORKBUFFER_SIZE);
    return mp_obj_new_int(result);
}
static MP_DEFINE_CONST_FUN_OBJ_1(bsec_set_state_obj, bsec_set_state_wrapper);

// Wrapper for bsec_update_subscription
static mp_obj_t bsec_update_subscription_wrapper(mp_obj_t requested_virtual_sensors_tuple) {
    mp_obj_t *requested_virtual_sensors;
    size_t n_requested_virtual_sensors;

    // Get the tuples
    mp_obj_get_array(requested_virtual_sensors_tuple, &n_requested_virtual_sensors, &requested_virtual_sensors);

    // open a file for reading
    
    
    

    bsec_sensor_configuration_t requested_virtual_sensors_configs[n_requested_virtual_sensors];
    bsec_sensor_configuration_t required_sensor_settings_configs[BSEC_MAX_PHYSICAL_SENSOR];

    // Convert requested virtual sensors tuples to bsec_sensor_configuration_t
    for (size_t i = 0; i < n_requested_virtual_sensors; i++) {
        tuple_to_sensor_configuration(requested_virtual_sensors[i], &requested_virtual_sensors_configs[i]);
    }

    uint8_t n_required_sensor_settings = BSEC_MAX_PHYSICAL_SENSOR;

    bsec_library_return_t result = bsec_update_subscription(
        requested_virtual_sensors_configs,
        n_requested_virtual_sensors,
        required_sensor_settings_configs,
        &n_required_sensor_settings
    );

    if (result != BSEC_OK) {
        return to_result_tuple_none(result);
    }

    // Convert required_sensor_settings_configs to tuples
    mp_obj_t required_sensor_settings_tuple = mp_obj_new_list(n_required_sensor_settings, NULL);
    for (size_t i = 0; i < n_required_sensor_settings; i++) {
        mp_obj_list_store(MP_OBJ_FROM_PTR(required_sensor_settings_tuple), MP_OBJ_NEW_SMALL_INT(i), sensor_configuration_to_tuple(&required_sensor_settings_configs[i]));
    }

    return to_result_tuple(result, required_sensor_settings_tuple);
}
static MP_DEFINE_CONST_FUN_OBJ_1(bsec_update_subscription_obj, bsec_update_subscription_wrapper);

// Wrapper for bsec_sensor_control
static mp_obj_t bsec_sensor_control_wrapper(mp_obj_t time_stamp_high_obj, mp_obj_t time_stamp_low_obj) {
    uint32_t time_stamp_high = mp_obj_get_int_truncated(time_stamp_high_obj);
    uint32_t time_stamp_low = mp_obj_get_int_truncated(time_stamp_low_obj);

    bsec_bme_settings_t sensor_settings;
    bsec_library_return_t result = bsec_sensor_control(from_uint_high_low(time_stamp_high, time_stamp_low), &sensor_settings);
    if (result != BSEC_OK) {
        return to_result_tuple_none(result);
    }

    return to_result_tuple(result, sensor_settings_to_dict(&sensor_settings));
}
static MP_DEFINE_CONST_FUN_OBJ_2(bsec_sensor_control_obj, bsec_sensor_control_wrapper);

// Wrapper for bsec_do_steps
static mp_obj_t bsec_do_steps_wrapper(mp_obj_t inputs_tuple) {
    mp_obj_t *inputs;
    size_t n_inputs;
    
    // Get the tuples
    mp_obj_get_array(inputs_tuple, &n_inputs, &inputs);
    
    bsec_input_t bsec_inputs[n_inputs];
    for (size_t i = 0; i < n_inputs; i++) {
        mp_obj_t *items;
        size_t len;
        mp_obj_get_array(inputs[i], &len, &items);
        if (len != 4) {
            mp_raise_ValueError("Input tuple length must be 4");
        }

        bsec_inputs[i].sensor_id = mp_obj_get_int(items[0]);
        bsec_inputs[i].signal = mp_obj_get_float(items[1]);
        bsec_inputs[i].time_stamp = from_uint_high_low(mp_obj_get_int_truncated(items[2]), mp_obj_get_int_truncated(items[3]));
    }

    bsec_output_t outputs[BSEC_NUMBER_OUTPUTS];
    uint8_t n_outputs = BSEC_NUMBER_OUTPUTS;

    bsec_library_return_t result = bsec_do_steps(bsec_inputs, n_inputs, outputs, &n_outputs);
    if (result != BSEC_OK) {
        return to_result_tuple_none(result);
    }

    // Convert outputs to list of dictionaries
    mp_obj_t outputs_list = mp_obj_new_list(n_outputs, NULL);
    for (size_t i = 0; i < n_outputs; i++) {
        mp_obj_t dict = mp_obj_new_dict(0);
        mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_sensor_id), mp_obj_new_int(outputs[i].sensor_id));
        mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_signal), mp_obj_new_float(outputs[i].signal));
        mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_accuracy), mp_obj_new_int(outputs[i].accuracy));
        mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_time_stamp_high), mp_obj_new_int_from_uint(to_uint_high(outputs[i].time_stamp)));
        mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_time_stamp_low), mp_obj_new_int_from_uint(to_uint_low(outputs[i].time_stamp)));
        mp_obj_list_store(outputs_list, MP_OBJ_NEW_SMALL_INT(i), dict);
    }

    return to_result_tuple(result, outputs_list);
}
static MP_DEFINE_CONST_FUN_OBJ_1(bsec_do_steps_obj, bsec_do_steps_wrapper);

// Define all attributes of the module.
// Table entries are key/value pairs of the attribute name (a string)
// and the MicroPython object reference.
// All identifiers and strings are written as MP_QSTR_xxx and will be
// optimized to word-sized integers by the build system (interned strings).
static const mp_rom_map_elem_t bsec2_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_bsec) },
    { MP_ROM_QSTR(MP_QSTR_get_version), MP_ROM_PTR(&bsec_get_version_obj) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&bsec_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_configuration), MP_ROM_PTR(&bsec_set_configuration_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_state), MP_ROM_PTR(&bsec_get_state_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_state), MP_ROM_PTR(&bsec_set_state_obj) },
    { MP_ROM_QSTR(MP_QSTR_update_subscription), MP_ROM_PTR(&bsec_update_subscription_obj) },
    { MP_ROM_QSTR(MP_QSTR_sensor_control), MP_ROM_PTR(&bsec_sensor_control_obj) },
    { MP_ROM_QSTR(MP_QSTR_do_steps), MP_ROM_PTR(&bsec_do_steps_obj) },
    { MP_ROM_QSTR(MP_QSTR_list_and_print_files), MP_ROM_PTR(&list_and_print_files_obj) },
};

static MP_DEFINE_CONST_DICT(bsec_module_globals, bsec2_module_globals_table);

// Define module object.
const mp_obj_module_t bsec2_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&bsec_module_globals,
};

// Register the module to make it available in Python.
MP_REGISTER_MODULE(MP_QSTR_bsec2, bsec2_module);
