#include "py/obj.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "py/misc.h"
#include "bsec_datatypes.h"
#include "bsec_interface.h"

// Wrapper for bsec_get_version
static mp_obj_t bsec_get_version_wrapper(void) {
    bsec_version_t version;
    bsec_library_return_t result = bsec_get_version(&version);
    if (result != BSEC_OK) {
        mp_raise_msg(&mp_type_RuntimeError, "Calling BSEC library bsec_get_version failed");
    }

    mp_obj_t tuple[4];
    tuple[0] = mp_obj_new_int(version.major);
    tuple[1] = mp_obj_new_int(version.minor);
    tuple[2] = mp_obj_new_int(version.major_bugfix);
    tuple[3] = mp_obj_new_int(version.minor_bugfix);

    return mp_obj_new_tuple(4, tuple);
}
static MP_DEFINE_CONST_FUN_OBJ_0(bsec_get_version_obj, bsec_get_version_wrapper);

// Wrapper for bsec_init
static mp_obj_t bsec_init_wrapper(void) {
    bsec_library_return_t result = bsec_init();
    if (result != BSEC_OK) {
        mp_raise_msg(&mp_type_RuntimeError, "Calling BSEC library bsec_init failed");
    }
    return mp_const_none;
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
    if (result != BSEC_OK) {
        mp_raise_msg(&mp_type_RuntimeError, "Failed to set BSEC configuration");
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(bsec_set_configuration_obj, bsec_set_configuration_wrapper);

// Wrapper for bsec_get_state
static mp_obj_t bsec_get_state_wrapper(void) {
    uint8_t state_buffer[BSEC_MAX_STATE_BLOB_SIZE];
    uint8_t work_buffer[BSEC_MAX_WORKBUFFER_SIZE];
    uint32_t n_serialized_state;

    bsec_library_return_t result = bsec_get_state(0, state_buffer, BSEC_MAX_STATE_BLOB_SIZE, work_buffer, BSEC_MAX_WORKBUFFER_SIZE, &n_serialized_state);
    if (result != BSEC_OK) {
        mp_raise_msg(&mp_type_RuntimeError, "Failed to get BSEC state");
    }

    return mp_obj_new_bytes(state_buffer, n_serialized_state);
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
    if (result != BSEC_OK) {
        mp_raise_msg(&mp_type_RuntimeError, "Failed to set BSEC state");
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(bsec_set_state_obj, bsec_set_state_wrapper);

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

// Wrapper for bsec_update_subscription
static mp_obj_t bsec_update_subscription_wrapper(mp_obj_t requested_virtual_sensors_tuple) {
    mp_obj_t *requested_virtual_sensors;
    size_t n_requested_virtual_sensors;

    // Get the tuples
    mp_obj_get_array(requested_virtual_sensors_tuple, &n_requested_virtual_sensors, &requested_virtual_sensors);

    bsec_sensor_configuration_t requested_virtual_sensors_configs[n_requested_virtual_sensors];
    bsec_sensor_configuration_t required_sensor_settings_configs[BSEC_MAX_PHYSICAL_SENSOR];

    // Convert requested virtual sensors tuples to bsec_sensor_configuration_t
    for (size_t i = 0; i < n_requested_virtual_sensors; i++) {
        tuple_to_sensor_configuration(requested_virtual_sensors[i], &requested_virtual_sensors_configs[i]);
    }

    uint8_t n_required_sensor_settings = BSEC_MAX_PHYSICAL_SENSOR;

    // Call bsec_update_subscription
    bsec_library_return_t result = bsec_update_subscription(
        requested_virtual_sensors_configs,
        n_requested_virtual_sensors,
        required_sensor_settings_configs,
        &n_required_sensor_settings
    );

    if (result != BSEC_OK) {
        mp_raise_msg(&mp_type_RuntimeError, "Failed to update BSEC subscription");
    }

    // Convert required_sensor_settings_configs to tuples
    mp_obj_t required_sensor_settings_tuple = mp_obj_new_list(n_required_sensor_settings, NULL);
    for (size_t i = 0; i < n_required_sensor_settings; i++) {
        mp_obj_list_store(MP_OBJ_FROM_PTR(required_sensor_settings_tuple), MP_OBJ_NEW_SMALL_INT(i), sensor_configuration_to_tuple(&required_sensor_settings_configs[i]));
    }

    return required_sensor_settings_tuple;
}
static MP_DEFINE_CONST_FUN_OBJ_1(bsec_update_subscription_obj, bsec_update_subscription_wrapper);

// Helper function to convert bsec_bme_settings_t to dictionary
static mp_obj_t sensor_settings_to_dict(const bsec_bme_settings_t *settings) {
    mp_obj_t dict = mp_obj_new_dict(0);
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_next_call), mp_obj_new_int(settings->next_call));
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

// Wrapper for bsec_sensor_control
static mp_obj_t bsec_sensor_control_wrapper(mp_obj_t timestamp_obj) {
    int64_t timestamp = (int64_t)mp_obj_get_int(timestamp_obj);

    // Print the timestamp for debugging purposes
    mp_printf(&mp_plat_print, "Timestamp: %lld\n", timestamp);

    bsec_bme_settings_t sensor_settings;

    bsec_library_return_t result = bsec_sensor_control(timestamp, &sensor_settings);
    if (result != BSEC_OK) {
        mp_raise_msg(&mp_type_RuntimeError, "Failed to control BSEC sensor");
    }

    return sensor_settings_to_dict(&sensor_settings);
}
static MP_DEFINE_CONST_FUN_OBJ_1(bsec_sensor_control_obj, bsec_sensor_control_wrapper);

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
};

static MP_DEFINE_CONST_DICT(bsec_module_globals, bsec2_module_globals_table);

// Define module object.
const mp_obj_module_t bsec2_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&bsec_module_globals,
};

// Register the module to make it available in Python.
MP_REGISTER_MODULE(MP_QSTR_bsec2, bsec2_module);
