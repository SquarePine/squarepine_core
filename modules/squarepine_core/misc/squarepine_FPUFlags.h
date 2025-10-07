/** A tiny controller of CPU floating-point flags.

    @see http://www.website.masmforum.com/tutorials/fptute/fpuchap1.htm#sword
    @see http://www.felixcloutier.com/x86/FSTSW:FNSTSW.xml
    @see http://www.plantation-productions.com/Webster/www.artofasm.com/Linux/HTML/RealArithmetica2.html
    @see http://www.jaist.ac.jp/iscenter-new/mpc/altix/altixdata/opt/intel/vtune/doc/users_guide/mergedProjects/analyzer_ec/mergedProjects/reference_olh/mergedProjects/instructions/instruct32_hh/vc121.htm
*/
class FPUFlags
{
public:
    //==============================================================================
    /** @returns the CPU/FPU's current floating point status.

        When the FPU is initialised, all the bits are reset to 0.

        The B field (bit 15) indicates if the FPU is busy (B=1) while executing an instruction, or is idle (B=0).

        The C3 (bit 14) and C2 - C0 (bits 10-8) fields contain the condition codes following
        the execution of some instructions such as comparisons.
        These codes will be explained in detail for each instruction affecting those fields.

        The TOP field (bits 13-11) is where the FPU keeps track of which of its 80-bit registers is at the TOP.
        The BC numbers described previously for the FPU's internal numbering system of the 80-bit registers
        would be displayed in that field. When the programmer specifies one of the FPU 80-bit registers ST(x) in an instruction,
        the FPU adds (modulo 8) the ST number supplied to the value in this TOP field to
        determine in which of its registers the required data is located.

        The IR field (bit 7) or Interrupt Request gets set to 1 by the FPU while an exception is being handled and
        gets reset to 0 when the exception handling is completed. When the interrupt is masked
        in the Control Word for the FPU to handle the exception, this bit may never be seen to be set while
        stepping through the instructions with a debugger.
        However, if the programmer handles the interrupt, that bit should remain
        set until the interrupt handling routine is completed.

        Bits 6-0 are flags raised by the FPU whenever it detects an exception.
        Those exception flags are cumulative in the sense that, once set (bit=1),
        they are not reset (bit=0) by the result of a subsequent instruction which, by itself,
        would not have raised that flag.
        Those flags can only be reset by either initialising the
        FPU (FINIT instruction) or by explicitly clearing those flags (FCLEX instruction).

        The SF field (bit6) or Stack Fault exception is set whenever an attempt is made to either load
        a value into a register which is not free (the C1 bit would also get set to 1) or pop a value from
        a register which is free (and the C1 bit would get reset to 0). (Such stack fault is also treated as
        an invalid operation and the I field flag bit0 would thus also be set by this exception; see below.)

        The P field (bit5) or Precision exception is set whenever some precision
        is lost by instructions which do exact arithmetic.
        For example, dividing 1 by 10 does not yield an exact value in
        binary arithmetic and would set the P exception flag.
        Another example which sets the P exception flag would be the conversion of
        a REAL10to a REAL4 when some of the least significant bits would be lost. 
        If the FPU handles this exception (when the PM bit is set in the Control Word),
        it rounds the result according to the rounding mode specified in the RC field of the Control Word.

        The U field (bit4) or Underflow exception flag gets set whenever a value is
        too small (without being equal to 0) to be represented properly.
        Each of the floating point formats has a different limit on the smallest number which can be represented.
        The U flag gets set if the result of an operation exceeds that limit.
        For example, dividing a valid very small number by a large number could exceed the limit.
        A valid REAL10 small number may be much smaller than acceptable for the REAL4 or REAL8 formats;
        in such cases, conversion from the former to the latter would also set the U flag. 
        If the FPU handles this exception (when the UM bit is set in the Control Word),
        it would denormalize the value until the exponent is in range or ultimately return a 0.

        The O field (bit3) or Overflow exception flag gets set whenever a value is too large in magnitude to be represented properly.
        Again, each of the floating point formats has a different limit on the largest number which can be represented.
        The O flag gets set if the result of an operation exceeds that limit.
        For example, multiplying a valid very large number by another large number could exceed the limit.
        A valid REAL10 large number may be much larger than acceptable for the REAL4 or REAL8 formats; conversion from the former to the latter would also set the O flag. 
        If the FPU handles this exception (when the OM bit is set in the Control Word),
        it would generate a properly signed INFINITY according to the IC flag of the Control Word.

        The Z field (bit2) or Zero divide exception flag gets set whenever the
        division of a finite non-zero value by 0 is attempted. 
        If the FPU handles this exception (when the ZM bit is set in the Control Word),
        it would generate a properly signed INFINITY according to the XOR of the
        operand signs and then according to the IC flag of the Control Word.

        The D field (bit1) or Denormalised exception flag gets set whenever an instruction attempts
        to operate on a denormalized number or the result of the operation is a denormalized number. 
        If the FPU handles this exception (when the DM bit is set in the Control Word),
        it would simply continue with normal processing and then check for other possible exceptions.

        The I field (bit0) or Invalid operation exception flag gets set whenever an
        operation is considered invalid by the FPU.

        Examples of such operations are: 
        - Stack overflow or underflow 
        - Indeterminate arithmetic such as 0 divided by 0, or subtracting infinity from infinity 
        - Using a Not-A-Number (NAN) as an operand with some instructions 
        - Trying to extract the square root of a negative number

        If the FPU handles this exception (when the IM bit is set in the Control Word),
        it would either return the NAN if one was used as an operand
        (or the larger absolute value if two NANs were used as operands),
        or otherwise return a special "INDEFINITE" NAN.
    */
    static uint32 getFloatingPointStatus();

    //==============================================================================
    /** @returns true if the CPU/FPU is indicating floating-point denormalisation.

        @see getFloatingPointStatus
    */
    static bool hasDenormalisationOccurred();

    /** Clears the floating-point status word.

        @see https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/clear87-clearfp

        @see getFloatingPointStatus, clearIfDenormalised, reset
    */
    static void clear();

    /** Calls clear() if the FPU flags are indicating being denormalised.

        @see clear
    */
    static void clearIfDenormalised();

    /** Resets the floating-point package.

        @see https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/fpreset
    */
    static void reset();

private:
    //==============================================================================
    SQUAREPINE_DECLARE_TOOL_CLASS (FPUFlags)
};
