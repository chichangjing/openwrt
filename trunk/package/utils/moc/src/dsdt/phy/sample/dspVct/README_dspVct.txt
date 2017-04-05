

  DSP VCT Sample program.
  The <madDstVctTest> function is an application sample program of Marvell Phy driver.
The <madDstVctTest> function calls API <madDspVct> to test and validate Marvell Phy function "DSPVCT".
  To support Marvell Phy DSP VCT function, there are two part of function. One is to implement DSP VCT function and collect result data. It is in src/madapi/dspVct/madDspVct.c. Another is to calculate on result data, and to get accuracy VCT result, Cable length.
  There are two modes of API.
  One is "no pending mode". The mode set "wait loop" to get VCT signal completed. The mode is easy to read source code, and to merge to customer’s software.
  Another is "pending mode". The mode set no "wait loop". It returns a "MAD_PENDING" status to application function. It supports real time system to merge the API.
  The sample program to supports both API modes.