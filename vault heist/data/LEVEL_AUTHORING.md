# Adding campaign levels

Campaign entries are stored in `data/levels.json`. Keep each level as one
object inside the top-level `levels` array. The current loader supports the
flat string, integer, and boolean fields shown below; it does not support
nested objects or arrays inside a level.

```json
{
  "act": 2,
  "number": 1,
  "title": "Search Heist",
  "concept": "BST search",
  "objective": "Follow each comparison to find treasure 60.",
  "scenarioId": 1,
  "targetValue": 60,
  "optimalSteps": 3,
  "alarmCapacity": 7,
  "implemented": true
}
```

## Fields

- `act`: campaign act number, 1 through 4.
- `number`: level number within that act, 1 through 5.
- `title`: short name shown on the campaign map.
- `concept`: data-structure topic shown in the level preview.
- `objective`: player-facing instruction.
- `scenarioId`: ID passed to `GameModel::loadScenario`. Agree on its meaning
  with the model owner; the JSON number alone does not create a scenario.
  Scenario `25` is reserved for Act 2.5: it must load an empty BST so the GUI
  can insert keys `10, 20, 30, 40, 50, 60, 70` in sorted order and create the
  deliberately right-skewed search corridor.
  Scenario `31` is reserved for Act 3.1: it must load a fixed, deterministic
  five-node tree (50 with children 30 and 70; 30 -> 20 -> 10 on the left) whose
  balance factor is +2 at 50 and +2 at 30. Heights follow the leaf = 1 rule.
  Scenario `13` is reserved for Act 1.3: a fixed nine-room tree (ids 1-9, see CONTRACT.md)
  whose diameter bends at room 3. The GUI derives optimal steps (rooms + 1) and the alarm
  capacity from the model's postorder, so the level needs no `optimalSteps`.
- `targetValue`: optional target for levels that search or compare a key.
- `rangeLow` and `rangeHigh`: optional inclusive range bounds for range-query
  levels.
- `optimalSteps`: expected minimum operation count, used for stars. Use 0 or
  omit it when the model should determine this dynamically.
- `alarmCapacity`: failure limit. Use 0 or omit it to derive the limit as
  `ceil(1.5 * optimalSteps) + 2`.
- `implemented`: set to `true` only when the GUI has a working interaction for
  the level. Unimplemented entries remain visible but locked.

## Adding a level

1. Add an entry with a unique `(act, number)` pair. Keep levels in campaign
   order in the JSON file.
2. Select or create a model scenario and document the `scenarioId` with the
   model owner.
3. Add the level's interaction and success/failure flow in the GUI. Use model
   actions and events for data-structure behavior; do not implement tree or
   heap algorithms in the GUI.
4. Set `implemented` to `true` only after the interaction can be played.
5. Build and launch the app from the project root so it can find
   `data/levels.json`.

The current loader requires `act`, `number`, `title`, `concept`, `objective`,
and `implemented`. Other fields are optional. Keep JSON strings on one line
and escape quotation marks and backslashes according to JSON rules.