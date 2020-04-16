\page device Device operations

\section reset_d Reset device

@startuml
Host    --> BioMcu:     CMD_RESET
BioMcu  --> Host:       CMD_RESET \nARG_RESULT[bep_result]
rnote over BioMcu:      Reset
@enduml
***
