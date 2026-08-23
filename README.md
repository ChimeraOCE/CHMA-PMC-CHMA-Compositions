# CHMA Compositions

Custom **Arma Reforger compositions, logistics assets and support systems** for **Chimera PMC (CHMA)**.

This addon provides reusable CHMA compositions and supporting prefabs for use in scenarios, Game Master and CHMA server environments.

The project also contains custom functionality for CHMA logistics and rekit systems.

## Dependencies

CHMA Compositions requires the following Arma Reforger addons.

### RHS

* `RHS - Content Pack 01`
* `RHS - Status Quo`
* `RHS - Content Pack 02`

### CHMA

* `CHMA Arsenal`
* `CHMA Patches`
* `CHMA Flags`
* `CHMA Factions`

### WCS

* `WCS_Clothing`
* `WCS_NATO`
* `WCS_RU`
* `WCS_Attachments`
* `WCS_Armbands`
* `WCS_Armaments`
* `WCS_Scopes`
* `WCS_Sounds`
* `WCS_Weapons`
* `WCS_Clothing_Assets`
* `WCS_Weapon_Scripts`

### ACE

* `ACE Core Dev`
* `ACE Medical Core Dev`
* `ACE Medical Circulation Dev`
* `ACE Captives Dev`
* `ACE Facepaint Dev`

### B59

* `B59 - FAL`
* `B59 Attachments`
* `B59 - SoundFX`
* `B59 - BravoCore`

### Attachment Framework

* `Attachment Framework`
* `Attachment Framework-Core`

### Weapons & Equipment

* `RIS Laser Attachments`
* `M249 Scope Rails`
* `Chungus Shotguns`
* `EF88 By Tonnie`
* `Medical Accessories`
* `TT Radio Backpacks`

### Characters & Clothing

* `Zeliks Character`
* `Civilian Clothing Mod`
* `AUS_CORE`

### Tools & Supporting Addons

* `Bacon Loadout Editor`
* `Placeables for GM byHeine`
* `TacticalAnimationOverhaul TEST`

All dependencies must be installed and enabled when opening, modifying or testing **CHMA Compositions** in Arma Reforger Workbench.

Compositions can reference resources from multiple addons at the same time. Removing a dependency without first replacing its referenced resources can result in missing objects, broken prefabs, script errors or compositions failing to load correctly.

When adding a new dependency, add it to this README. Before removing a dependency, confirm that no compositions, prefabs, scripts or inherited resources still rely on it.

## Project Structure

The project is organised around composition assets, Game Master configuration and supporting scripts.

```text
Configs/
└── Editor/
    └── PlaceableEntities/
        └── Groups/

Prefabs/
├── Compositions/
├── Items/
│   └── Equipment/
│       └── Backpacks/
└── Props/
    └── Military/

PrefabsEditable/
└── Auto/
    └── Props/

Scripts/
└── Game/
    ├── CHMA/
    ├── Components/
    └── UserActions/

addon.gproj
```

## Compositions

Reusable compositions are stored under:

```text
Prefabs/Compositions/
```

and:

```text
Prefabs/Props/Military/Compositions/
```

These assets are intended to provide pre-built objects and infrastructure that can be placed into CHMA scenarios without rebuilding the same setup for every operation.

Current composition-related assets include CHMA-specific supply and ammunition infrastructure.

When creating a new composition:

1. Build and test the composition in Workbench.
2. Save it within the existing composition structure.
3. Use a clear and descriptive prefab name.
4. Confirm all referenced resources belong to declared dependencies.
5. Test placement in both Workbench and the intended scenario.
6. Confirm the composition works correctly in multiplayer.

## Game Master Placeables

Game Master placeable configuration is maintained under:

```text
Configs/Editor/PlaceableEntities/Groups/
```

The project contains a placeable entity registry used to expose supported assets through Game Master.

When adding a composition intended for Game Master use, ensure it is correctly registered and exposed where required.

After making changes, verify that the asset:

* Appears in Game Master.
* Appears in the intended category.
* Can be placed correctly.
* Has the correct orientation.
* Does not generate missing-resource errors.
* Functions correctly after placement.

## CHMA Ammo Crates

CHMA-specific ammunition and supply crate assets are maintained within the project.

Example:

```text
Prefabs/Items/Equipment/Backpacks/CHMA_Crate_Ammo.et
```

Crates may contain inventory from CHMA and third-party equipment addons.

When modifying crate contents:

* Confirm all inventory resources are valid.
* Confirm magazines match their intended weapons.
* Check inventory capacity.
* Check the crate can be carried or interacted with as intended.
* Test resupply functionality.
* Check multiplayer replication.
* Check that removed dependencies are not still referenced by crate contents.

## Logistics System

The addon contains CHMA-specific logistics functionality.

Key components include:

```text
CHMA_LogiPointComponent.c
CHMA_CratePreloadComponent.c
```

These components support CHMA logistics points and supply crate behaviour.

When modifying logistics functionality, test it on a dedicated multiplayer environment where possible. Behaviour that works in Workbench or local testing may behave differently when replication is involved.

## Crate Request System

The project contains user actions for requesting and repacking CHMA supply crates:

```text
CHMA_RequestCrateAction.c
CHMA_RepackCrateAction.c
```

These systems should be treated as part of the CHMA logistics framework rather than standalone scripts.

When changing crate behaviour, verify:

* Crates can be requested correctly.
* Required interactions appear to players.
* Crates spawn at the intended location.
* Crates contain the expected inventory.
* Repacking works correctly.
* Actions cannot be unintentionally duplicated.
* Multiplayer clients receive the correct state.

## Rekit Terminal

The addon contains a custom CHMA rekit terminal system.

Key files include:

```text
CHMA_RekitTerminalComponent.c
CHMA_LaptopRekitUserAction.c
```

The rekit terminal allows configured character kit prefabs to be selected through interaction actions.

Individual actions can represent specific kits such as:

```text
Grab Kit: Rifleman
```

The available kits are controlled through the rekit terminal component.

When adding or changing kits:

1. Confirm the character prefab exists.
2. Add the prefab to the terminal's configured kit list.
3. Ensure the user action references the correct kit index.
4. Give the action a clear role name.
5. Test the interaction in-game.
6. Confirm the correct character kit is applied.
7. Test the system in multiplayer.

Changing the order of kit prefabs can change their indexes. Check all associated user actions after modifying the kit list.

## Development

This addon is developed using **Arma Reforger Workbench**.

Clone the repository into the local Reforger addons directory:

```text
Documents\My Games\ArmaReforger\addons\CHMA-PMC-CHMA-Compositions
```

Open:

```text
addon.gproj
```

through Arma Reforger Workbench.

The project ID is:

```text
CHMACompositions
```

Before modifying the project:

1. Install all required dependencies.
2. Enable the dependencies in Workbench.
3. Allow Workbench to finish processing resources.
4. Check the console for missing resources.
5. Resolve dependency errors before editing existing compositions.

## Modifying

This repository should remain focused on **CHMA compositions and the systems directly required by those compositions**.

Reusable assets that belong to another CHMA project should remain in that project and be referenced as dependencies.

For example:

* Faction configuration belongs in `CHMA Factions`.
* Arsenal configuration belongs in `CHMA Arsenal`.
* Patches belong in `CHMA Patches`.
* Flags belong in `CHMA Flags`.
* Reusable compositions and their supporting logistics functionality belong here.

Avoid copying assets from another project solely to avoid adding a dependency.

Do not directly modify third-party addon resources.

## Naming

Use clear names that identify both the asset and its CHMA purpose.

Examples:

```text
CHMA_Crate_Ammo.et
AmmoStorage_S_US_01_CHMA.et
CHMA_LogiPointComponent.c
CHMA_RequestCrateAction.c
```

New CHMA-specific resources should use the `CHMA_` prefix where appropriate.

Avoid generic names such as:

```text
NewComposition.et
Crate2.et
TestPrefab.et
```

unless the asset is temporary and will not be committed.

## Prefab Inheritance

Where possible, inherit from existing vanilla, CHMA or dependency prefabs rather than duplicating complete assets.

This makes future maintenance easier and avoids unnecessary duplicated configuration.

When modifying inherited prefabs:

* Check the parent prefab before overriding values.
* Only override properties required by the CHMA version.
* Avoid breaking inherited components.
* Check references after dependency updates.
* Test the final prefab rather than relying only on Workbench inheritance previews.

## Testing

Before committing changes:

* Open the project without missing-resource errors.
* Check the Workbench console.
* Test modified compositions.
* Test Game Master placement.
* Test CHMA ammunition crates.
* Test logistics points.
* Test crate requests.
* Test crate repacking.
* Test rekit terminals.
* Test affected user actions.
* Confirm all dependency resources load correctly.
* Test multiplayer replication.
* Test with the normal CHMA server modset.

For changes involving scripts, logistics, inventory or player interactions, dedicated-server testing is strongly recommended.

## Dependency Changes

Do not add dependencies simply because they are part of the normal CHMA server modset.

A dependency should exist because an asset, prefab, composition or script in this project relies on it.

When adding a dependency:

1. Add it to the project.
2. Confirm the dependency is actually required.
3. Add it to the dependency list in this README.
4. Test the project after adding it.

When removing a dependency:

1. Search compositions for references.
2. Search prefabs for references.
3. Check crate inventories.
4. Check inherited resources.
5. Check scripts where applicable.
6. Remove or replace all references.
7. Remove the dependency.
8. Test the project again.

## Git

Only files required to develop, build and maintain the addon should be committed.

Workbench-generated local files such as:

```text
resourceDatabase.rdb
```

should not be committed.

Before committing:

* Review all changed files.
* Check for accidentally deleted resources.
* Check for broken prefab references.
* Include required `.meta` files.
* Check dependency changes.
* Test affected compositions.
* Test affected scripts.
* Remove temporary test assets.
* Use a clear commit message.

Example commit messages:

```text
Add CHMA logistics point composition
```

```text
Update CHMA ammo crate contents
```

```text
Add Combat Medic rekit terminal option
```

```text
Fix crate request multiplayer behaviour
```

```text
Add new Game Master composition
```

## Notes

**CHMA Compositions** acts as the reusable composition and support-system layer for Chimera PMC scenarios.

The addon should allow commonly used CHMA infrastructure, supply setups and gameplay-support objects to be maintained once and reused across multiple scenarios.

If a future change introduces a new dependency, unusual Workbench setup requirement, special multiplayer behaviour or non-obvious relationship between compositions and supporting scripts, document it in this README.
