import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import number
from esphome.const import (
    CONF_ID,
    CONF_MODE,
    ENTITY_CATEGORY_CONFIG,
    ICON_RADIO_TOWER,
)
from . import lampify_ns, Lampify

DEPENDENCIES = ["lampify"]

LampifyDeviceIdNumber = lampify_ns.class_(
    "LampifyDeviceIdNumber", number.Number, cg.Component
)

CONFIG_SCHEMA = (
    number.number_schema(
        LampifyDeviceIdNumber,
        icon=ICON_RADIO_TOWER,
        entity_category=ENTITY_CATEGORY_CONFIG,
    )
    .extend(
        {
            cv.GenerateID("lampify_id"): cv.use_id(Lampify),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config):
    var = await number.new_number(
        config,
        min_value=0,
        max_value=65535,
        step=1,
    )
    await cg.register_component(var, config)

    parent = await cg.get_variable(config["lampify_id"])
    cg.add(var.set_parent(parent))
