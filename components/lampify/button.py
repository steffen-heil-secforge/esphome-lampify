import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import button
from esphome.const import (
    CONF_ID,
    ENTITY_CATEGORY_CONFIG,
)
from . import lampify_ns, Lampify, CONF_LAMP_INDEX

DEPENDENCIES = ["lampify"]

LampifyPairButton = lampify_ns.class_("LampifyPairButton", button.Button, cg.Component)

CONFIG_SCHEMA = (
    button.button_schema(
        LampifyPairButton,
        icon="mdi:bluetooth",
        entity_category=ENTITY_CATEGORY_CONFIG,
    )
    .extend(
        {
            cv.GenerateID("lampify_id"): cv.use_id(Lampify),
            cv.Optional(CONF_LAMP_INDEX, default=0): cv.int_range(min=0, max=15),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config):
    var = await button.new_button(config)
    await cg.register_component(var, config)

    parent = await cg.get_variable(config["lampify_id"])
    cg.add(var.set_parent(parent))
    cg.add(var.set_lamp_index(config[CONF_LAMP_INDEX]))
