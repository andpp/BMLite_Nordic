\page sensor Sensor operations

\section wait_up_s Wait for finger up

@startuml
Host    --> BioMcu:     CMD_WAIT \nARG_FINGER_UP \nARG_TIMEOUT[timeout]
... Wait for finger up...
BioMcu  --> Host:       CMD_WAIT \nARG_RESULT[bep_result]
@enduml
***

\section wait_down_s Wait for finger down

@startuml
Host    --> BioMcu:     CMD_WAIT \nARG_FINGER_DOWN \nARG_TIMEOUT[timeout]
... Wait for finger down...
BioMcu  --> Host:       CMD_WAIT \nARG_RESULT[bep_result]
@enduml
***

\section sensor_reset_s Reset sensor

@startuml
Host    -->  BioMcu :   CMD_SENSOR \nARG_RESET
rnote   over BioMcu :   fpc_bep_sensor_reset
BioMcu  -->  Host   :   CMD_SENSOR \nARG_RESULT[bep_result]
@enduml
***
