/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_behavior_profile

#include <zephyr/device.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <drivers/behavior.h>
#include <dt-bindings/zmk/profile.h>
#include <zmk/behavior.h>

#include <zmk/keymap.h>

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

#if IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)

static const struct behavior_parameter_value_metadata no_arg_values[] = {
    {
        .display_name = "Next Profile",
        .type = BEHAVIOR_PARAMETER_VALUE_TYPE_VALUE,
        .value = PROFILE_NXT_CMD,
    },
    {
        .display_name = "Previous Profile",
        .type = BEHAVIOR_PARAMETER_VALUE_TYPE_VALUE,
        .value = PROFILE_PRV_CMD,
    },
};

static const struct behavior_parameter_metadata_set no_args_set = {
    .param1_values = no_arg_values,
    .param1_values_len = ARRAY_SIZE(no_arg_values),
};

static const struct behavior_parameter_value_metadata prof_index_param1_values[] = {
    {
        .display_name = "Select Profile",
        .type = BEHAVIOR_PARAMETER_VALUE_TYPE_VALUE,
        .value = PROFILE_SEL_CMD,
    },
};

static const struct behavior_parameter_value_metadata prof_index_param2_values[] = {
    {
        .display_name = "Profile",
        .type = BEHAVIOR_PARAMETER_VALUE_TYPE_RANGE,
        .range = {.min = 0, .max = ZMK_KEYMAP_PROFILES_LEN - 1},
    },
};

static const struct behavior_parameter_metadata_set profile_index_metadata_set = {
    .param1_values = prof_index_param1_values,
    .param1_values_len = ARRAY_SIZE(prof_index_param1_values),
    .param2_values = prof_index_param2_values,
    .param2_values_len = ARRAY_SIZE(prof_index_param2_values),
};

static const struct behavior_parameter_metadata_set metadata_sets[] = {no_args_set,
                                                                       profile_index_metadata_set};

static const struct behavior_parameter_metadata metadata = {
    .sets_len = ARRAY_SIZE(metadata_sets),
    .sets = metadata_sets,
};

#endif // IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)

static int on_keymap_binding_pressed(struct zmk_behavior_binding *binding,
                                     struct zmk_behavior_binding_event event) {
    int current_profile = zmk_keymap_profile_index();
    int ret;

    LOG_DBG("Profile behavior pressed: cmd=%d param2=%d pos=%d layer=%d current_profile=%d",
            binding->param1, binding->param2, event.position, event.layer, current_profile);

    switch (binding->param1) {
    case PROFILE_NXT_CMD:
        ret = zmk_keymap_profile_next();
        break;
    case PROFILE_PRV_CMD:
        ret = zmk_keymap_profile_prev();
        break;
    case PROFILE_SEL_CMD:
        ret = zmk_keymap_profile_select(binding->param2);
        break;
    default:
        LOG_ERR("Unknown profile command: %d", binding->param1);
        return -ENOTSUP;
    }

    if (ret < 0) {
        LOG_ERR("Profile behavior failed: cmd=%d ret=%d", binding->param1, ret);
        return ret;
    }

    LOG_DBG("Profile behavior complete: cmd=%d profile %d -> %d", binding->param1,
            current_profile, zmk_keymap_profile_index());

    return ret;
}

static int on_keymap_binding_released(struct zmk_behavior_binding *binding,
                                      struct zmk_behavior_binding_event event) {
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api behavior_profile_driver_api = {
    .binding_pressed = on_keymap_binding_pressed,
    .binding_released = on_keymap_binding_released,
#if IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
    .parameter_metadata = &metadata,
#endif // IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
};

BEHAVIOR_DT_INST_DEFINE(0, NULL, NULL, NULL, NULL, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,
                        &behavior_profile_driver_api);

#endif /* DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT) */
