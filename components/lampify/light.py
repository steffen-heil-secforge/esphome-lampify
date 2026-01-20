import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import light
from esphome.const import CONF_OUTPUT_ID
from . import lampify_ns, Lampify, CONF_DEVICE_ID

DEPENDENCIES = ["lampify"]

LampifyLight = lampify_ns.class_("LampifyLight", light.LightOutput, cg.Component)

CONFIG_SCHEMA = light.RGB_LIGHT_SCHEMA.extend(
    {
        cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(LampifyLight),
        cv.Required(CONF_DEVICE_ID): cv.hex_uint16_t,
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_OUTPUT_ID])
    await cg.register_component(var, config)
    await light.register_light(var, config)
    cg.add(var.set_device_id(config[CONF_DEVICE_ID]))
