\page bio Biometrics

\section capture_b Capture

@startuml
Host    --> BioMcu:     CMD_CAPTURE \nARG_TIMEOUT[timeout]
... Wait for finger down...
rnote over BioMcu :     fpc_bep_capture
hnote over BioMcu :     image
BioMcu  --> Host:       CMD_CAPTURE \nARG_RESULT[bep_result]
@enduml
***

\section extract_b Extract

@startuml
group Capture
Host    --> BioMcu:     CMD_CAPTURE \nARG_TIMEOUT[timeout]
... Wait for finger down...
rnote over BioMcu :     fpc_bep_capture
hnote over BioMcu :     image
BioMcu  --> Host:       CMD_CAPTURE \nARG_RESULT[bep_result]
end
Host    --> BioMcu:     CMD_IMAGE \nARG_EXTRACT
rnote over BioMcu :     fpc_bep_image_extract
hnote over BioMcu :     template
BioMcu  --> Host:       CMD_IMAGE \nARG_RESULT[bep_result]
@enduml
***

\section enroll_b Enroll

@startuml
Host    --> BioMcu:     CMD_ENROLL \nARG_START
rnote over BioMcu :     fpc_bep_enroll_start
hnote over BioMcu :     session
BioMcu  --> Host:       CMD_ENROLL \nARG_RESULT[bep_result]
|||
loop Enroll Image
group Capture
Host    --> BioMcu:     CMD_CAPTURE \nARG_TIMEOUT[timeout]
... Wait for finger down...
rnote over BioMcu :     fpc_bep_capture
hnote over BioMcu :     image
BioMcu  --> Host:       CMD_CAPTURE \nARG_RESULT[bep_result]
end
|||
Host    --> BioMcu:     CMD_ENROLL \nARG_ADD
rnote over BioMcu :     fpc_bep_enroll
rnote over BioMcu :     count
BioMcu  --> Host:       CMD_ENROLL \nARG_COUNT[count] \nARG_RESULT[bep_result]
Host    --> BioMcu:     CMD_WAIT \nARG_FINGER_UP \nARG_TIMEOUT[timeout]
... Wait for finger up...
BioMcu  --> Host:       CMD_WAIT \nARG_RESULT[bep_result]
end
|||
Host    --> BioMcu:     CMD_ENROLL \nARG_FINISH
rnote over BioMcu :     fpc_bep_enroll_finish
hnote over BioMcu :     enroll template
BioMcu  --> Host:       CMD_ENROLL \nARG_RESULT[bep_result]
@enduml
***

\section identify_b Identify

@startuml
group Capture
Host    --> BioMcu:     CMD_CAPTURE \nARG_TIMEOUT[timeout]
... Wait for finger down...
rnote over BioMcu :     fpc_bep_capture
hnote over BioMcu :     image
BioMcu  --> Host:       CMD_CAPTURE \nARG_RESULT[bep_result]
end
|||
group Extract
Host    --> BioMcu:     CMD_IMAGE \nARG_EXTRACT
rnote over BioMcu :     fpc_bep_image_extract
hnote over BioMcu :     template
BioMcu  --> Host:       CMD_IMAGE \nARG_RESULT[bep_result]
end
|||
Host    --> BioMcu:     CMD_IDENTIFY
rnote over BioMcu :     fpc_template_storage_get_all
rnote over BioMcu :     fpc_bep_identify
hnote over BioMcu :     id
BioMcu  --> Host:       CMD_IDENTIFY \nARG_MATCH \nARG_ID[id] \nARG_RESULT[bep_result]
hnote over Host:        id
@enduml
***
