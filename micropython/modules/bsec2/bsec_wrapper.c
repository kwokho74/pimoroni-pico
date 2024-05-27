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
    uint32_t n_serialized_state = BSEC_MAX_STATE_BLOB_SIZE;
    bsec_library_return_t result = bsec_get_state(state_buffer, BSEC_MAX_STATE_BLOB_SIZE, &n_serialized_state);
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
};

static MP_DEFINE_CONST_DICT(bsec_module_globals, bsec2_module_globals_table);

// Define module object.
const mp_obj_module_t bsec2_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&bsec_module_globals,
};

// Register the module to make it available in Python.
MP_REGISTER_MODULE(MP_QSTR_bsec2, bsec2_module);
