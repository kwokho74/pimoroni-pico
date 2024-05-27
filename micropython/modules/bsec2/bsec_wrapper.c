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
