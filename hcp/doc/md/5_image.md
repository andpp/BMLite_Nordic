\page image Image handling

\section create_i Create

@startuml
Host    --> BioMcu:     CMD_IMAGE \nARG_CREATE
hnote over BioMcu :     image
BioMcu  --> Host:       CMD_IMAGE \nARG_RESULT[bep_result]
@enduml
***

\section upload_i Upload

@startuml
participant Host
participant BioMcu

hnote over BioMcu :     image
Host    --> BioMcu:     CMD_IMAGE \nARG_UPLOAD
BioMcu  --> Host:       CMD_IMAGE \nARG_RESULT[bep_result] \nARG_DATA[image]
hnote over Host :       image
@enduml
***

\section download_i Download

@startuml
participant Host
participant BioMcu

hnote over Host :       image
Host    --> BioMcu:     CMD_IMAGE \nARG_DOWNLOAD \nARG_DATA[image]
hnote over BioMcu :     image
BioMcu  --> Host:       CMD_IMAGE \nARG_RESULT[bep_result]
@enduml
***
