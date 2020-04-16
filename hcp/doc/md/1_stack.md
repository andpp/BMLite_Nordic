\page stack FPC embedded stack

The communication stack implemented on the embedded devices by FPC follows the following
specification.

\image html stack.svg "HCP embedded stack"
\image latex stack.eps "HCP embedded stack"

\section phy Physical

The physical layer have a fixed size buffer of 256 bytes.

\section link Link

The link layer handles packet consistency.

Each packet received is acknowledged on the link layer, if an error occurs no retransmission is
done on this level, instead the error is propagated upwards.

Channel | Size    | Payload    | CRC     |
------- | ------- | ---------- | ------- |
2 bytes | 2 bytes | size bytes | 4 bytes |

All fields are using unsigned data types.

\section tsp Transport

The transport layer handles packet segmentation.

As the PHY MTU is 256 bytes the maximum payload per segment is 242 bytes.

Errors are propagated upwards.

Size    | Seq Nr  | Seq Len | Payload    |
------- | ------- | ------- | ---------- |
2 bytes | 2 bytes | 2 bytes | size bytes |

All fields are using unsigned data types.

\section app Application

The application layer is a optional security layer, the default implementation is clear text
(unsecure).

If a security solution is used it will be part of that products documentation.

\section hcp HCP

The HCP frame is described in the \link hcpf HCP frame format section.
