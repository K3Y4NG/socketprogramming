# socketprogramming
In this socket programming two patients try to communicate with the healthserver to fix appointments with a doctor. There are two available doctor, each with thier own availabilities schedule. 
A patient connets to the healthserver using a TCP connection. After receiving the required information from the command line from the patient, the healthserver connects with a UDP connection to the desired doctor.
The doctor then sends in his availablility to the healthserver, which is then forwarded to the patient (through TCP).

The design and implementation restrictions are that no two patients can simultaneously communicate with the healthserver (no forking) and that the healthserver cannot book the same time and date for a patient, which has already been booked by another patient.
