# Cat Safety

This repository currently describes an engineering prototype, not a finished cat toy.

Hard constraints before animal use:

1. No exposed LiPo battery.
2. No loose wires, loose bristles, sharp edges, detachable small parts, or exposed solder joints.
3. Body must be too large to swallow.
4. Firmware must stop motors on stale commands.
5. Hardware must include a physical power switch.
6. Firmware max duty must remain limited.
7. Use only under supervision until enclosure and durability are validated.

Validation checks:

| Check | Pass criteria |
|---|---|
| Pull test | No legs, shell parts, or wires detach under light pulling |
| Drop/tap test | No exposed electronics after repeated light impacts |
| Heat check | Motors, driver, and battery stay cool during test runs |
| Runtime stop | Motors stop after browser closes or commands stop |
| Size check | Finished body cannot be swallowed |

