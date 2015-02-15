# socketprogramming
In this socket programming two patients try to communicate with the healthserver to fix appointments with a doctor. There are two available doctor, each with thier own availabilities schedule. 
The patient connets to the healthserver using a TCO connection. After receiving multiple information from the command line from the patient, the healthserver connects with a UDP connection to the desired doctor.
The doctor then sends in his availablility, which is forwarded to the patient via the healthserver.

The design and implementation restrictions are that no two patients can simultaneously communicate with the healthserver and that the healthserver cannot book the same time and date to a patient which has already been booked by another patient.
