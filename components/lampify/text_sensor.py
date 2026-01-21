import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor
from esphome.const import (
    ENTITY_CATEGORY_DIAGNOSTIC,
)
from . import Lampify, CONF_LAMPIFY_ID, lampify_ns

DEPENDENCIES = ["lampify"]

LampifyPacketSensor = lampify_ns.class_(
    "LampifyPacketSensor", text_sensor.TextSensor, cg.PollingComponent
)

CONFIG_SCHEMA = (
    text_sensor.text_sensor_schema(
        LampifyPacketSensor,
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
    )
    .extend(
        {
            cv.GenerateID(CONF_LAMPIFY_ID): cv.use_id(Lampify),
        }
    )
    .extend(cv.polling_component_schema("1s"))
)


async def to_code(config):
    var = await text_sensor.new_text_sensor(config)
    await cg.register_component(var, config)
    parent = await cg.get_variable(config[CONF_LAMPIFY_ID])
    cg.add(var.set_parent(parent))
