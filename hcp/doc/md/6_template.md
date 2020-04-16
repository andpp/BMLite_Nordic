\page template Template handling

\section upload_t Upload

@startuml
participant Host
participant BioMcu

hnote over BioMcu :     template
Host    --> BioMcu:     CMD_TEMPLATE \nARG_UPLOAD
BioMcu  --> Host:       CMD_TEMPLATE \nARG_RESULT[bep_result] \nARG_DATA[template]
hnote over Host :       template
@enduml
***

\section download_t Download

@startuml
participant Host
participant BioMcu

hnote over Host :       template
Host    --> BioMcu:     CMD_TEMPLATE \nARG_DOWNLOAD \nARG_DATA[template]
hnote over BioMcu :     template
BioMcu  --> Host:       CMD_TEMPLATE \nARG_RESULT[bep_result]
@enduml
***

\section save_t Save

@startuml
participant Host
participant BioMcu

hnote over BioMcu :     template
Host    --> BioMcu:     CMD_TEMPLATE \nARG_SAVE \nARG_ID[id]
hnote over BioMcu :     template[id] (nvm)
BioMcu  --> Host:       CMD_TEMPLATE \nARG_RESULT[bep_result]
@enduml
***
