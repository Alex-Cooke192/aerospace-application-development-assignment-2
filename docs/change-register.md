
# AFRM Software Change Register

## 1. Version History

| Version | Change Implemented | Services Affected | Date of Version Creation | Actions Required | Status | git commit hash
|---|---|---|---|---|---|---|
| v0.1.0 | Added SITL sensors | Sensor, FuelSensor, AirSpeedSensor| TBD | n/a | Superseded | acd4f06 |
| v0.2.0 | Added EventBus | EventBus | TBD | n/a | Superseded | 0999ccd |
| v0.3.0 | Added FuelDisplayService | FuelDisplayService | TBD | n/a | Superseded | 12bdf31 |
| v0.4.0 | Added FuelService | FuelService, FuelDisplayService, EventBus, AirSpeedSensor, FuelSensor | TBD | establish initial baseline | Superseded | 25381ab |
| v1.0.0 | Initial controlled software baseline | FuelService, FuelDisplayService, EventBus | TBD | Verify baseline configuration, record commit hash and complete baseline testing | Superseded | efb582f |
| v1.1.0 | Modified fuel service fuel transition edge case | FuelService | TBD | n/a | Superseded | c63325d |
| v1.1.1 | Added unit tests | FuelService, FuelDisplayService, EventBus | TBD | Write integration tests & end-to-end tests, complete coverage analysis for unit tests | Superseded | 4ea7b81 | 
| v1.1.2 | Added integration tests | FuelService, FuelDisplayService, EventBus | TBD | Complete tests, verify results & complete regression code coverage analysis | Superseded | cb9525a | 
| v1.1.3 | Added end-to-end tests | FuelService, FuelDisplayService, EventBus, FuelSensor, AirSpeedSensor | TBD | complete tests, verify results and complete regression code coverage analysis | Superseded | 8ea5b96 |
| v1.1.4 | Restructured project configuration to include conditional compiler generation | CMake project configuration | TBD | n/a | Approved |

## 2. Versioning Convention

The AFRM uses Semantic Versioning (MAJOR.MINOR.PATCH):

- **MAJOR:** Significant architectural restructuring, redistribution of service responsibilities, or modifications to existing internal or external software interfaces.
- **MINOR:** Small-scale functionality or internal enhancements that preserve all interfaces (internal and external) and do not significantly restructure the software architecture.
- **MICRO/PATCH:** Defect corrections, maintenance activities or very small modifications that do not alter application functionality or interfaces.

## 3. Configuration-Control Procedure

Each software version shall:

1. Reference a documented change request or approved modification.
2. Identify affected requirements, services and interfaces.
3. Record implementation changes through Git commits.
4. Complete appropriate verification and regression testing.
5. Undergo a documented software review.
6. Update affected requirements and design documentation.
7. Receive a Git version tag following successful verification and approval.

## 4. Change Status Definitions

| Status | Meaning |
|---|---|
| Planned | Modification proposed but not implemented |
| In Progress | Modification currently being developed |
| Under Review | Implementation completed and undergoing verification/review |
| Approved | Modification verified and approved for baseline release |
| Rejected | Proposed modification not accepted |
| Superseded | Version replaced by a newer approved baseline |
