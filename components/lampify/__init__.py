import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID
from esphome.components import esp32_ble

DEPENDENCIES = ["esp32_ble"]
CODEOWNERS = ["@steffen-heil-secforge"]
AUTO_LOAD = ["number", "button"]

CONF_DEVICE_ID = "device_id"

lampify_ns = cg.esphome_ns.namespace("lampify")
Lampify = lampify_ns.class_("Lampify", cg.Component)

# Actions
TurnOnAction = lampify_ns.class_("TurnOnAction", cg.Action)
TurnOffAction = lampify_ns.class_("TurnOffAction", cg.Action)
SetLevelAction = lampify_ns.class_("SetLevelAction", cg.Action)
PairAction = lampify_ns.class_("PairAction", cg.Action)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(Lampify),
        cv.Optional(CONF_DEVICE_ID): cv.hex_uint16_t,
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    if CONF_DEVICE_ID in config:
        cg.add(var.set_device_id(config[CONF_DEVICE_ID]))


# Action schemas
LAMPIFY_ACTION_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.use_id(Lampify),
    }
)

LAMPIFY_SET_LEVEL_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.use_id(Lampify),
        cv.Required("cold"): cv.templatable(cv.uint8_t),
        cv.Required("warm"): cv.templatable(cv.uint8_t),
    }
)


@cg.action_registry.register("lampify.turn_on", LAMPIFY_ACTION_SCHEMA)
async def turn_on_action_to_code(config, action_id, template_arg, args):
    var = cg.new_Pvariable(action_id, template_arg)
    await cg.register_parented(var, config[CONF_ID])
    return var


@cg.action_registry.register("lampify.turn_off", LAMPIFY_ACTION_SCHEMA)
async def turn_off_action_to_code(config, action_id, template_arg, args):
    var = cg.new_Pvariable(action_id, template_arg)
    await cg.register_parented(var, config[CONF_ID])
    return var


@cg.action_registry.register("lampify.set_level", LAMPIFY_SET_LEVEL_SCHEMA)
async def set_level_action_to_code(config, action_id, template_arg, args):
    var = cg.new_Pvariable(action_id, template_arg)
    await cg.register_parented(var, config[CONF_ID])
    template_ = await cg.templatable(config["cold"], args, cg.uint8)
    cg.add(var.set_cold(template_))
    template_ = await cg.templatable(config["warm"], args, cg.uint8)
    cg.add(var.set_warm(template_))
    return var


@cg.action_registry.register("lampify.pair", LAMPIFY_ACTION_SCHEMA)
async def pair_action_to_code(config, action_id, template_arg, args):
    var = cg.new_Pvariable(action_id, template_arg)
    await cg.register_parented(var, config[CONF_ID])
    return var
