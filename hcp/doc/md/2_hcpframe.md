\page hcpf HCP frame format

The Host Communication Protocol (HCP) describes a general way of sending commands and information
between devices.

\image html hcp.svg "HCP frame format"
\image latex hcp.eps "HCP frame format"

\section cmd Command

The Commands define the general action that is going to be executed. However, each command
can have several Arguments each with data attached.

| CMD     | Num Args | Payload  |
| ------- | -------- | -------- |
| 2 bytes | 2 bytes  | xx bytes |

All fields are using unsigned data types.

\section arg Argument

The Argument is used as a complement to the command if it is needed and can contain arbitrary data.

| ARG     | Size    | Data       |
| ------- | --------| ---------- |
| 2 bytes | 2 bytes | size bytes |

All fields are using unsigned data types.
