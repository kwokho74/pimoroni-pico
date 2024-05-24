#include "py/obj.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "bsec_datatypes.h"
#include "bsec_interface.h"

// Wrapper for bsec_get_version
static mp_obj_t bsec_get_version_wrapper(void) {
    bsec_version_t version;
    bsec_library_return_t result = bsec_get_version(&version);
    if (result != BSEC_OK) {
        mp_raise_msg(&mp_type_RuntimeError, "Failed to get BSEC version");
    }

    mp_obj_t tuple[4];
    tuple[0] = mp_obj_new_int(version.major);
    tuple[1] = mp_obj_new_int(version.minor);
    tuple[2] = mp_obj_new_int(version.major_bugfix);
    tuple[3] = mp_obj_new_int(version.minor_bugfix);

    return mp_obj_new_tuple(4, tuple);
}
static MP_DEFINE_CONST_FUN_OBJ_0(bsec_get_version_obj, bsec_get_version_wrapper);

// // Wrapper for bsec_init
// static mp_obj_t bsec_init_wrapper(void) {
//     bsec_library_return_t result = bsec_init();
//     if (result != BSEC_OK) {
//         mp_raise_msg(&mp_type_RuntimeError, "Failed to initialize BSEC library");
//     }
//     return mp_const_none;
// }
// static MP_DEFINE_CONST_FUN_OBJ_0(bsec_init_obj, bsec_init_wrapper);

// // Wrapper for bsec_update_subscription
// static mp_obj_t bsec_update_subscription_wrapper(mp_obj_t requested_virtual_sensors_list, mp_obj_t required_sensor_settings_list) {
//     size_t n_requested_virtual_sensors;
//     mp_obj_t *requested_virtual_sensors_items;
//     mp_obj_get_array(requested_virtual_sensors_list, &n_requested_virtual_sensors, &requested_virtual_sensors_items);

//     size_t n_required_sensor_settings;
//     mp_obj_t *required_sensor_settings_items;
//     mp_obj_get_array(required_sensor_settings_list, &n_required_sensor_settings, &required_sensor_settings_items);

//     bsec_sensor_configuration_t requested_virtual_sensors[n_requested_virtual_sensors];
//     bsec_sensor_configuration_t required_sensor_settings[BSEC_MAX_PHYSICAL_SENSOR];
    
//     for (size_t i = 0; i < n_requested_virtual_sensors; i++) {
//         mp_obj_t *tuple;
//         mp_obj_get_array_fixed_n(requested_virtual_sensors_items[i], 2, &tuple);
//         requested_virtual_sensors[i].sensor_id = mp_obj_get_int(tuple[0]);
//         requested_virtual_sensors[i].sample_rate = mp_obj_get_float(tuple[1]);
//     }

//     uint8_t n_required_sensor_settings_out = BSEC_MAX_PHYSICAL_SENSOR;
//     bsec_library_return_t result = bsec_update_subscription(requested_virtual_sensors, n_requested_virtual_sensors, required_sensor_settings, &n_required_sensor_settings_out);
//     if (result != BSEC_OK) {
//         mp_raise_msg(&mp_type_RuntimeError, "Failed to update BSEC subscription");
//     }

//     mp_obj_t output_list = mp_obj_new_list(n_required_sensor_settings_out, NULL);
//     for (uint8_t i = 0; i < n_required_sensor_settings_out; i++) {
//         mp_obj_t tuple[2];
//         tuple[0] = mp_obj_new_int(required_sensor_settings[i].sensor_id);
//         tuple[1] = mp_obj_new_float(required_sensor_settings[i].sample_rate);
//         mp_obj_list_store(output_list, i, mp_obj_new_tuple(2, tuple));
//     }

//     return output_list;
// }
// static MP_DEFINE_CONST_FUN_OBJ_2(bsec_update_subscription_obj, bsec_update_subscription_wrapper);

// // Wrapper for bsec_do_steps
// // bsec_library_return_t bsec_do_steps(const bsec_input_t * const inputs, const uint8_t n_inputs, bsec_output_t * outputs, uint8_t * n_outputs);
// static mp_obj_t bsec_do_steps_wrapper(mp_obj_t inputs_list) {
//     size_t n_inputs;
//     mp_obj_t *inputs_items;
//     mp_obj_get_array(inputs_list, &n_inputs, &inputs_items);

//     bsec_input_t inputs[n_inputs];
//     for (size_t i = 0; i < n_inputs; i++) {
//         mp_obj_t *tuple;
//         mp_obj_get_array_fixed_n(inputs_items[i], 2, &tuple);
//         inputs[i].sensor_id = mp_obj_get_int(tuple[0]);
//         inputs[i].signal = mp_obj_get_float(tuple[1]);
//     }

//     bsec_output_t outputs[BSEC_NUMBER_OUTPUTS];
//     uint8_t n_outputs;
//     bsec_library_return_t result = bsec_do_steps(inputs, n_inputs, outputs, &n_outputs);
//     if (result != BSEC_OK) {
//         mp_raise_msg(&mp_type_RuntimeError, "Failed to run BSEC steps");
//     }

//     mp_obj_t output_list = mp_obj_new_list(n_outputs, NULL);
//     for (uint8_t i = 0; i < n_outputs; i++) {
//         mp_obj_t tuple[2];
//         tuple[0] = mp_obj_new_int(outputs[i].sensor_id);
//         tuple[1] = mp_obj_new_float(outputs[i].signal);
//         mp_obj_list_store(output_list, i, mp_obj_new_tuple(2, tuple));
//     }

//     return output_list;
// }
// static MP_DEFINE_CONST_FUN_OBJ_1(bsec_do_steps_obj, bsec_do_steps_wrapper);

// Define all attributes of the module.
// Table entries are key/value pairs of the attribute name (a string)
// and the MicroPython object reference.
// All identifiers and strings are written as MP_QSTR_xxx and will be
// optimized to word-sized integers by the build system (interned strings).
static const mp_rom_map_elem_t bsec2_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_bsec) },
    { MP_ROM_QSTR(MP_QSTR_get_version), MP_ROM_PTR(&bsec_get_version_obj) },
};

static MP_DEFINE_CONST_DICT(bsec_module_globals, bsec2_module_globals_table);

// Define module object.
const mp_obj_module_t bsec2_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&bsec_module_globals,
};

// Register the module to make it available in Python.
MP_REGISTER_MODULE(MP_QSTR_bsec2, bsec2_module);
