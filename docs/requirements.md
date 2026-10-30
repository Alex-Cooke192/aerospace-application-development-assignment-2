
# AFRM Software Requirements

## 1. Functional Requirements

| ID | Requirement Description | Acceptance Criteria | Rationale |
|---|---|---|---|
| FR-01 – Sensor Input | Obtain and process fuel-level and airspeed sensor data. | New sensor values are received and stored by FuelService. | Provides inputs for fuel and range calculations. |
| FR-02 – Consumption & Range | Calculate current fuel consumption and estimated remaining range. | Values are recalculated when relevant inputs change and updated when required. | Provides fuel endurance and range information. |
| FR-03 – Fuel State & Warning Detection | Maintain NORMAL, WARN or CRITICAL fuel state based on defined thresholds. | NORMAL applies above the warning threshold, WARN at or below warning and above critical, and CRITICAL at or below critical. State transitions raise or clear the corresponding warning events. | Provides deterministic fuel-status behaviour and automatic detection of significant fuel conditions. |
| FR-04 – Information Display | Display fuel level, consumption, range and warning information. | FuelDisplayService updates the relevant information when fuel events are received. | Ensures fuel information is available to the operator. |
| FR-05 – Event Communication | Publish fuel-state changes through the EventBus. | Changes in fuel level, consumption, range or warnings generate events for subscribed services. | Reduces service coupling through publish/subscribe communication. |


## 2. Non-functional Requirements

| ID | Requirement Description | Acceptance Criteria | Rationale |
|---|---|---|---|
| NFR-01 – Loose Coupling | Services shall communicate through the EventBus rather than direct dependencies. | FuelService sends fuel information to FuelDisplayService through subscribed events only. | Reduces dependencies and simplifies component changes. |
| NFR-02 – Responsiveness | Process sensor updates and resulting fuel-state changes within the current processing cycle. | Calculations, state checks, event publishing and handling complete before the next sensor cycle. | Keeps fuel and warning information current. |
| NFR-03 – Maintainability | Keep sensing, fuel logic, event handling and presentation as separate components. | Display or sensor components can be changed without affecting unrelated components. | Improves separation of concerns and maintainability. |
| NFR-04 – Extensibility | Allow new event publishers or subscribers without changing existing components. | New services can publish or subscribe to compatible events through the EventBus without modifying existing services. | Supports future system expansion. |
| NFR-05 – Testability | Components shall support independent testing using controlled inputs and observable outputs. | Tests can verify calculations, states, warnings and events without physical aircraft hardware. | Enables reliable and repeatable verification. |


## 3. Interface Requirements

| ID | Requirement Description | Acceptance Criteria | Rationale |
|---|---|---|---|
| IR-01 – Event Payloads | Define a standard payload structure for each event type. | Publishers and subscribers use the same defined fields for each event. | Prevents ambiguity and incompatible event handling. |
| IR-02 – EventBus Interface | Components shall communicate through the defined EventBus publish/subscribe interface. | Components use Subscribe() and Publish() without knowing the receiving service. | Maintains loose coupling between services. |
| IR-03 – Sensor Interface | Fuel and airspeed sensors shall use a common sensor interface. | FuelSensor and AirSpeedSensor can be accessed through the shared Sensor abstraction. | Provides consistent sensor access and supports future replacement or extension. |

