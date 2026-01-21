import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID
from esphome.components import esp32_ble
from esphome import automation

DEPENDENCIES = ["esp32_ble"]
CODEOWNERS = ["@steffen-heil-secforge"]
AUTO_LOAD = ["number", "button", "text_sensor"]

CONF_DEVICE_ID = "device_id"
CONF_LAMPIFY_ID = "lampify_id"
CONF_LAMPS = "lamps"
CONF_LAMP_INDEX = "lamp_index"

lampify_ns = cg.esphome_ns.namespace("lampify")
Lampify = lampify_ns.class_("Lampify", cg.Component)

# Actions
TurnOnAction = lampify_ns.class_("TurnOnAction", automation.Action)
TurnOffAction = lampify_ns.class_("TurnOffAction", automation.Action)
SetLevelAction = lampify_ns.class_("SetLevelAction", automation.Action)
PairAction = lampify_ns.class_("PairAction", automation.Action)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(Lampify),
        cv.Optional(CONF_LAMPS, default=8): cv.int_range(min=1, max=16),
        cv.Optional(CONF_DEVICE_ID): cv.hex_uint16_t,  # Legacy: sets lamp 0
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    cg.add(var.set_num_lamps(config[CONF_LAMPS]))

    if CONF_DEVICE_ID in config:
        cg.add(var.set_device_id(0, config[CONF_DEVICE_ID]))


# Action schemas
LAMPIFY_ACTION_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.use_id(Lampify),
        cv.Optional(CONF_LAMP_INDEX, default=0): cv.templatable(cv.uint8_t),
    }
)

LAMPIFY_SET_LEVEL_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.use_id(Lampify),
        cv.Optional(CONF_LAMP_INDEX, default=0): cv.templatable(cv.uint8_t),
        cv.Required("cold"): cv.templatable(cv.uint8_t),
        cv.Required("warm"): cv.templatable(cv.uint8_t),
    }
)


@automation.register_action("lampify.turn_on", TurnOnAction, LAMPIFY_ACTION_SCHEMA)
async def turn_on_action_to_code(config, action_id, template_arg, args):
    var = cg.new_Pvariable(action_id, template_arg)
    await cg.register_parented(var, config[CONF_ID])
    template_ = await cg.templatable(config[CONF_LAMP_INDEX], args, cg.uint8)
    cg.add(var.set_lamp_index(template_))
    return var


@automation.register_action("lampify.turn_off", TurnOffAction, LAMPIFY_ACTION_SCHEMA)
async def turn_off_action_to_code(config, action_id, template_arg, args):
    var = cg.new_Pvariable(action_id, template_arg)
    await cg.register_parented(var, config[CONF_ID])
    template_ = await cg.templatable(config[CONF_LAMP_INDEX], args, cg.uint8)
    cg.add(var.set_lamp_index(template_))
    return var


@automation.register_action("lampify.set_level", SetLevelAction, LAMPIFY_SET_LEVEL_SCHEMA)
async def set_level_action_to_code(config, action_id, template_arg, args):
    var = cg.new_Pvariable(action_id, template_arg)
    await cg.register_parented(var, config[CONF_ID])
    template_ = await cg.templatable(config[CONF_LAMP_INDEX], args, cg.uint8)
    cg.add(var.set_lamp_index(template_))
    template_ = await cg.templatable(config["cold"], args, cg.uint8)
    cg.add(var.set_cold(template_))
    template_ = await cg.templatable(config["warm"], args, cg.uint8)
    cg.add(var.set_warm(template_))
    return var


@automation.register_action("lampify.pair", PairAction, LAMPIFY_ACTION_SCHEMA)
async def pair_action_to_code(config, action_id, template_arg, args):
    var = cg.new_Pvariable(action_id, template_arg)
    await cg.register_parented(var, config[CONF_ID])
    template_ = await cg.templatable(config[CONF_LAMP_INDEX], args, cg.uint8)
    cg.add(var.set_lamp_index(template_))
    return var
