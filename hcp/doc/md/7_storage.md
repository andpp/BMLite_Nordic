\page storage Storage handling

\section delete_id Delete ID

@startuml
participant Host
participant BioMcu

hnote over BioMcu :     template[id] (nvm)
Host    --> BioMcu:     CMD_STORAGE_TEMPLATE \nARG_DELETE \nARG_ID[id]
rnote over BioMcu :     fpc_template_storage_remove
BioMcu  --> Host:       CMD_STORAGE_TEMPLATE \nARG_RESULT[bep_result]
@enduml
***

\section delete_all Delete All

@startuml
participant Host
participant BioMcu

hnote over BioMcu :     template[] (nvm)
Host    --> BioMcu:     CMD_STORAGE_TEMPLATE \nARG_DELETE \nARG_ALL
rnote over BioMcu :     fpc_template_storage_remove_all
BioMcu  --> Host:       CMD_STORAGE_TEMPLATE \nARG_RESULT[bep_result]
@enduml
***

\section upload_s Upload

@startuml
participant Host
participant BioMcu

hnote over BioMcu :     template[id] (nvm)
Host    --> BioMcu:     CMD_STORAGE_TEMPLATE \nARG_UPLOAD \nARG_ID[id]
rnote over BioMcu :     fpc_template_storage_get
hnote over BioMcu :     template (ram)
BioMcu  --> Host:       CMD_STORAGE_TEMPLATE \nARG_RESULT[bep_result]

@enduml
***

\section count_s Count

@startuml
participant Host
participant BioMcu

Host    --> BioMcu:     CMD_STORAGE_TEMPLATE \nARG_COUNT
rnote over BioMcu :     fpc_template_storage_get_count
BioMcu  --> Host:       CMD_STORAGE_TEMPLATE \nARG_RESULT[bep_result] \nARG_COUNT[template_count]

@enduml
***

\section get_id Get IDs

@startuml
participant Host
participant BioMcu

hnote over BioMcu :     ids[] (nvm)
Host    --> BioMcu:     CMD_STORAGE_TEMPLATE \nARG_ID
rnote over BioMcu :     fpc_template_storage_get_all_ids
BioMcu  --> Host:       CMD_STORAGE_TEMPLATE \nARG_RESULT[bep_result] \nARG_DATA[ids]

@enduml
***